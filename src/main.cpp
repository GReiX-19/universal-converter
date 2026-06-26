#include <QApplication>
#include <QMessageBox>

#include "MainWindow.hpp"

int main(int argc, char* argv[]) {
    QApplication app(argc, argv);

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