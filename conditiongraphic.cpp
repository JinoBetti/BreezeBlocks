#include "conditiongraphic.h"
#include <QPainter>
#include <QColorDialog>

/*!
 *  \fn ConditionGraphic::ConditionGraphic(QString _condition, StepGraphic *_parent):parent(_parent),condition(_condition)
 *  \brief
 * \param
 * \param
 */
ConditionGraphic::ConditionGraphic(const QString &_condition, StepGraphic *_parent):parent(_parent),condition(_condition)
{
    polygon<<QPoint(0, 0)<<QPoint(0, CONDITION_SIZE)<<QPoint(CONDITION_SIZE, 0);
    parent->setCondition(this);
    color = Qt::red;
    setZValue(3);
}

/*!
 *  \fn void ConditionGraphic::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
 *  \brief
 *
 * \param
 */
void ConditionGraphic::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);
    setToolTip(condition);
    painter->setRenderHint(QPainter::Antialiasing);
    painter->setBrush(color);
    painter->setPen(Qt::black);
    painter->drawPolygon(polygon);
}

void ConditionGraphic::mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event){parent->mouseDoubleClickEvent(event);}

void ConditionGraphic::mousePressEvent(QGraphicsSceneMouseEvent *event){parent->mousePressEvent(event);}

void ConditionGraphic::mouseReleaseEvent(QGraphicsSceneMouseEvent *event){parent->mouseReleaseEvent(event);}

void ConditionGraphic::mouseMoveEvent(QGraphicsSceneMouseEvent *event){parent->mouseMoveEvent(event);}
