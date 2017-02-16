#include "opennewdialog.h"
#include "ui_opennewdialog.h"

OpenNewDialog::OpenNewDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::OpenNewDialog)
{
    ui->setupUi(this);
    ui->buttonBox->addButton("&Annuler", QDialogButtonBox::RejectRole);
}

OpenNewDialog::~OpenNewDialog(){ delete ui;}

void OpenNewDialog::on_buttonBox_accepted()
{
    name = ui->lineEdit->text();
    title = ui->textEdit->toPlainText();
    version = ui->lineEditVersion->text();
}
QString OpenNewDialog::getVersion() const
{
    return version;
}

void OpenNewDialog::setVersion(const QString &value)
{
    version = value;
}

