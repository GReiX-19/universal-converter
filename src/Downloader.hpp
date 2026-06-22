#pragma once

#include <QObject>
#include <QProcess>
#include <QString>
#include <QQueue>

struct DownloadTask {
    QString url;
    QString outputDir;
    QString format;
};

class Downloader : public QObject {

    Q_OBJECT

public:
    explicit Downloader(QObject* _parent = nullptr);

    void enqueue(const DownloadTask& _task);
    void cancelAll();

signals:
    void progressChanged(const QString& _label, quint32 _progress);
    void downloadFinished(const QString& _filePath, bool _success);
    void allDownloadsFinished();

private slots:
    void onReadyReadStandardOutput();
    void onProcessFinished(qint32 _exitCode, QProcess::ExitStatus _exitStatus);

private:
    void startNext();
    bool isAudioFormat(const QString& _format) const;

private:
    QProcess* m_process;
    DownloadTask m_currentTask;
    QQueue<DownloadTask> m_queue;
    bool m_cancelled = false;
    bool m_busy = false;
};