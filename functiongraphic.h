#ifndef FUNCTIONGRAPHIC_H
#define FUNCTIONGRAPHIC_H

#include "nodegraphic.h"
#include <QDebug>

class FunctionGraphic : public NodeGraphic
{
private:
    QString name;
    QString level;
    QString callMethod;
    QString precision;

public:
    FunctionGraphic(const QString &_name, const QString &_level, const QString &_precision, const QString &_callMethod, const QString &_comment, CustomGraphics *_graphics, bool _automatic);

    QString getName() const;
    void setName(const QString &value);
    QString getLevel() const;
    void setLevel(const QString &value);
    QString getCallMethod() const;
    void setCallMethod(const QString &value);
    QString getPrecision() const;
    void setPrecision(const QString &value);

    virtual void hoverEnterEvent ( QGraphicsSceneHoverEvent *  );
    virtual void hoverLeaveEvent ( QGraphicsSceneHoverEvent *  );
    NodeGraphic* createCopy(CustomGraphics* _graphics);
    
    QRectF boundingRect() const;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);
    QVariant itemChange(GraphicsItemChange change, const QVariant &value);
    QString getText() const;
};

#endif // FUNCTIONGRAPHIC_H
