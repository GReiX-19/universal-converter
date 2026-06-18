#include "Converter.hpp"
#include "ConversionRules.hpp"

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

void Converter::enqueue(const ConversionTask& _task) {
    m_queue.enqueue(_task);

    if (!m_busy)
        startNext();
}

void Converter::startNext() {
    if (m_queue.isEmpty()) {
        m_busy = false;
        emit allTasksFinished();
        return;
    }

    m_busy = true;
    m_duration = 0.0;
    m_currentTask = m_queue.dequeue();

    const FileCategory category = ConversionRules::categoryOf(m_currentTask.inputPath);
    const ConverterTool tool = ConversionRules::toolFor(category, m_currentTask.format);

    QStringList args;

    switch (tool) {
    case ConverterTool::FFmpeg:
        args << "-i" << m_currentTask.inputPath << "-y" << m_currentTask.outputPath;
        m_process->start("ffmpeg", args);
        break;
    case ConverterTool::LibreOffice:
        args << "--headless" << "--convert-to" << m_currentTask.format.toLower()
            << "--outdir" << QFileInfo(m_currentTask.outputPath).dir().absolutePath()
            << m_currentTask.inputPath;
        m_process->start("soffice", args);
        break;
    case ConverterTool::None:
        emit taskFinished(QFileInfo(m_currentTask.inputPath).fileName(), false);
        startNext();
        break;
    }
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

        int progress = static_cast<int>((current / m_duration) * 100);

        emit progressChanged(QFileInfo(m_currentTask.inputPath).fileName(), progress);
    }
}

void Converter::onProcessFinished(qint32 _exitCode, QProcess::ExitStatus _exitStatus) {
    const bool success = (_exitCode == 0 and _exitStatus == QProcess::NormalExit);
    const QString fileName = QFileInfo(m_currentTask.inputPath).fileName();

    emit progressChanged(fileName, 100);
    emit taskFinished(fileName, success);

    startNext();
}