#pragma once

#include <QString>
#include <QMap>

enum class Dependency {
    FFmpeg,
    LibreOffice,
    YtDlp
};

struct DependencyStatus {
    bool found;
    QString version;
    QString installHint;
};

class DependencyChecker {
public:
    static DependencyStatus check(Dependency _dep);
    static QMap<Dependency, DependencyStatus> checkAll();

private:
    static QString executableName(Dependency _dep);
    static QString installHintFor(Dependency _dep);
};