#ifndef SYSINGRAPHIC_H
#define SYSINGRAPHIC_H

#include <QGraphicsItem>
#include"stepgraphic.h"
class StepGraphic;

class SysinGraphic : public QGraphicsItem
{
private:
    QPolygon polygon;
    StepGraphic* step;
    QString sysin;

public:
    SysinGraphic(const QString &_sysin, StepGraphic* _step = 0);
    ~SysinGraphic(){}
    QString getSysin() const {return sysin;}
    void setSysin(const QString &_sysin){sysin = _sysin;}
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);
    QRectF boundingRect() const { return QRectF(0, 0,  4*SYSIN_SIZE, 4*SYSIN_SIZE);}
    void resetPolygon();
    void setPosition();
protected:
    void mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event);
    void mousePressEvent(QGraphicsSceneMouseEvent *event);
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event);
};

#endif // SYSINGRAPHIC_H
