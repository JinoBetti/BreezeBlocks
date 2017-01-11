#include "modifycustomnodedialog.h"
#include "ui_modifycustomnodedialog.h"
#include<QColorDialog>
#include "mainwindow.h"
#include"utils.h"
#include <QFileDialog>


ModifyCustomNodeDialog::ModifyCustomNodeDialog(NodeGraphic* _node, QWidget *parent) :
    QDialog(parent),
    graphics(dynamic_cast<CustomGraphics*>(parent)),
    ui(new Ui::ModifyCustomNodeDialog)
{
    ui->setupUi(this);

    node = dynamic_cast<CustomNodeGraphic*>(_node);

    ui->textEditComment->setPlainText(node->getComment());
    ui->textEditText->setPlainText(node->getText());
    ui->checkBoxContour->setChecked(node->getTransparentContour());
    ui->checkBoxShadow->setChecked(node->getHasShadow());

    ui->spinBoxHeight->setValue(node->getHeight());
    ui->spinBoxWidth->setValue(node->getWidth());

    color = node->getColorSaved();

    ui->comboBoxShape->setCurrentIndex(node->getShape());
    color = node->getColorSaved();
    QString qss = QString("background-color: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1, stop: 0 %1, stop: 1 #ffffff);").arg(color.name());
    ui->pushButtonColor->setStyleSheet(qss);
    if(!node->getDetailPath().isEmpty()) ui->pushButtonLink->setText(node->getDetailPath());
    ui->buttonBox->addButton("&Annuler", QDialogButtonBox::RejectRole);

    ui->comboBoxHzAlign->setCurrentIndex(node->getHzAlign());
    ui->comboBoxVtAlign->setCurrentIndex(node->getVtAlign());
    ui->fontComboBox->setCurrentFont(node->getFont());

    int fontSize = node->getFontSize();

    if((node->getShape()==MyShapes::Diamond && fontSize==TECHNICAL_FONT_SIZE) || (node->getShape()!=MyShapes::Diamond && fontSize==FONT_SIZE)) fontSize = 0;
    ui->spinBoxSize->setValue(fontSize);
    ui->checkBoxMovable->setChecked(!node->getMovable());
    ui->textEditText->setFocus();
    ui->checkBoxBackground->setChecked(color.alpha()==0);
    ui->checkBoxInTheBack->setChecked(!node->zValue());

    autoAdjustUnset = ui->checkBoxInTheBack->isChecked() ? true : false;

}

ModifyCustomNodeDialog::~ModifyCustomNodeDialog() { delete ui; }

void ModifyCustomNodeDialog::on_buttonBox_accepted()
{
    node->getGraphics()->saveTemporaryFile("Modification du noeud "+ui->textEditText->toPlainText().left(10));
    node->setShape(MyShapes::Shape(ui->comboBoxShape->currentIndex()));
    node->setHasShadow(ui->checkBoxShadow->isChecked());
    node->setTransparentContour(ui->checkBoxContour->isChecked());
    node->setComment(ui->textEditComment->toPlainText());
    node->setText(ui->textEditText->toPlainText());
    node->setWidth(ui->spinBoxWidth->value());
    node->setHeight(ui->spinBoxHeight->value());
    node->setColorSaved(color);
    if(!detailPath.isEmpty()) node->setDetailPath(detailPath);
    if(detailPath=="empty") node->setDetailPath("");

    node->setHzAlign(ui->comboBoxHzAlign->currentIndex());
    node->setVtAlign(ui->comboBoxVtAlign->currentIndex());

    node->setFontSize(ui->spinBoxSize->value());
    node->setFont(ui->fontComboBox->currentFont());

    node->setMovable(!ui->checkBoxMovable->isChecked());
    node->setZValue(ui->checkBoxInTheBack->isChecked()? 0 : 10);

    if(ui->checkBoxAutoAdjust->isChecked()) node->autoAdjustSize();

    node->getGraphics()->getMain()->setStatus("Noeud personnalisable modifiée : " + node->getText().left(10));

    //qDebug()<<node->pos();
}

void ModifyCustomNodeDialog::on_pushButtonColor_clicked()
{
    QColor temporaryColor = QColorDialog::getColor(color, this,"Choix de la couleur de fond", (QColorDialog::ShowAlphaChannel));
    if(!temporaryColor.isValid()) return;
    color = temporaryColor;
    QString qss = QString("background-color: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1, stop: 0 %1, stop: 1 #ffffff);").arg(color.name());
    ui->pushButtonColor->setStyleSheet(qss);
    ui->pushButtonColor->clearFocus();
    ui->checkBoxBackground->setChecked(color.alpha()==0);
}

void ModifyCustomNodeDialog::on_pushButtonLink_clicked()
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

void ModifyCustomNodeDialog::on_pushButtonInitDetail_clicked()
{
    detailPath = "empty";
    ui->pushButtonLink->setText("Parcourir...");
}

void ModifyCustomNodeDialog::on_checkBoxBackground_clicked(bool checked)
{
    color.setAlpha(checked ? 0 : 255);
}

void ModifyCustomNodeDialog::on_textEditText_textChanged()
{
    if(!autoAdjustUnset){ ui->checkBoxAutoAdjust->setChecked(true); }
}

void ModifyCustomNodeDialog::on_checkBoxAutoAdjust_clicked() { autoAdjustUnset = true; }
void ModifyCustomNodeDialog::on_spinBoxWidth_valueChanged(int)
{
    autoAdjustUnset = true;
    ui->checkBoxAutoAdjust->setChecked(false);
}

void ModifyCustomNodeDialog::on_spinBoxHeight_valueChanged(int)
{
    autoAdjustUnset = true;
    ui->checkBoxAutoAdjust->setChecked(false);
}

void ModifyCustomNodeDialog::on_spinBoxSize_valueChanged(int)
{
    if(!autoAdjustUnset){ ui->checkBoxAutoAdjust->setChecked(true); }
}
