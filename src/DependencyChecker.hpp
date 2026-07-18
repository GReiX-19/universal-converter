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
    QString resolvedPath;
};

class DependencyChecker {
public:
    static QMap<Dependency, DependencyStatus> checkAll();

private:
    static QString executableName(Dependency _dep);
    static QString installHintFor(Dependency _dep);

    static DependencyStatus fullCheck(Dependency _dep);

    static QString cacheFilePath();
    static bool loadCache(QMap<Dependency, DependencyStatus>& _out);
    static void saveCache(const QMap<Dependency, DependencyStatus>& _statuses);

    static bool quickValidate(const DependencyStatus& _cached);

    static QString dependencyKey(Dependency _dep);

};