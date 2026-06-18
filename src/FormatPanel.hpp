#pragma once

#include "ConversionRules.hpp"

#include <QWidget>
#include <QListWidget>
#include <QPushButton>

class FormatPanel : public QWidget
{

    Q_OBJECT

public:
    explicit FormatPanel(QWidget* _parent = nullptr);

    void updateCompatibility(const QStringList& _files);
    void resetCompatibility();

signals:
    void formatSelected(const QString& _format);
    void convertRequested();

public slots:
    void setConverterEnabled(bool enabled);

private slots:
    void onFormatClicked(QListWidgetItem* _item);

private:
    void setupUI();
    void addSection(const QString& _title, const QStringList& _formats);

private:
    QListWidget* m_formatList;
    QPushButton* m_convertButton;
};