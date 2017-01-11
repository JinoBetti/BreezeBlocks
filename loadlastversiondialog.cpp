#include "loadlastversiondialog.h"
#include "ui_loadlastversiondialog.h"

LoadLastVersionDialog::LoadLastVersionDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::LoadLastVersionDialog)
{ ui->setupUi(this); }

LoadLastVersionDialog::~LoadLastVersionDialog() { delete ui; }

void LoadLastVersionDialog::setUser(QString _value) { ui->userLabel->setText(_value); }

void LoadLastVersionDialog::setDateAndTime(QString _value) { ui->dateAndTimeLabel->setText(_value); }

void LoadLastVersionDialog::on_pushButtonReload_clicked() { accept(); }

void LoadLastVersionDialog::on_pushButtonContinue_clicked() { reject(); }
