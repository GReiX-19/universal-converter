#pragma once

#include <QWidget>
#include <QLabel>
#include <QListWidget>
#include <QPushButton>
#include <QProgressBar>
#include <QMap>

struct OutputEntry {
    QString fileName;
    QString outputPath;
    quint32 progress;
    bool indeterminate = false;
};

class OutputPanel : public QWidget {

    Q_OBJECT

public:
    explicit OutputPanel(QWidget* _parent = nullptr);

    void addEntry(const OutputEntry& _entry);
    void updateProgress(const QString& _fileName, quint32 _progress);

public slots:
    void onTaskFinished(const QString& _fileName, bool _success);

signals:
    void outputDirRequested();

private:
    void setupUI();

private:
    QListWidget* m_outputList;
    QPushButton* m_outputDirButton;
    QPushButton* m_clearButton;
    QMap<QString, QProgressBar*> m_progressBars;
    QMap<QString, QLabel*> m_statusLabels;
};