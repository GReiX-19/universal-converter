#include "DependencyChecker.hpp"

#include <QProcess>
#include <QCoreApplication>
#include <QDebug>
#include <QSettings>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QStandardPaths>
#include <optional>
#ifdef Q_OS_WIN
#include <Windows.h>
#endif

#ifdef Q_OS_WIN
static QString libreOfficePathFromRegistry() {
    QSettings registry("HKEY_LOCAL_MACHINE\\SOFTWARE\\LibreOffice\\UNO\\InstallPath", QSettings::NativeFormat);

    const QString installPath = registry.value(".").toString();

    if (installPath.isEmpty())
        return {};

    return QDir::toNativeSeparators(installPath + "/soffice.exe");
}
#endif

QMap<Dependency, DependencyStatus> DependencyChecker::checkAll() {
    QMap<Dependency, DependencyStatus> result;

    if (loadCache(result)) {
        qWarning() << "Dependency cahce hit - skipping full checks";
        return result;
    }

    result.clear();
    for (auto dep : { Dependency::FFmpeg, Dependency::LibreOffice, Dependency::YtDlp }) {
        result[dep] = fullCheck(dep);
    }

    saveCache(result);

    return result;
}

QString DependencyChecker::executableName(Dependency _dep) {
    switch (_dep) {
    case Dependency::FFmpeg:
#ifdef Q_OS_WIN
        return "ffmpeg.exe";
#else
        return "ffmpeg";
#endif
    case Dependency::LibreOffice:
#ifdef Q_OS_WIN
        return "soffice.exe";
#else
        return "soffice";
#endif
    case Dependency::YtDlp:
#ifdef Q_OS_WIN
        return "yt-dlp.exe";
#else
        return "yt-dlp";
#endif
    }

    return {};
}

QString DependencyChecker::installHintFor(Dependency _dep) {
#ifdef Q_OS_WIN
    switch (_dep) {
    case Dependency::FFmpeg: return "Download from ffmpeg.org or via winget: winget install ffmpeg";
    case Dependency::LibreOffice: return "Download from libreoffice.org";
    case Dependency::YtDlp: return "winget install yt-dlp";
    }
#else
    switch (_dep) {
    case Dependency::FFmpeg: return "sudo dnf install ffmpeg (or apt install ffmpeg)";
    case Dependency::LibreOffice: return "sudo dnf install libreoffice (or apt install libreoffice)";
    case Dependency::YtDlp: return "sudo dnf install yt-dlp (or pip install yt-dlp)";
    }
#endif

    return {};
    }

DependencyStatus DependencyChecker::fullCheck(Dependency _dep) {
    const QString exe = executableName(_dep);

    qDebug() << "Checking for " << exe << "...";

    auto tryRun = [](const QString& _path) -> std::optional<QString> {
        QProcess process;
        process.setProcessChannelMode(QProcess::MergedChannels);

#ifdef Q_OS_WIN
        process.setCreateProcessArgumentsModifier([](QProcess::CreateProcessArguments* args) {
            args->flags |= CREATE_NO_WINDOW;
            args->startupInfo->dwFlags |= STARTF_USESHOWWINDOW;
            args->startupInfo->wShowWindow = SW_HIDE;
            });
#endif

        process.start(_path, { "--version" });
        if (!process.waitForStarted(1000))
            return std::nullopt;

        process.waitForFinished(3000);
        const QString output = process.readAllStandardOutput();

        if (process.state() != QProcess::NotRunning)
            process.kill();

        return output.split('\n').value(0).trimmed();
        };

    if (auto version = tryRun(exe)) {
        return { true, *version, {}, exe };
        }

    if (_dep == Dependency::FFmpeg or _dep == Dependency::YtDlp) {
        const QString bundledPath = QCoreApplication::applicationDirPath() + "/" + exe;
        if (auto version = tryRun(bundledPath)) {
            return { true, *version, {}, bundledPath };
        }
    }

#ifdef Q_OS_WIN
    if (_dep == Dependency::LibreOffice) {
        const QString regPath = libreOfficePathFromRegistry();
        if (!regPath.isEmpty() and QFile::exists(regPath)) {
            if (auto version = tryRun(regPath)) {
                return { true, *version, {}, regPath };
            }
        }
    }
#endif

    return { false, {}, installHintFor(_dep), {} };
}

QString DependencyChecker::cacheFilePath() {
    const QString dir = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    QDir().mkpath(dir);

    return dir + "/dependency_cache.ini";
}
bool DependencyChecker::loadCache(QMap<Dependency, DependencyStatus>& _out) {
    QSettings settings(cacheFilePath(), QSettings::IniFormat);

    const QString cachedAppVersion = settings.value("General/appVersion").toString();
    if (cachedAppVersion != QCoreApplication::applicationVersion()) {
        qWarning() << "Dependency cache stale (app version changed), full recheck";
        return false;
    }

    for (auto dep : { Dependency::FFmpeg, Dependency::YtDlp, Dependency::LibreOffice }) {
        settings.beginGroup(dependencyKey(dep));
        if (!settings.contains("resolvedPath")) {
            settings.endGroup();
            return false;
        }

        DependencyStatus status;
        status.found = settings.value("found").toBool();
        status.version = settings.value("version").toString();
        status.resolvedPath = settings.value("resolvedPath").toString();
        settings.endGroup();

        if (!quickValidate(status)) {
            qWarning() << "Cached path invalid for " << dependencyKey(dep) << "- full recheck";
            return false;
        }

        _out[dep] = status;
    }

    return true;
}
void DependencyChecker::saveCache(const QMap<Dependency, DependencyStatus>& _statuses) {
    QSettings settings(cacheFilePath(), QSettings::IniFormat);
    settings.clear();
    settings.setValue("General/appVersion", QCoreApplication::applicationVersion());

    for (auto it = _statuses.constBegin(); it != _statuses.constEnd(); ++it) {
        settings.beginGroup(dependencyKey(it.key()));
        settings.setValue("found", it.value().found);
        settings.setValue("version", it.value().version);
        settings.setValue("resolvedPath", it.value().resolvedPath);
        settings.endGroup();
    }
    settings.sync();
}

bool DependencyChecker::quickValidate(const DependencyStatus& _cached) {
    if (!_cached.found or _cached.resolvedPath.isEmpty())
        return false;

    const QFileInfo info(_cached.resolvedPath);
    if (info.isAbsolute()) {
        return info.exists() and info.isExecutable();
    }

    return !QStandardPaths::findExecutable(_cached.resolvedPath).isEmpty();
}

QString DependencyChecker::dependencyKey(Dependency _dep) {
    switch (_dep) {
    case Dependency::FFmpeg: return "FFmpeg";
    case Dependency::LibreOffice: return "LibreOffice";
    case Dependency::YtDlp: return "YtDlp";
    }

    return {};
}