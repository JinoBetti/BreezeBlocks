#include "pclrulewidget.h"
#include "ui_pclrulewidget.h"

PCLRuleWidget::PCLRuleWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::PCLRuleWidget){ ui->setupUi(this); }

PCLRuleWidget::PCLRuleWidget(const QString &_program, const QString &_parameter, const QString &_subparam, const QString &_classe, bool _isOut, QWidget *parent):
    QWidget(parent),
    ui(new Ui::PCLRuleWidget),
    program(_program),
    parameter(_parameter),
    subparam(_subparam),
    classe(_classe),
    isOut(_isOut)
{
    ui->setupUi(this);
    ui->labelIO->setText(isOut ? "Sortie" : "Entrée");
    ui->labelParam->setText(parameter);
    ui->labelProgram->setText(program);
    ui->labelSubParam->setText(subparam);
    ui->labelClasse->setText(classe);
}

PCLRuleWidget::~PCLRuleWidget() { delete ui;}
