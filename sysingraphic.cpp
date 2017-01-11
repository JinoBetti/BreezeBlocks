#include "sysingraphic.h"
#include<QPainter>
#include "utils.h"

SysinGraphic::SysinGraphic(const QString &_sysin, StepGraphic *_step):step(_step),sysin(_sysin)
{
    polygon<<QPoint(0, SYSIN_SIZE)<<QPoint(2*SYSIN_SIZE, SYSIN_SIZE)<<QPoint(2*SYSIN_SIZE, 0)<<QPoint(4*SYSIN_SIZE, 2*SYSIN_SIZE)<<QPoint(2*SYSIN_SIZE, 4*SYSIN_SIZE)<<QPoint(2*SYSIN_SIZE, 3*SYSIN_SIZE)<<QPoint(0, 3*SYSIN_SIZE);
    _step->setSysin(this);
    setZValue(3);
}

void SysinGraphic::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);

    setToolTip(sysin);
    painter->setRenderHint(QPainter::Antialiasing);
    painter->setBrush(Qt::black);
    painter->setPen(Qt::white);
    painter->drawPolygon(polygon);
}

void SysinGraphic::setPosition() { setPos(0, step->getHeight()/2 - 2*SYSIN_SIZE); }

void SysinGraphic::mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event){step->mouseDoubleClickEvent(event);}

void SysinGraphic::mousePressEvent(QGraphicsSceneMouseEvent *event){step->mousePressEvent(event);}

void SysinGraphic::mouseReleaseEvent(QGraphicsSceneMouseEvent *event){step->mouseReleaseEvent(event);}

void SysinGraphic::mouseMoveEvent(QGraphicsSceneMouseEvent *event){step->mouseMoveEvent(event);}

void SysinGraphic::resetPolygon()
{
    polygon.clear();
    polygon<<QPoint(0, SYSIN_SIZE)<<QPoint(2*SYSIN_SIZE, SYSIN_SIZE)<<QPoint(2*SYSIN_SIZE, 0)<<QPoint(4*SYSIN_SIZE, 2*SYSIN_SIZE)<<QPoint(2*SYSIN_SIZE, 4*SYSIN_SIZE)<<QPoint(2*SYSIN_SIZE, 3*SYSIN_SIZE)<<QPoint(0, 3*SYSIN_SIZE);
    setPosition();
}


