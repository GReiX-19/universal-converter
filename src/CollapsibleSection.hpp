#pragma once

#include <QWidget>
#include <QToolButton>

class QParallelAnimationGroup;

class CollapsibleSection : public QWidget {

    Q_OBJECT

public:
    explicit CollapsibleSection(const QString& _title, QWidget* _parent = nullptr);

    void setContentLayout(QLayout* _contentLayout);
    void setExpanded(bool _expanded, bool _animate = true);
    void setHightlighted(bool _highlighted);
    bool isExpanded() const;

private:
    void toggle();
    void updateAnimationRange();

private:
    QToolButton* m_headerButton;
    QWidget* m_contentArea;
    QParallelAnimationGroup* m_animation;
    bool m_expanded = false;
    int m_collapsedHeight = 0;
};