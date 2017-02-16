#include "colorrulewidget.h"
#include "ui_colorrulewidget.h"
#include<QColorDialog>

/*!
 *  \fn ColorRuleWidget::ColorRuleWidget(QString _classe, QColor _color, QWidget *parent)
 *  \brief Constructeur de la class ColorRuleWidget
 *
 * \param _classe : Classe de DCLF concernée par la règle
 * \param _color : Couleur spécifiée pour le classe concernée
 * \param parent : Widget parent
 */
ColorRuleWidget::ColorRuleWidget(const QString &_classe, const QColor &_color, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ColorRuleWidget),
    classe(_classe),
    color(_color)
{
    ui->setupUi(this);
    ui->label->setText(classe);
    QString qss = QString("background-color: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1, stop: 0 %1, stop: 1 #ffffff);").arg(color.name());
    ui->pushButtonColor->setStyleSheet(qss);
    ui->pushButtonColor->clearFocus();
}

/*!
 *  \fn ColorRuleWidget::~ColorRuleWidget()
 *  \brief Destructeur de la classe ColorRuleWidget
 */
ColorRuleWidget::~ColorRuleWidget(){ delete ui;}

/*!
 *  \fn void ColorRuleWidget::on_pushButtonColor_clicked()
 *  \brief Comportement au clic sur le bouton "Couleur"
 */
void ColorRuleWidget::on_pushButtonColor_clicked()
{
    QColor newColor = QColorDialog::getColor(color, this, "Changer la couleur des fichiers de classe " + classe);
    if(newColor.isValid())
    {
        color = newColor;
        QString qss = QString("background-color: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1, stop: 0 %1, stop: 1 #ffffff);").arg(color.name());
        ui->pushButtonColor->setStyleSheet(qss);
        ui->pushButtonColor->clearFocus();
    }
}
