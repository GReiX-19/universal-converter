#pragma once

#include "ConversionRules.hpp"

#include <QWidget>
#include <QListWidget>
#include <QPushButton>
#include <QDragEnterEvent>
#include <QDropEvent>
#include <QList>

struct SourceFile {
    QString path;
    QString selectedFormat;
    bool isUrl = false;
};

class SourcePanel : public QWidget {

    Q_OBJECT

public:
    explicit SourcePanel(QWidget* _parent = nullptr);

    QList<SourceFile> files() const;
    void setFormatForFile(const QString& _path, const QString& _format);
    QString activeFilePath() const;

signals:
    void filesChanged(const QStringList& _files);
    void fileSelected(const QString& _path);

protected:
    void dragEnterEvent(QDragEnterEvent* _event) override;
    void dropEvent(QDropEvent* _event) override;

private slots:
    void onAddClicked();
    void onRemoveClicked();
    void onItemClicked(QListWidgetItem* _item);

private:
    void setupUI();
    bool isUrl(const QString& _text) const;
    void addFile(const QString& _path);
    void emitFilesChanged();
    void rebuildList();
    QString sectionTitleFor(FileCategory _category) const;

private:
    QListWidget* m_fileList;
    QPushButton* m_addButton;
    QPushButton* m_removeButton;
    QList<SourceFile> m_sourceFiles;
};