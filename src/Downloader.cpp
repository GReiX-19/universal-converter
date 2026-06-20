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

void Downloader::download(const QString& _url, const QString& _outputDir) {
    m_currenUrl = _url;

    m_outputTemplate = _outputDir + "/%(title)s.%(ext)s";

    QStringList args;
    args << _url << "-o" << m_outputTemplate << "--newline";

    m_process->start("yt-dlp", args);
}

void Downloader::onReadyReadStandardOutput() {
    const QString output = m_process->readAllStandardOutput();

    QRegularExpression percentRx(R"(\[download\]\s+([\d.]+)%)");
    auto match = percentRx.match(output);

    if (match.hasMatch()) {
        const quint32 progress = static_cast<quint32>(match.captured(1).toDouble());
        emit progressChanged(m_currenUrl, progress);
    }
}

void Downloader::onProcessFinished(qint32 _exitCode, QProcess::ExitStatus _exitStatus) {
    const bool success = (_exitCode == 0 and _exitStatus == QProcess::NormalExit);

    emit downloadFinished(m_currenUrl, success);
}