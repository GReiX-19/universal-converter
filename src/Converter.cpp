#include "Converter.hpp"

#include <QFileInfo>
#include <QDir>

Converter::Converter(QObject* _parent)
    : QObject(_parent)
    , m_process(new QProcess(this))
{
    connect(m_process, &QProcess::readyReadStandardError, this, &Converter::onReadyReadStandardError);
    connect(m_process, &QProcess::finished, this, &Converter::onProcessFinished);
}

void Converter::convert(const ConversionTask& _task) {
    m_currentTask = _task;

    QStringList args;
    args << "-i" << _task.inputPath << "-y" << _task.outputPath;

    m_process->start("ffmpeg", args);
}

void Converter::onReadyReadStandardError() {
    const QString output = m_process->readAllStandardError();
    qDebug() << output;
}

void Converter::onProcessFinished(qint32 _exitCode, QProcess::ExitStatus _exitStatus) {
    const bool success = (_exitCode == 0 and _exitStatus == QProcess::NormalExit);
    const QString fileName = QFileInfo(m_currentTask.inputPath).fileName();

    emit progressChanged(fileName, 100);
    emit taskFinished(fileName, success);
}

QString Converter::buildOutputPath(const QString& _inputPath, const QString& _format) {
    QFileInfo info(_inputPath);
    return info.dir().filePath(info.baseName() + "." + _format.toLower());
}