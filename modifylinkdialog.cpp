#include "modifylinkdialog.h"
#include "ui_modifylinkdialog.h"
#include "customgraphics.h"

ModifyLinkDialog::ModifyLinkDialog(LinkGraphic* _link, QWidget *parent) :
    QDialog(parent),
    link(_link),
    graphics(dynamic_cast<CustomGraphics*>(parent)),
    ui(new Ui::ModifyLinkDialog)
{
    ui->setupUi(this);
    ui->buttonBox->addButton("&Annuler", QDialogButtonBox::RejectRole);
    ui->labelInputName->setText(link->getInput()->getName());
    ui->labelOutputName->setText(link->getOutput()->getName());
    ui->lineEditDDName->setText(link->getDDName());
    ui->horizontalSlider->setValue(link->getPonderation());
    difference = 0;
    if(link->getPonderation()>100) difference = link->getPonderation()-100;
    if(link->getPonderation()<0) difference = link->getPonderation();
    ui->percentage->setText(QString::number(link->getPonderation())+"%");
    ui->comboBox->setCurrentIndex(link->getShape());

    int iconWidth = 400;
    ui->comboBoxPenStyle->setEditable(false);
    ui->comboBoxPenStyle->setIconSize(QSize(iconWidth,14));
    ui->comboBoxPenStyle->setMinimumWidth(iconWidth);
    ui->radioButton->setChecked(!link->getEndShape());
    ui->radioButtonArrow->setChecked(link->getEndShape());

    for(int i=Qt::SolidLine; i<=Qt::DashDotLine; i++)
    {
        QPixmap pix(iconWidth,14);
        pix.fill(Qt::white);
        QBrush brush(Qt::black);
        QPen pen(brush, 1.5, (Qt::PenStyle) i);
        QPainter painter (&pix);
        painter.setPen(pen);
        painter.drawLine(2,7,iconWidth-2,7);
        ui->comboBoxPenStyle->addItem(QIcon(pix),"");
    }

    ui->comboBoxPenStyle->setCurrentIndex(link->getPenStyle()-1);

    ui->lineEditDDName->setFocus();
}

void ModifyLinkDialog::on_buttonBox_accepted()
{
    graphics->saveTemporaryFile("Modification du lien entre "+ link->getInput()->getName()+" et "+link->getOutput()->getName());
    link->setDDName(ui->lineEditDDName->text());
    link->setToolTip(link->getDDName());
    link->setPonderation(ui->horizontalSlider->value()+difference);
    link->setShape(ui->comboBox->currentIndex());
    link->setPenStyle(Qt::PenStyle(ui->comboBoxPenStyle->currentIndex()+1));
    link->setEndShape(ui->radioButtonArrow->isChecked());
    graphics->getMain()->setStatus("Lien modifié entre "+ link->getInput()->getName()+" et "+link->getOutput()->getName());
}

ModifyLinkDialog::~ModifyLinkDialog() { delete ui; }

void ModifyLinkDialog::on_horizontalSlider_valueChanged(int value){ difference = 0; ui->percentage->setText(QString::number(value)+"%");}
