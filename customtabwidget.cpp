#include "customtabwidget.h"
#include "customtabbar.h"

CustomTabWidget::CustomTabWidget(QWidget *parent) :
    QTabWidget(parent)
{
    CustomTabBar* myTab = new CustomTabBar(this);
    setTabBar (myTab);
    setStyleSheet("background-color: lightgray"); 
}
