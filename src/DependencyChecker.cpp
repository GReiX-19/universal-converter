#include "DependencyChecker.hpp"

#include <QProcess>

QString DependencyChecker::executableName(Dependency _dep) {
    switch (_dep) {
    case Dependency::FFmpeg: return "ffmpeg";
    case Dependency::LibreOffice:
#ifdef Q_OS_WIN
        return "soffice.exe";
#else
        return "soffice";
#endif
    case Dependency::YtDlp: return "yt-dlp";
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

    QProcess process;
    process.start(exe, { "--version" });
    const bool started = process.waitForStarted(1000);

    if (!started) {
        return { false, {} , installHintFor(_dep) };
    }

    process.waitForFinished(3000);
    const QString output = process.readAllStandardOutput();

    const QString version = output.split('\n').value(0).trimmed();

    return { true, version, {} };
}

QMap<Dependency, DependencyStatus> DependencyChecker::checkAll() {
    QMap<Dependency, DependencyStatus> result;
    result[Dependency::FFmpeg] = check(Dependency::FFmpeg);
    result[Dependency::LibreOffice] = check(Dependency::LibreOffice);
    result[Dependency::YtDlp] = check(Dependency::YtDlp);
    return result;
}