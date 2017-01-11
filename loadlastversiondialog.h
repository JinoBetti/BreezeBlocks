#ifndef LOADLASTVERSIONDIALOG_H
#define LOADLASTVERSIONDIALOG_H

#include <QDialog>

namespace Ui {
class LoadLastVersionDialog;
}

class LoadLastVersionDialog : public QDialog
{
    Q_OBJECT

public:
    explicit LoadLastVersionDialog(QWidget *parent = 0);
    ~LoadLastVersionDialog();

    void setUser(QString _value);
    void setDateAndTime(QString _value);

private slots:
    void on_pushButtonReload_clicked();

    void on_pushButtonContinue_clicked();

private:
    Ui::LoadLastVersionDialog *ui;
};

#endif // LOADLASTVERSIONDIALOG_H
