#ifndef CUSTOMTABBAR_H
#define CUSTOMTABBAR_H

#include <QTabBar>
#include "customtabwidget.h"

class CustomTabBar : public QTabBar
{
    Q_OBJECT
public:
    explicit CustomTabBar(QWidget *parent = 0);

protected slots:
    void mouseDoubleClickEvent(QMouseEvent *e);
    void mousePressEvent(QMouseEvent *e);

private:
    CustomTabWidget * parent;

protected:
    void focusInEvent(QFocusEvent *);
};

#endif // CUSTOMTABBAR_H
