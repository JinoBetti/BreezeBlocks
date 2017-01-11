#include "modifydclfdialog.h"
#include "ui_modifydclfdialog.h"
#include "dclfgraphic.h"
#include "utils.h"
#include <QColorDialog>
#include "mainwindow.h"
#include <QFileDialog>

ModifyDCLFDialog::ModifyDCLFDialog(NodeGraphic* _node,QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ModifyDCLFDialog),
    defaultColor(false)
{
    ui->setupUi(this);
    dclf = dynamic_cast<DCLFGraphic*>(_node);
    ui->ID->setText(QString::number(dclf->getId()));
    ui->ID->setToolTip("Identifiant de la DCLF dans la sauvegarde XML");
    ui->lineEditName->setText(dclf->getName());
    ui->lineEditClasse->setText(dclf->getClass());
    ui->textEditComment->setText(dclf->getComment());
    ui->textEditPrecisions->setText(dclf->getPrecision());
    ui->spinBoxHeight->setValue(dclf->getHeight());
    ui->spinBoxWidth->setValue(dclf->getWidth());
    ui->spinBoxSize->setValue(dclf->getLength());
    ui->lineEditType->setText(dclf->getFormat());
    color = dclf->getColorSaved();
    QString qss = QString("background-color: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1, stop: 0 %1, stop: 1 #ffffff);").arg(color.name());
    ui->pushButtonColor->setStyleSheet(qss);
    if(!dclf->getDetailPath().isEmpty()) ui->pushButtonDetail->setText(dclf->getDetailPath());

    ui->buttonBox->addButton("&Annuler", QDialogButtonBox::RejectRole);

    connect(ui->lineEditClasse,SIGNAL(textChanged(QString)),this, SLOT(reloadColor(QString)));

    if(color == DCLFGraphic::getColorFromRules(dclf->getClass())) defaultColor = true;

    autoAdjustUnset = false;
}

ModifyDCLFDialog::~ModifyDCLFDialog() { delete ui; }

void ModifyDCLFDialog::reloadColor(const QString &_class)
{
    if(defaultColor) color = DCLFGraphic::getColorFromRules(_class);

    QString qss = QString("background-color: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1, stop: 0 %1, stop: 1 #ffffff);").arg(color.name());
    ui->pushButtonColor->setStyleSheet(qss);
    ui->pushButtonColor->clearFocus();
}

void ModifyDCLFDialog::on_buttonBox_accepted()
{
    dclf->getGraphics()->saveTemporaryFile("Modification de la DCLF " + ui->lineEditName->text());
    dclf->setComment(ui->textEditComment->toPlainText());
    dclf->setClass(ui->lineEditClasse->text());
    dclf->setName(ui->lineEditName->text());
    dclf->setHeight(ui->spinBoxHeight->value());
    dclf->setWidth(ui->spinBoxWidth->value());
    dclf->setFormat(ui->lineEditType->text());
    dclf->setPrecision(ui->textEditPrecisions->toPlainText());
    dclf->setLength(ui->spinBoxSize->value());

    dclf->getGraphics()->updateDCLF(dclf->getName(), color, ui->lineEditClasse->text(), ui->lineEditType->text(), ui->spinBoxSize->value());
    dclf->getGraphics()->setModified(true);

    if(!detailPath.isEmpty()) dclf->setDetailPath(detailPath);
    if(detailPath=="empty") dclf->setDetailPath("");

    if(ui->checkBoxAutoAdjust->isChecked()) dclf->autoAdjustSize();

    dclf->getGraphics()->getMain()->setStatus("DCLF modifiée : " + dclf->getName());
}

void ModifyDCLFDialog::on_pushButtonColor_clicked()
{
    QColor temporaryColor = QColorDialog::getColor(color, this);
    if(!temporaryColor.isValid()) return;
    color = temporaryColor;
    QString qss = QString("background-color: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1, stop: 0 %1, stop: 1 #ffffff);").arg(color.name());
    ui->pushButtonColor->setStyleSheet(qss);
    ui->pushButtonColor->clearFocus();

    if(color == DCLFGraphic::getColorFromRules(dclf->getClass())) defaultColor = true;
}

void ModifyDCLFDialog::on_pushButtonDetail_clicked()
{
    QString openingLocation = WRITE_PATH;

    QString myfile = QFileDialog::getOpenFileName(this, tr("Ajouter un lien vers un autre dessin de chaine"), openingLocation, tr("Dessins de chaine (*.txt *.xml *.ddc)"));
    if(!myfile.isEmpty())
    {
        QDir workspace(WORKSPACE);
        detailPath = workspace.relativeFilePath(myfile);
        ui->pushButtonDetail->setText(detailPath);
    }
}

void ModifyDCLFDialog::on_pushButtonInitColor_clicked()
{
    color = DCLFGraphic::getColorFromRules(ui->lineEditClasse->text());
    QString qss = QString("background-color: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1, stop: 0 %1, stop: 1 #ffffff);").arg(color.name());
    ui->pushButtonColor->setStyleSheet(qss);
    ui->pushButtonColor->clearFocus();
}

void ModifyDCLFDialog::on_pushButtonInitDetail_clicked()
{
    detailPath = "empty";
    ui->pushButtonDetail->setText("Parcourir...");
}

void ModifyDCLFDialog::on_textEditPrecisions_textChanged()
{
    if(!autoAdjustUnset){ ui->checkBoxAutoAdjust->setChecked(true); }
}

void ModifyDCLFDialog::on_lineEditName_textChanged(const QString &)
{
    if(!autoAdjustUnset){ ui->checkBoxAutoAdjust->setChecked(true); }
}

void ModifyDCLFDialog::on_lineEditClasse_textChanged(const QString &)
{
    if(!autoAdjustUnset){ ui->checkBoxAutoAdjust->setChecked(true); }
}

void ModifyDCLFDialog::on_spinBoxWidth_valueChanged(int)
{
    autoAdjustUnset = true;
}

void ModifyDCLFDialog::on_spinBoxHeight_valueChanged(int)
{
    autoAdjustUnset = true;
}

void ModifyDCLFDialog::on_checkBoxAutoAdjust_clicked()
{
    autoAdjustUnset = true;
}
