#pragma once

#include <QWidget>
#include <QListWidget>
#include <QPushButton>

struct OutputEntry {
    QString fileName;
    QString outputPath;
    quint32 progress;
};

class OutputPanel : public QWidget {

    Q_OBJECT

public:
    explicit OutputPanel(QWidget* _parent = nullptr);

    void addEntry(const OutputEntry& _entry);
    void updateProgress(const QString& _fileName, quint32 _progress);

private:
    void setupUI();

private:
    QListWidget* m_outputList;
    QPushButton* m_downloadButton;
};