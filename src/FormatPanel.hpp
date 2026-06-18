#pragma once

#include <QWidget>
#include <QListWidget>
#include <QPushButton>

class FormatPanel : public QWidget
{

    Q_OBJECT

public:
    explicit FormatPanel(QWidget *_parent = nullptr);

signals:
    void formatSelected(const QString &_format);

private slots:
    void onFormatClicked(QListWidgetItem *_item);

private:
    void setupUI();
    void addSection(const QString &_title, const QStringList &_formats);

private:
    QListWidget *m_formatList;
    QPushButton *m_convertButton;
};