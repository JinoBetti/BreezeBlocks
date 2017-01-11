#ifndef CUSTOMTABWIDGET_H
#define CUSTOMTABWIDGET_H

#include <QTabWidget>

class CustomTabWidget : public QTabWidget
{
    Q_OBJECT
public:
    explicit CustomTabWidget(QWidget *parent = 0);
};

#endif // CUSTOMTABWIDGET_H
