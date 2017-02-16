#include "breezeblocksapp.h"


MainWindow *BreezeBlocksApp::getWindow() const{ return window; }

void BreezeBlocksApp::setWindow(MainWindow *value)
{
    window = value;
    //connect(this, SIGNAL(focusChanged(QWidget*,QWidget*)), window, SLOT(warnAboutConflict(QWidget*,QWidget*)));
}

BreezeBlocksApp::BreezeBlocksApp(int &argc, char **argv, int flags):QApplication(argc, argv, flags)
{}


