#ifndef CONFLICTDIALOG_H
#define CONFLICTDIALOG_H

#include <QDialog>

namespace Ui {
class ConflictDialog;
}

class ConflictDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ConflictDialog(QWidget *parent = 0);
    ~ConflictDialog();
    void setUser(QString);
    void setDateAndTime(QString);


private slots:
    void on_savePushButton_clicked();

    void on_loadPushButton_clicked();

private:
    Ui::ConflictDialog *ui;
};

#endif // CONFLICTDIALOG_H
