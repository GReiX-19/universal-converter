#include "Downloader.hpp"

#include <QRegularExpression>
#include <QDir>
#include <QMessageBox>

#ifdef Q_OS_UNIX
#include <csignal>
#include <sys/types.h>
#endif

Downloader::Downloader(QObject* _parent)
    : QObject(_parent)
    , m_process(new QProcess(this))
    , m_tempDir(QDir::tempPath() + "/universal-converter-temp")
{
    connect(m_process, &QProcess::readyReadStandardOutput, this, &Downloader::onReadyReadStandardOutput);
    connect(m_process, &QProcess::finished, this, &Downloader::onProcessFinished);

#ifdef Q_OS_UNIX
    m_process->setChildProcessModifier([]() {
        ::setpgid(0, 0);
        });
#endif
}

void Downloader::enqueue(const DownloadTask& _task) {
    m_queue.enqueue(_task);

    if (!m_busy)
        startNext();
}
void Downloader::cancelAll() {
    m_cancelled = true;
    m_queue.clear();

    if (m_process->state() != QProcess::NotRunning) {
        const qint64 pid = m_process->processId();

#ifdef Q_OS_UNIX
        if (pid > 0)
            ::kill(-static_cast<pid_t>(pid), SIGTERM);
#else
        m_process->terminate();
#endif

        if (!m_process->waitForFinished(2000)) {
#ifdef Q_OS_UNIX
            if (pid > 0)
                ::kill(-static_cast<pid_t>(pid), SIGKILL);
#else
            m_process->kill();
#endif
        }
    }

    m_busy = false;

    if (m_tempDir.isEmpty() or !m_tempDir.startsWith(QDir::tempPath())) {
        qWarning() << "Refusing to remove suspicious temp path: " << m_tempDir;
        return;
    }

    QDir(m_tempDir).removeRecursively();
}

void Downloader::onReadyReadStandardOutput() {
    if (m_cancelled)
        return;

    const QString output = m_process->readAllStandardOutput();

    QRegularExpression percentRx(R"(\[download\]\s+([\d.]+)%)");
    auto match = percentRx.match(output);

    if (match.hasMatch()) {
        const quint32 progress = static_cast<quint32>(match.captured(1).toDouble());
        emit progressChanged(m_currentTask.url, progress);
    }
}
void Downloader::onProcessFinished(qint32 _exitCode, QProcess::ExitStatus _exitStatus) {
    if (m_cancelled) {
        m_cancelled = false;
        return;
    }

    const bool success = (_exitCode == 0 and _exitStatus == QProcess::NormalExit);

    emit downloadFinished(m_currentTask.url, success);

    startNext();
}

void Downloader::startNext() {
    if (m_queue.isEmpty()) {
        m_busy = false;
        emit allDownloadsFinished();
        return;
    }

    m_busy = true;
    m_currentTask = m_queue.dequeue();

    QDir().mkpath(m_tempDir);

    QStringList args;
    args << m_currentTask.url << "-o" << "%(title)s.%(ext)s"
        << "--paths" << ("home:" + m_currentTask.outputDir)
        << "--paths" << ("temp:" + m_tempDir)
        << "--newline";

    if (isAudioFormat(m_currentTask.format)) {
        args << "--extract-audio" << "--audio-format" << m_currentTask.format.toLower();
    }
    else {
        args << "--recode-video" << m_currentTask.format.toLower();
    }

    m_process->start("yt-dlp", args);
}
bool Downloader::isAudioFormat(const QString& _format) const {
    const QString& fmt = _format.toLower();
    return fmt == "mp3" or fmt == "wav";
}