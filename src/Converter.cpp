#include "Converter.hpp"

#include <QFileInfo>
#include <QDir>
#include <QRegularExpression>

Converter::Converter(QObject* _parent)
    : QObject(_parent)
    , m_process(new QProcess(this))
{
    connect(m_process, &QProcess::readyReadStandardError, this, &Converter::onReadyReadStandardError);
    connect(m_process, &QProcess::finished, this, &Converter::onProcessFinished);
}

void Converter::convert(const ConversionTask& _task) {
    m_currentTask = _task;
    m_duration = 0;

    QStringList args;
    args << "-i" << _task.inputPath << "-y" << _task.outputPath;

    m_process->start("ffmpeg", args);
}

void Converter::onReadyReadStandardError() {
    const QString output = m_process->readAllStandardError();

    if (m_duration == 0) {
        QRegularExpression durationRX(R"(Duration:\s*(\d+):(\d+):(\d+)\.(\d+))");
        auto match = durationRX.match(output);

        if (match.hasMatch()) {
            int h = match.captured(1).toInt();
            int m = match.captured(2).toInt();
            int s = match.captured(3).toInt();
            int cs = match.captured(4).toInt();
            m_duration = (h * 3600) + (m * 60) + s + cs / 100.0;
        }
    }

    QRegularExpression timeRx(R"(time=(\d+):(\d+):(\d+)\.(\d+))");
    auto match = timeRx.match(output);

    if (match.hasMatch() and m_duration > 0) {
        int h = match.captured(1).toInt();
        int m = match.captured(2).toInt();
        int s = match.captured(3).toInt();
        int cs = match.captured(4).toInt();
        double current = (h * 3600) + (m * 60) + s + cs / 100.0;

        int percent = static_cast<int>((current / m_duration) * 100);

        emit progressChanged(QFileInfo(m_currentTask.inputPath).fileName(), percent);
    }
}

void Converter::onProcessFinished(qint32 _exitCode, QProcess::ExitStatus _exitStatus) {
    const bool success = (_exitCode == 0 and _exitStatus == QProcess::NormalExit);
    const QString fileName = QFileInfo(m_currentTask.inputPath).fileName();

    emit progressChanged(fileName, 100);
    emit taskFinished(fileName, success);
}

QString Converter::buildOutputPath(const QString& _inputPath, const QString& _format) {
    QFileInfo info(_inputPath);
    return info.dir().filePath(info.baseName() + _format.toLower());
}