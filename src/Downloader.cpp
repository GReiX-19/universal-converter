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

void Downloader::download(const QString& _url, const QString& _outputDir, const QString& _format) {
    m_currentUrl = _url;

    m_outputTemplate = _outputDir + "/%(title)s.%(ext)s";

    QStringList args;
    args << _url << "-o" << m_outputTemplate << "--newline";

    if (isAudioFormat(_format)) {
        args << "--extract-audio" << "--audio-format" << _format.toLower();
    }
    else {
        args << "--recode-video" << _format.toLower();
    }

    m_process->start("yt-dlp", args);
}
void Downloader::cancel() {
    m_cancelled = true;

    if (m_process->state() != QProcess::NotRunning) {
        m_process->terminate();
        if (m_process->waitForFinished(2000))
            m_process->kill();
    }
}

void Downloader::onReadyReadStandardOutput() {
    if (m_cancelled)
        return;

    const QString output = m_process->readAllStandardOutput();

    QRegularExpression percentRx(R"(\[download\]\s+([\d.]+)%)");
    auto match = percentRx.match(output);

    if (match.hasMatch()) {
        const quint32 progress = static_cast<quint32>(match.captured(1).toDouble());
        emit progressChanged(m_currentUrl, progress);
    }
}
void Downloader::onProcessFinished(qint32 _exitCode, QProcess::ExitStatus _exitStatus) {
    if (m_cancelled) {
        m_cancelled = false;
        return;
    }

    const bool success = (_exitCode == 0 and _exitStatus == QProcess::NormalExit);

    emit downloadFinished(m_currentUrl, success);
}

bool Downloader::isAudioFormat(const QString& _format) const {
    const QString& fmt = _format.toLower();
    return fmt == "mp3" or fmt == "wav";
}