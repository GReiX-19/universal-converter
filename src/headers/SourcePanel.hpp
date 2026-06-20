#pragma once

#include <QWidget>
#include <QListWidget>
#include <QPushButton>

class SourcePanel : public QWidget {

    Q_OBJECT

public:
    explicit SourcePanel(QWidget* _parent = nullptr);

signals:
    void filesChanged(const QStringList& _files);

private slots:
    void onAddClicked();
    void onRemoveClicked();

private:
    void setupUI();

private:
    QListWidget* m_fileList;
    QPushButton* m_addButton;
    QPushButton* m_removeButton;

};