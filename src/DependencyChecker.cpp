#include "DependencyChecker.hpp"

#include <QProcess>
#include <QCoreApplication>
#include <QDebug>
#include <QSettings>
#include <QDir>
#include <optional>
#include <Windows.h>

#ifdef Q_OS_WIN
static QString libreOfficePathFromRegistry() {
    QSettings registry("HKEY_LOCAL_MACHINE\\SOFTWARE\\LibreOffice\\UNO\\InstallPath", QSettings::NativeFormat);

    const QString installPath = registry.value(".").toString();
    
    if (installPath.isEmpty())
        return {};

    return QDir::toNativeSeparators(installPath + "/soffice.exe");
}
#endif

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

DependencyStatus DependencyChecker::check(Dependency _dep) {
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

QMap<Dependency, DependencyStatus> DependencyChecker::checkAll() {
    QMap<Dependency, DependencyStatus> result;
    result[Dependency::FFmpeg] = check(Dependency::FFmpeg);
    result[Dependency::LibreOffice] = check(Dependency::LibreOffice);
    result[Dependency::YtDlp] = check(Dependency::YtDlp);
    return result;
}