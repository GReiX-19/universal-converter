#include <QApplication>
#include <QMessageBox>

#include "MainWindow.hpp"

int main(int argc, char* argv[]) {
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