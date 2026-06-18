#pragma once

#include <QObject>
#include <QProcess>
#include <QStringList>
#include <QQueue>

struct ConversionTask {
    QString inputPath;
    QString outputPath;
    QString format;
};

class Converter : public QObject {

    Q_OBJECT

public:
    explicit Converter(QObject* _parent = nullptr);

    void enqueue(const ConversionTask& _task);

signals:
    void progressChanged(const QString& _fileName, quint32 _progress);
    void taskFinished(const QString& _fileName, bool _success);
    void allTasksFinished();

private slots:
    void onReadyReadStandardError();
    void onProcessFinished(qint32 _exitCode, QProcess::ExitStatus _exitStatus);

private:
    void startNext();

private:
    QProcess* m_process;
    ConversionTask m_currentTask;
    QQueue<ConversionTask> m_queue;
    double m_duration = 0.0;
    bool m_busy = false;
};