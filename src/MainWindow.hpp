#pragma once

#include <QMainWindow>

class SourcePanel;
class FormatPanel;
class OutputPanel;

class MainWindow : public QMainWindow {

    Q_OBJECT

public:
    explicit MainWindow(QWidget* _parent = nullptr);
    ~MainWindow() override = default;

private slots:
    void onFilesChanged(const QStringList& _files);
    void onFormatSelected(const QString& _format);
    void onConvertRequested();

private:
    void setupUI();
    void setupLayout();
    void connectPanels();

private:
    SourcePanel* m_sourcePanel;
    FormatPanel* m_formatPanel;
    OutputPanel* m_outputPanel;

    QStringList m_currentFiles;
    QString m_currentFormat;

};