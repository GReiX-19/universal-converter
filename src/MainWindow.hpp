#pragma once

#include <QMainWindow>

#include "Converter.hpp"
#include "Downloader.hpp"

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
    void onFileSelected(const QString& _path);
    void onFormatSelected(const QString& _format);
    void onConvertRequested();
    void onOutputDirRequested();
    void onDownloadFinished(const QString& _url, bool _success);
    void onClearRequested();

private:
    void setupUI();
    void setupLayout();
    void connectPanels();

private:
    SourcePanel* m_sourcePanel;
    FormatPanel* m_formatPanel;
    OutputPanel* m_outputPanel;

    QStringList m_currentFiles;
    QString m_activeFilePath;
    QString m_outputDir;

    Converter* m_converter;
    Downloader* m_downloader;
};