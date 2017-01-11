#ifndef CONDITIONGRAPHIC_H
#define CONDITIONGRAPHIC_H

#include <QGraphicsItem>
#include"stepgraphic.h"
class StepGraphic;

/*! \class ConditionGraphic
 * \brief Classe de composant graphique qui permet de représenter une condition d'execution d'un step sous forme d'un triangle rouge dans le coin
 * en haut à gauche du StepGraphic. Chaque StepGraphic a une ConditionGraphic, mais celle ci n'est affichée que si la condition n'est pas vide
 * et si la variable CONDITIONS_DISPLAYED est vraie (i.e. si l'entrée du menu affichage est cochée)
 *
 */
class ConditionGraphic : public QGraphicsItem
{
private:
    QPolygon polygon; /*!< Forme de la condition*/
    StepGraphic* parent; /*!< Etape sur laquelle est placée la condition*/
    QString condition; /*!< Condition*/
    QColor color; /*!< Couleur de la condition*/

public:
    ConditionGraphic(const QString &_condition, StepGraphic* _parent = 0);
    ~ConditionGraphic(){}
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);
    QRectF boundingRect() const { return QRectF(0, 0,  CONDITION_SIZE, CONDITION_SIZE); }

    QColor getColor() const { return color; }
    void setColor(const QColor &value) {color = value; }

    void resetPolygon() { polygon.clear(); polygon<<QPoint(0, 0)<<QPoint(0, CONDITION_SIZE)<<QPoint(CONDITION_SIZE, 0); }

    QString getCondition() const { return condition; }
    void setCondition(const QString &value) { condition = value; }

protected:
    void mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event);
    void mousePressEvent(QGraphicsSceneMouseEvent *event);
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event);
};

#endif // CONDITIONGRAPHIC_H
