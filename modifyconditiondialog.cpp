#include "modifyconditiondialog.h"
#include "ui_modifyconditiondialog.h"
#include "customgraphics.h"
#include<QColorDialog>
#include<QFileDialog>

ModifyConditionDialog::ModifyConditionDialog(NodeGraphic *_node, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ModifyConditionDialog),
    node(dynamic_cast<ConditionNodeGraphic*>(_node))
{
    ui->setupUi(this);
    ui->buttonBox->addButton("&Annuler", QDialogButtonBox::RejectRole);

    ui->labelID->setText(QString::number(node->getId()));
    ui->labelID->setToolTip("Identifiant de la fonction dans la sauvegarde XML");
    ui->plainTextEditCondition->setPlainText(node->getText());
    ui->plainTextEditComment->setPlainText(node->getComment());
    ui->spinBoxHeight->setValue(node->getHeight());
    ui->spinBoxWidth->setValue(node->getWidth());
    color = node->getColorSaved();
    QString qss = QString("background-color: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1, stop: 0 %1, stop: 1 #ffffff);").arg(color.name());
    ui->pushButtonColor->setStyleSheet(qss);

    if(!node->getDetailPath().isEmpty()) ui->pushButtonLink->setText(node->getDetailPath());

}

ModifyConditionDialog::~ModifyConditionDialog() { delete ui; }

void ModifyConditionDialog::on_buttonBox_accepted()
{
    node->getGraphics()->saveTemporaryFile("Modification de la condition");

    node->setText(ui->plainTextEditCondition->toPlainText());
    node->setComment(ui->plainTextEditComment->toPlainText());
    node->setHeight(ui->spinBoxHeight->value());
    node->setWidth(ui->spinBoxWidth->value());
    node->setColorSaved(color);

    if(!detailPath.isEmpty()) node->setDetailPath(detailPath);
    if(detailPath=="empty") node->setDetailPath("");

    node->getGraphics()->getMain()->setStatus("Fonction modifiée : " + node->getName());
}

void ModifyConditionDialog::on_pushButtonColor_clicked()
{
    QColor newColor = QColorDialog::getColor(color, this);
    if(newColor.isValid())
    {
        color = newColor;
        QString qss = QString("background-color: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1, stop: 0 %1, stop: 1 #ffffff);").arg(color.name());
        ui->pushButtonColor->setStyleSheet(qss);
        ui->pushButtonColor->clearFocus();
    }

}

void ModifyConditionDialog::on_pushButtonColorInit_clicked()
{
    color = Qt::white;
    QString qss = QString("background-color: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1, stop: 0 %1, stop: 1 #ffffff);").arg(color.name());
    ui->pushButtonColor->setStyleSheet(qss);
    ui->pushButtonColor->clearFocus();
}

void ModifyConditionDialog::on_pushButtonLinkInit_clicked()
{
    detailPath = "empty";
    ui->pushButtonLink->setText("Parcourir...");
}

void ModifyConditionDialog::on_pushButtonLink_clicked()
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
