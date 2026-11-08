#ifndef SIDEBAR_H
#define SIDEBAR_H

#include <QWidget>

class QButtonGroup;

// Left navigation rail: branding + the four section buttons
// (Overview / Simulator / Algorithms / About).
class Sidebar : public QWidget
{
    Q_OBJECT

public:
    explicit Sidebar(QWidget *parent = nullptr);

    void setActivePage(int index);

signals:
    void pageSelected(int index);

private:
    QButtonGroup *m_group = nullptr;
};

#endif // SIDEBAR_H
