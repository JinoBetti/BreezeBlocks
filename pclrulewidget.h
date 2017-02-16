#ifndef PCLRULEWIDGET_H
#define PCLRULEWIDGET_H

#include <QWidget>

namespace Ui {
class PCLRuleWidget;
}

class PCLRuleWidget : public QWidget
{
    Q_OBJECT

public:
    explicit PCLRuleWidget(QWidget *parent = 0);
    PCLRuleWidget(const QString &_program, const QString &_parameter, const QString &_subparam, const QString &_classe, bool _isOut, QWidget *parent = 0);
    ~PCLRuleWidget();

    QString getProgram() const { return program; }
    void setProgram(const QString &value) { program = value;}

    QString getParameter() const { return parameter;}
    void setParameter(const QString &value) { parameter = value;}

    QString getSubparam() const { return subparam;}
    void setSubparam(const QString &value) { subparam = value;}

    bool getIsOut() const { return isOut;}
    void setIsOut(bool value) { isOut = value;}

    QString getClasse() const{ return classe;}
    void setClasse(const QString &value) { classe = value; }

private:
    Ui::PCLRuleWidget *ui;
    QString program;
    QString parameter;
    QString subparam;
    QString classe;
    bool isOut;

signals:
    void deletePCLRule(QString, QString, QString); /*!< Signal émis lors du clic sur le bouton supprimer, utilisé par la classe de la fenêtre préférences. */

private slots:
    void on_pushButtonDelete_clicked(){ emit deletePCLRule(program, parameter, subparam); }
};

#endif // PCLRULEWIDGET_H
