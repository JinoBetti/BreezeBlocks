#ifndef SAVEDIALOG_H
#define SAVEDIALOG_H

#include <QDialog>
#include "mainwindow.h"

namespace Ui {
class SaveDialog;
}

class SaveDialog : public QDialog
{
    Q_OBJECT

public:
    explicit SaveDialog(MainWindow*, int);
    ~SaveDialog();

public slots:
    void on_pushButton_Quit_clicked();
    void on_pushButton_Cancel_clicked();
    void on_pushButton_Save_clicked();

private:
    Ui::SaveDialog *ui;
    MainWindow* main;
    int index;
};

#endif // SAVEDIALOG_H
