#ifndef SAVEBEFOREQUITDIALOG_H
#define SAVEBEFOREQUITDIALOG_H

#include <QDialog>
#include <QLayout>
#include "mainwindow.h"

namespace Ui {
class SaveBeforeQuitDialog;
}

class SaveBeforeQuitDialog : public QDialog
{
    Q_OBJECT

public:
    explicit SaveBeforeQuitDialog(QWidget *parent = 0);
    void addRow(QHBoxLayout* _row, int _i);
    ~SaveBeforeQuitDialog();
    MainWindow * parent;

private slots:
    void on_pushButton_cancel_clicked();
    void on_pushButton_quit_clicked(){ accept();}
    void on_pushButton_save_clicked();

private:
    Ui::SaveBeforeQuitDialog *ui;
    QVector<QHBoxLayout*> rows;
    QVector<int> indexes;
};

#endif // SAVEBEFOREQUITDIALOG_H
