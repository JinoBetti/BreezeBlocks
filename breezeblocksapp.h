#ifndef BREEZEBLOCKSAPP_H
#define BREEZEBLOCKSAPP_H
#include<QApplication>
#include "mainwindow.h"

class BreezeBlocksApp : public QApplication
{
private:
    MainWindow* window;
public:
    BreezeBlocksApp(int &argc, char **argv, int = ApplicationFlags);

    MainWindow *getWindow() const;
    void setWindow(MainWindow *value);
};

#endif // BREEZEBLOCKSAPP_H
