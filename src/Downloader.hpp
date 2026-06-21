#pragma once

#include <QObject>
#include <QProcess>
#include <QString>

class Downloader : public QObject {

    Q_OBJECT

public:
    explicit Downloader(QObject* _parent = nullptr);

    void download(const QString& _url, const QString& _outputDir);
    void cancel();

signals:
    void progressChanged(const QString& _label, quint32 _progress);
    void downloadFinished(const QString& _filePath, bool _success);

private slots:
    void onReadyReadStandardOutput();
    void onProcessFinished(qint32 _exitCode, QProcess::ExitStatus _exitStatus);

private:
    QProcess* m_process;
    QString m_currentUrl;
    QString m_outputTemplate;
    bool m_cancelled = false;
};