#include "Downloader.hpp"

#include <QRegularExpression>
#include <QDir>

Downloader::Downloader(QObject* _parent)
    : QObject(_parent)
    , m_process(new QProcess(this))
{
    connect(m_process, &QProcess::readyReadStandardOutput, this, &Downloader::onReadyReadStandardOutput);
    connect(m_process, &QProcess::finished, this, &Downloader::onProcessFinished);
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
        m_process->terminate();
        if (!m_process->waitForFinished(2000))
            m_process->kill();
    }

    m_busy = false;
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

    const QString outputTemplate = m_currentTask.outputDir + "/%(title)s.%(ext)s";

    QStringList args;
    args << m_currentTask.url << "-o" << outputTemplate << "--newline";

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