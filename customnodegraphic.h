#ifndef CUSTOMNODEGRAPHIC_H
#define CUSTOMNODEGRAPHIC_H

#include "nodegraphic.h"
#include "utils.h"


class CustomNodeGraphic : public NodeGraphic
{
private:
    bool transparentContour;
    Qt::AlignmentFlag hzAlign;
    Qt::AlignmentFlag vtAlign;
    QFont font;
    int fontSize;
    bool movable;
    bool hasShadow;

    MyShapes::Shape shapeType;
    QString text;
    QRectF textRect;

    void drawCylinder(QPainter *painter, bool _hasShadow);
public:
    CustomNodeGraphic(const QString &_text, const QString &_comment, MyShapes::Shape _shape, CustomGraphics* _graphics, bool _automatic);

    NodeGraphic* createCopy(CustomGraphics* _graphics);
    QRectF boundingRect() const;

    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);

    QString getName() const { return text; }
    QString getProgram() const { return text; }
    bool getTransparentContour() const { return transparentContour; }
    void setTransparentContour(bool value){ transparentContour = value; }
    bool getHasShadow() const {  return hasShadow; }
    void setHasShadow(bool value) { hasShadow = value; }
    MyShapes::Shape getShape() const { return shapeType; }
    void setShape(MyShapes::Shape value) { shapeType = value; }
    QString getText() const { return text; }
    void setText(const QString &value) { text = value; }
    int getHzAlign() const;
    void setHzAlign(const int value);
    int getVtAlign() const;
    void setVtAlign(const int value);

    bool getMovable() const { return movable; }
    void setMovable(bool value) { movable = value; if(!isSelected()) setFlag(QGraphicsItem::ItemIsMovable, value); }
    QVariant itemChange(GraphicsItemChange change, const QVariant &value);

    QFont getFont() const;
    void setFont(const QFont &value);
    int getFontSize() const;
    void setFontSize(int value);
};

#endif // CUSTOMNODEGRAPHIC_H
