#include "modifystepdialog.h"
#include <QColorDialog>
#include "ui_modifystepdialog.h"
#include "stepgraphic.h"
#include "mainwindow.h"
#include "utils.h"
#include <QFileDialog>

ModifyStepDialog::ModifyStepDialog(NodeGraphic* _step, QWidget *parent) :
    QDialog(parent),
    graphics(dynamic_cast<CustomGraphics*>(parent)),
    ui(new Ui::ModifyStepDialog)
{
    ui->setupUi(this);
    step = dynamic_cast<StepGraphic*>(_step);

    ui->ID->setText(QString::number(step->getId()));
    ui->ID->setToolTip("Identifiant du step dans la sauvegarde XML");
    ui->lineEditName->setText(step->getName());
    ui->lineEditProgram->setText(step->getProgram());
    ui->textEditComment->setText(step->getComment());
    ui->textEditData->setText(step->getSysinValue());
    ui->textEditCondition->setText(step->getConditionValue());
    ui->textEditPrecision->setText(step->getPrecision());
    ui->spinBoxHeight->setValue(step->getHeight());
    ui->spinBoxWidth->setValue(step->getWidth());
    color = step->getColorSaved();
    ui->checkBoxTechnical->setChecked(step->getIsTechnical());
    ui->buttonBox->addButton("&Annuler", QDialogButtonBox::RejectRole);

    connect(ui->checkBoxTechnical,SIGNAL(toggled(bool)),this, SLOT(changeColorIfDefault(bool)));
    if(!step->getDetailPath().isEmpty()) ui->pushButtonDetail->setText(step->getDetailPath());
    QString qss = QString("background-color: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1, stop: 0 %1, stop: 1 #ffffff);").arg(color.name());
    ui->pushButtonColor->setStyleSheet(qss);

    autoAdjustUnset = false;
}

ModifyStepDialog::~ModifyStepDialog(){ delete ui; }

void ModifyStepDialog::changeColorIfDefault(bool _toggled)
{
    if(color==TECHNICAL_STEP_COLOR && !_toggled) color = NORMAL_STEP_COLOR;
    if(color==NORMAL_STEP_COLOR && _toggled) color = TECHNICAL_STEP_COLOR;

    QString qss = QString("background-color: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1, stop: 0 %1, stop: 1 #ffffff);").arg(color.name());
    ui->pushButtonColor->setStyleSheet(qss);
    ui->pushButtonColor->clearFocus();
}

void ModifyStepDialog::on_buttonBox_accepted()
{
    graphics->saveTemporaryFile("Modification de l'étape " + ui->lineEditName->text());

    step->setSysin(ui->textEditData->toPlainText());
    step->setCondition(ui->textEditCondition->toPlainText());

    step->setPrecision(ui->textEditPrecision->toPlainText());
    step->setComment(ui->textEditComment->toPlainText());

    step->setProgram(ui->lineEditProgram->text());
    step->setName(ui->lineEditName->text());
    step->setHeight(ui->spinBoxHeight->value());
    step->setWidth(ui->spinBoxWidth->value());
    step->setColorSaved(color);

    step->setIsTechnical(ui->checkBoxTechnical->isChecked());

    step->getGraphics()->setModified(true);
    if(!detailPath.isEmpty()) step->setDetailPath(detailPath);
    if(detailPath=="empty") step->setDetailPath("");

    if(ui->checkBoxAutoAdjust->isChecked()) step->autoAdjustSize();

    step->getGraphics()->getMain()->setStatus("Etape modifiée : " + step->getName());
}

void ModifyStepDialog::on_pushButtonColor_clicked()
{
    QColor temporaryColor = QColorDialog::getColor(color, this);
    if(!temporaryColor.isValid()) return;
    color = temporaryColor;
    QString qss = QString("background-color: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1, stop: 0 %1, stop: 1 #ffffff);").arg(color.name());
    ui->pushButtonColor->setStyleSheet(qss);
    ui->pushButtonColor->clearFocus();
}

void ModifyStepDialog::on_pushButtonDetail_clicked()
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

void ModifyStepDialog::on_pushButtonInitColor_clicked()
{
    color = ui->checkBoxTechnical->isChecked() ? TECHNICAL_STEP_COLOR : NORMAL_STEP_COLOR;
    if(step->getSubtype()=="PATTERN") color = PATTERN_STEP_COLOR;
    QString qss = QString("background-color: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1, stop: 0 %1, stop: 1 #ffffff);").arg(color.name());
    ui->pushButtonColor->setStyleSheet(qss);
    ui->pushButtonColor->clearFocus();
}

void ModifyStepDialog::on_pushButtonInitDetail_clicked()
{
    detailPath = "empty";
    ui->pushButtonDetail->setText("Parcourir...");
}

void ModifyStepDialog::on_checkBoxAutoAdjust_clicked()
{
    autoAdjustUnset = true;
}

void ModifyStepDialog::on_textEditPrecision_textChanged()
{
    if(!autoAdjustUnset){ ui->checkBoxAutoAdjust->setChecked(true); }
}

void ModifyStepDialog::on_lineEditProgram_textChanged(const QString &)
{
    if(!autoAdjustUnset){ ui->checkBoxAutoAdjust->setChecked(true); }
}

void ModifyStepDialog::on_lineEditName_textChanged(const QString &)
{
    if(!autoAdjustUnset){ ui->checkBoxAutoAdjust->setChecked(true); }
}

void ModifyStepDialog::on_spinBoxWidth_valueChanged(int)
{
    autoAdjustUnset = true;
}

void ModifyStepDialog::on_spinBoxHeight_valueChanged(int)
{
    autoAdjustUnset = true;
}
