#pragma once

#include <QObject>
#include <QProcess>
#include <QStringList>

struct ConversionTask {
    QString inputPath;
    QString outputPath;
    QString format;
};

class Converter : public QObject {

    Q_OBJECT

public:
    explicit Converter(QObject* _parent = nullptr);

    void convert(const ConversionTask& _task);

signals:
    void progressChanged(const QString& _fileName, quint32 _progress);
    void taskFinished(const QString& _fileName, bool _success);

private slots:
    void onReadyReadStandardError();
    void onProcessFinished(qint32 _exitCode, QProcess::ExitStatus _exitStatus);

private:
    QString buildOutputPath(const QString& _inputPath, const QString& _format);

private:
    QProcess* m_process;
    ConversionTask m_currentTask;

};