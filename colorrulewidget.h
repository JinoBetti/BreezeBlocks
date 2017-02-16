#ifndef COLORRULEWIDGET_H
#define COLORRULEWIDGET_H

#include <QWidget>

namespace Ui {
class ColorRuleWidget;
}

/*! \class ColorRuleWidget
 * \brief Classe représentant un widget graphique permettant de définir une règle de couleur à appliquer selon la classe des noeuds DCLF
 *
 * Widget présent dans la fenêtre préférences, constitué d'un QLabel où figure le nom de la classe, d'un QPushButton permettant de modifier la couleur associée à cette classe,
 * et d'un second QPushButton permettant de supprimer la règle.
 *
 */
class ColorRuleWidget : public QWidget
{
    Q_OBJECT

private:
    Ui::ColorRuleWidget *ui; /*!< Lien vers le formulaire graphique */
    QString classe; /*!< Classe de DCLF dont la couleur est spécifiée */
    QColor color; /*!< Couleur spécifiée pour la classe */

public:
    explicit ColorRuleWidget(const QString &_classe, const QColor &_color, QWidget *parent = 0);
    ~ColorRuleWidget();

    QString getClasse() const { return classe;}
    void setClasse(const QString &value) { classe = value; }
    QColor getColor() const { return color; }
    void setColor(const QColor &value){ color = value;}

private slots:
    void on_pushButtonColor_clicked();
    void on_pushButtonDelete_clicked(){ emit deleteColorRule(classe); }

signals:
    void deleteColorRule(QString); /*!< Signal émis lors du clic sur le bouton supprimer, utilisé par la classe de la fenêtre préférences. */

};

#endif // COLORRULEWIDGET_H
