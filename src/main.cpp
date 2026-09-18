#include <QApplication>
#include <QMessageBox>
#include <QGuiApplication>
#include <QFile>

#include "MainWindow.hpp"

int main(int argc, char* argv[]) {
    QGuiApplication::setDesktopFileName("UniversalConverter");
    QApplication app(argc, argv);

    QCoreApplication::setApplicationName("UniversalConverter");
    QCoreApplication::setApplicationVersion(PROJECT_VERSION);

    QIcon appIcon;
    appIcon.addFile(":/icons/icons/universal-converter-icon-16.png");
    appIcon.addFile(":/icons/icons/universal-converter-icon-32.png");
    appIcon.addFile(":/icons/icons/universal-converter-icon-48.png");
    appIcon.addFile(":/icons/icons/universal-converter-icon-128.png");
    appIcon.addFile(":/icons/icons/universal-converter-icon-256.png");

    app.setWindowIcon(appIcon);

    QFile styleFile(":/styles/style.qss");
    if (styleFile.open(QFile::ReadOnly | QFile::Text)) {
        QTextStream stream(&styleFile);
        qApp->setStyleSheet(stream.readAll());
    }

    auto statuses = DependencyChecker::checkAll();

    QStringList missing;
    for (auto it = statuses.cbegin(); it != statuses.cend(); ++it) {
        if (!it.value().found)
            missing << it.value().installHint;
    }

    if (!missing.isEmpty()) {
        QMessageBox::warning(nullptr, "Absent dependencies", "The following utilities were not found:\n\n" + missing.join("\n\n"));
    }

    MainWindow window(statuses);
    window.show();

    return app.exec();
}