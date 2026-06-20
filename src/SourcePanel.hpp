#pragma once

#include <QWidget>
#include <QListWidget>
#include <QPushButton>
#include <QDragEnterEvent>
#include <QDropEvent>

class SourcePanel : public QWidget {

    Q_OBJECT

public:
    explicit SourcePanel(QWidget* _parent = nullptr);

signals:
    void filesChanged(const QStringList& _files);
    void urlDropped(const QString& _url);

protected:
    void dragEnterEvent(QDragEnterEvent* _event) override;
    void dropEvent(QDropEvent* _event) override;

private slots:
    void onAddClicked();
    void onRemoveClicked();

private:
    void setupUI();
    bool isUrl(const QString& _text) const;

private:
    QListWidget* m_fileList;
    QPushButton* m_addButton;
    QPushButton* m_removeButton;
};