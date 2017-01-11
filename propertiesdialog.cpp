#include "propertiesdialog.h"
#include "ui_propertiesdialog.h"
#include "loadxml.h"

PropertiesDialog::PropertiesDialog(QString _path, CustomGraphics* _graphics, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::PropertiesDialog)
{
    ui->setupUi(this);
    ui->labelPath->setText(_path);
    LoadXML loadXML(_path, _graphics, false);
    if(!loadXML.getError()) //Si fichier supprimé pas de pb
    {
        ui->labelDate->setText(loadXML.readModificationDate().toString());
        ui->labelTime->setText(loadXML.readModificationTime().toString());
        ui->labelUser->setText(loadXML.readUser());
    }
    else
    {
        ui->labelDate->setText("ERREUR");
        ui->labelTime->setText("ERREUR");
        ui->labelUser->setText("ERREUR");
    }
}

PropertiesDialog::~PropertiesDialog()
{
    delete ui;
}
