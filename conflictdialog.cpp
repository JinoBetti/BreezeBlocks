#include "conflictdialog.h"
#include "ui_conflictdialog.h"

ConflictDialog::ConflictDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ConflictDialog)
{
    ui->setupUi(this);
}

ConflictDialog::~ConflictDialog()
{
    delete ui;
}

void ConflictDialog::setUser(QString _value)
{
    ui->userLabel->setText(_value);
}

void ConflictDialog::setDateAndTime(QString _value)
{
    ui->dateAndTimeLabel->setText(_value);
}

void ConflictDialog::on_savePushButton_clicked()
{
    accept();
}

void ConflictDialog::on_loadPushButton_clicked()
{
    reject();
}
