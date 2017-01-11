#include "modifyfunctiondialog.h"
#include "ui_modifyfunctiondialog.h"
#include "functiongraphic.h"
#include "utils.h"
#include "mainwindow.h"
#include <QColorDialog>
#include <QFileDialog>

ModifyFunctionDialog::ModifyFunctionDialog(NodeGraphic *_node, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ModifyFunctionDialog)
{
    ui->setupUi(this);
    function = dynamic_cast<FunctionGraphic*>(_node);
    ui->lineEditCallMethod->setText(function->getCallMethod());
    ui->labelId->setText(QString::number(function->getId()));
    ui->labelId->setToolTip("Identifiant de la fonction dans la sauvegarde XML");
    ui->lineEditLevel->setText(function->getLevel());
    ui->lineEditName->setText(function->getName());
    ui->textEditComment->setText(function->getComment());
    ui->textEditPrecisions->setText(function->getPrecision());
    ui->spinBoxHeight->setValue(function->getHeight());
    ui->spinBoxWidth->setValue(function->getWidth());
    color = function->getColorSaved();
    QString qss = QString("background-color: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1, stop: 0 %1, stop: 1 #ffffff);").arg(color.name());
    ui->pushButtonColor->setStyleSheet(qss);

    if(!function->getDetailPath().isEmpty()) ui->pushButtonLink->setText(function->getDetailPath());

    autoAdjustUnset = false;
}

ModifyFunctionDialog::~ModifyFunctionDialog() { delete ui; }

void ModifyFunctionDialog::on_pushButtonColor_clicked()
{
    QColor temporaryColor = QColorDialog::getColor(color, this);
    if(!temporaryColor.isValid()) return;
    color = temporaryColor;
    QString qss = QString("background-color: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1, stop: 0 %1, stop: 1 #ffffff);").arg(color.name());
    ui->pushButtonColor->setStyleSheet(qss);
    ui->pushButtonColor->clearFocus();

}

void ModifyFunctionDialog::on_pushButtonLink_clicked()
{
    QString openingLocation = WRITE_PATH;

    QString myfile = QFileDialog::getOpenFileName(this, tr("Ajouter un lien vers un autre dessin de chaine"), openingLocation, tr("Dessins de chaine (*.txt *.xml *.ddc)"));
    if(!myfile.isEmpty())
    {
        QDir workspace(WORKSPACE);
        detailPath = workspace.relativeFilePath(myfile);
        ui->pushButtonLink->setText(detailPath);
    }
}

void ModifyFunctionDialog::on_pushButtonInitColor_clicked()
{
    color = Qt::white;
    QString qss = QString("background-color: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1, stop: 0 %1, stop: 1 #ffffff);").arg(color.name());
    ui->pushButtonColor->setStyleSheet(qss);
    ui->pushButtonColor->clearFocus();
}

void ModifyFunctionDialog::on_pushButtonInitLink_clicked()
{
    detailPath = "empty";
    ui->pushButtonLink->setText("Parcourir...");
}

void ModifyFunctionDialog::on_buttonBox_accepted()
{
    function->getGraphics()->saveTemporaryFile("Modification de la fonction " + ui->lineEditName->text());

    function->setName(ui->lineEditName->text());
    function->setLevel(ui->lineEditLevel->text());
    function->setCallMethod(ui->lineEditCallMethod->text());
    function->setComment(ui->textEditComment->toPlainText());
    function->setPrecision(ui->textEditPrecisions->toPlainText());
    function->setHeight(ui->spinBoxHeight->value());
    function->setWidth(ui->spinBoxWidth->value());
    function->getGraphics()->updateFunctions(ui->lineEditName->text(), color);

    if(!detailPath.isEmpty()) function->setDetailPath(detailPath);
    if(detailPath=="empty") function->setDetailPath("");

    if(ui->checkBoxAutoAdjust->isChecked()) function->autoAdjustSize();

    function->getGraphics()->getMain()->setStatus("Fonction modifiée : " + function->getName());

}

void ModifyFunctionDialog::on_lineEditLevel_textChanged(const QString &)
{
    if(!autoAdjustUnset){ ui->checkBoxAutoAdjust->setChecked(true); }
}

void ModifyFunctionDialog::on_lineEditCallMethod_textChanged(const QString &)
{
    if(!autoAdjustUnset){ ui->checkBoxAutoAdjust->setChecked(true); }
}

void ModifyFunctionDialog::on_lineEditName_textChanged(const QString &)
{
    if(!autoAdjustUnset){ ui->checkBoxAutoAdjust->setChecked(true); }
}

void ModifyFunctionDialog::on_textEditPrecisions_textChanged()
{
    if(!autoAdjustUnset){ ui->checkBoxAutoAdjust->setChecked(true); }
}

void ModifyFunctionDialog::on_spinBoxWidth_valueChanged(int)
{
    autoAdjustUnset = true;
}

void ModifyFunctionDialog::on_spinBoxHeight_valueChanged(int)
{
    autoAdjustUnset = true;
}

void ModifyFunctionDialog::on_checkBoxAutoAdjust_clicked()
{
    autoAdjustUnset = true;
}
