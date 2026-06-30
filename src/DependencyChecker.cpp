#include "DependencyChecker.hpp"

#include <QProcess>
#include <QCoreApplication>
#include <QDebug>
#include <optional>

QString DependencyChecker::executableName(Dependency _dep) {
    switch (_dep) {
    case Dependency::FFmpeg: 
        return "ffmpeg";
    case Dependency::LibreOffice:
#ifdef Q_OS_WIN
        return "soffice.exe";
#else
        return "soffice";
#endif
    case Dependency::YtDlp:
        return "yt-dlp";
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
        process.start(_path, { "--version" });
        if (!process.waitForFinished(3000))
            return std::nullopt;

        process.waitForFinished(2000);
        const QString output = process.readAllStandardOutput();
        return output.split('\n').value(0).trimmed();
    };

    if (auto version = tryRun(exe))
        return { true, *version, {}, exe };

    if (_dep == Dependency::FFmpeg or _dep == Dependency::YtDlp) {
        const QString bundledPath = QCoreApplication::applicationDirPath() + "/" + exe;
        if (auto version = tryRun(bundledPath))
            return { true, *version, {}, bundledPath };
    }

    return { false, {}, installHintFor(_dep), {} };
}

QMap<Dependency, DependencyStatus> DependencyChecker::checkAll() {
    QMap<Dependency, DependencyStatus> result;
    result[Dependency::FFmpeg] = check(Dependency::FFmpeg);
    result[Dependency::LibreOffice] = check(Dependency::LibreOffice);
    result[Dependency::YtDlp] = check(Dependency::YtDlp);
    return result;
}