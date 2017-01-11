#include "customnodegraphic.h"
#include "linkgraphic.h"
#include "customgraphics.h"
#include <QPainter>
#include <QDebug>


int CustomNodeGraphic::getHzAlign() const
{
    switch(hzAlign)
    {
    case Qt::AlignLeft :
        return 0;
    case Qt::AlignHCenter :
        return 1;
    case Qt::AlignRight :
        return 2;
    }
    return 1;
}

void CustomNodeGraphic::setHzAlign(const int value)
{
    switch(value)
    {
    case 0 :
        hzAlign = Qt::AlignLeft;
        break;
    case 1 :
        hzAlign = Qt::AlignHCenter;
        break;
    case 2 :
        hzAlign = Qt::AlignRight;
        break;
    }

}

int CustomNodeGraphic::getVtAlign() const
{
    switch(vtAlign)
    {
    case Qt::AlignTop :
        return 0;
    case Qt::AlignVCenter :
        return 1;
    case Qt::AlignBottom :
        return 2;
    }
    return 1;
}

void CustomNodeGraphic::setVtAlign(const int value)
{

    switch(value)
    {
    case 0 :
       vtAlign = Qt::AlignTop;
       break;
    case 1 :
       vtAlign = Qt::AlignVCenter;
       break;
    case 2 :
       vtAlign = Qt::AlignBottom;
       break;
    }
}



CustomNodeGraphic::CustomNodeGraphic(const QString &_text, const QString &_comment, MyShapes::Shape _shape, CustomGraphics* _graphics, bool _automatic):
    NodeGraphic(-1,-1,"CUSTOM", _comment, _graphics, _automatic),
    transparentContour(false),
    hzAlign(Qt::AlignHCenter),
    vtAlign(Qt::AlignCenter),
    font(QFont()),
    fontSize(0),
    movable(true),
    hasShadow(false),
    shapeType(_shape),
    text(_text)
{
    //qDebug()<<"CustomNodeGraphic::CustomNodeGraphic()"<< getName();

    setFlag(QGraphicsItem::ItemIsMovable, true);
    setFlag(QGraphicsItem::ItemIsSelectable,SELECTABLE_STATE);
    setFlag(QGraphicsItem::ItemSendsGeometryChanges,true);
    setZValue(5);

    setHeight(DEFAULT_HEIGHT);
    setWidth(DEFAULT_WIDTH);
    setColorSaved(Qt::white);

}

QRectF CustomNodeGraphic::boundingRect() const
{
    int arrowHead = 0;
    if(isSelected()) arrowHead = 5;

    return QRectF(0, 0, width + hasShadow*SHADOW_OFFSET + arrowHead , height + hasShadow*SHADOW_OFFSET + arrowHead).adjusted(-2, -2, 6, 6);
}


void CustomNodeGraphic::paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *)
{

    if(!movable) setFlag(QGraphicsItem::ItemIsMovable,isSelected());

    textRect = QRectF(INSIDE_MARGIN, INSIDE_MARGIN, width-2*INSIDE_MARGIN, height-2*INSIDE_MARGIN);

    ///\todo nettoyer

    QString commentHTML = getComment().replace("\r\n","<br />");
    QString richText("<html><p style=\"white-space: pre; font-family: courier new;\">"+commentHTML+"</p></html>");

    setToolTip(richText);// getComment());
    if(getComment().simplified().isEmpty()) setToolTip("");

    if(! fontSize)
    {
        font.setPixelSize(FONT_SIZE);
        if(getShape()==MyShapes::Diamond) font.setPixelSize(TECHNICAL_FONT_SIZE);
    }
    else font.setPixelSize(fontSize);

    painter->setFont(font);
    painter->setRenderHint(QPainter::Antialiasing);

    // Dessin de l'ombre
    QPolygon polygonShadow;
    if(hasShadow)
    {
        painter->setPen(Qt::darkGray);
        painter->setBrush(Qt::darkGray);
        if(transparentContour) painter->setPen(Qt::transparent);
        if(shapeType==MyShapes::Ellipse) painter->drawEllipse(SHADOW_OFFSET, SHADOW_OFFSET, width, height);
        if(shapeType==MyShapes::Rectangle) painter->drawRect(SHADOW_OFFSET, SHADOW_OFFSET, width, height);
        if(shapeType==MyShapes::RoundedRectangle) painter->drawRoundedRect(SHADOW_OFFSET, SHADOW_OFFSET, width, height, 10, 10);
        if(shapeType==MyShapes::Diamond)
        {
            polygonShadow<<QPoint(width/2+SHADOW_OFFSET,SHADOW_OFFSET)<<QPoint(width+SHADOW_OFFSET,height/2+SHADOW_OFFSET)<<QPoint(width/2+SHADOW_OFFSET,height+SHADOW_OFFSET)<<QPoint(5,height/2+SHADOW_OFFSET);
            painter->drawPolygon(polygonShadow);
        }
    }

    // Dessin de la forme principale
    int weight = 1;
    if(!getDetailPath().isEmpty() && DISPLAY_SCHEMATICS) weight = DETAILED_WEIGHT;

    painter->setPen(QPen(Qt::black, weight));
    if(transparentContour) painter->setPen(Qt::transparent);

    painter->setBrush(getColorSaved());

    if(isSelected()) painter->setBrush(SELECTED_COLOR);

    QPolygon polygon;
    if(shapeType==MyShapes::Ellipse) painter->drawEllipse(0, 0, width, height);
    if(shapeType==MyShapes::Rectangle) painter->drawRect(0, 0, width, height);
    if(shapeType==MyShapes::RoundedRectangle) painter->drawRoundedRect(0, 0, width, height, 10, 10);
    if(shapeType==MyShapes::Diamond)
    {
        polygon<<QPoint(width/2,0)<<QPoint(width,height/2)<<QPoint(width/2,height)<<QPoint(0,height/2);
        painter->drawPolygon(polygon);
    }
    if(shapeType==MyShapes::Cylinder) drawCylinder(painter, hasShadow);

    QTextOption textOptions(QTextOption(Qt::AlignAbsolute | hzAlign | vtAlign));

    painter->setPen(QPen(Qt::black, weight));

    painter->drawText(textRect, text, textOptions);

    if(isSelected())
    {
        QPainterPath arrow;
        painter->setBrush(Qt::NoBrush);
        arrow.moveTo(width-10, height-10);
        arrow.lineTo(width-5, height-10);
        arrow.moveTo(width-10, height-10);
        arrow.lineTo(width-10, height-5);
        arrow.moveTo(width-9, height-9);
        arrow.lineTo(width+5, height+5);
        arrow.lineTo(width+5, height);
        arrow.moveTo(width+5, height+5);
        arrow.lineTo(width, height+5);
        painter->setPen(QPen(Qt::black, 3));
        painter->setFont(font);
        painter->drawPath(arrow);
    }
}

NodeGraphic* CustomNodeGraphic::createCopy(CustomGraphics* _graphics)
{
    //qDebug()<<"NodeGraphic* DCLFGraphic::createCopy()";
    CustomNodeGraphic* _copy = new CustomNodeGraphic(text, getComment(), shapeType, _graphics, false);
    _copy->setPos(pos().x() + COPY_OFFSET, pos().y() + COPY_OFFSET);
    _copy->setHeight(height);
    _copy->setWidth(width);
    _copy->setColor(getColorSaved());
    _copy->setColorSaved(getColorSaved());
    _copy->setDetailPath(getDetailPath());
    _copy->setTransparentContour(getTransparentContour());
    _copy->setHasShadow(getHasShadow());
    _copy->setZValue(zValue());
    _copy->setHzAlign(getHzAlign());
    _copy->setVtAlign(getVtAlign());
    _copy->setMovable(movable);
    _copy->setFont(font);
    _copy->setFontSize(fontSize);
    _copy->setFlag(QGraphicsItem::ItemIsSelectable,SELECTABLE_STATE);

    return _copy;
}



QFont CustomNodeGraphic::getFont() const
{
    return font;
}

void CustomNodeGraphic::setFont(const QFont &value)
{
    font = value;
}

int CustomNodeGraphic::getFontSize() const
{
    return fontSize;
}

void CustomNodeGraphic::setFontSize(int value)
{
    fontSize = value;
}
void CustomNodeGraphic::drawCylinder(QPainter *painter, bool _hasShadow)
{
    int portion = height/10;
    QBrush savedBrush = painter->brush();

    painter->setPen(Qt::NoPen);
    painter->setBrush(Qt::darkGray);

    if(_hasShadow)
    {
        QPainterPath shadowPath;
        shadowPath.moveTo(SHADOW_OFFSET, SHADOW_OFFSET);
        shadowPath.arcTo(SHADOW_OFFSET, SHADOW_OFFSET, width, 2*portion,180,-180);
        shadowPath.lineTo(width+SHADOW_OFFSET, height-portion);
        shadowPath.arcTo(SHADOW_OFFSET, height-2*portion+SHADOW_OFFSET, width, 2*portion, 0, -180);
        shadowPath.lineTo(SHADOW_OFFSET, portion);

        painter->drawPath(shadowPath);
    }

    int weight = 1;
    if(!getDetailPath().isEmpty() && DISPLAY_SCHEMATICS) weight = DETAILED_WEIGHT;

    painter->setPen(QPen(Qt::black, weight));
    painter->setBrush(savedBrush);

    QPainterPath path;
    path.moveTo(0, portion);
    path.arcTo(0, 0, width, 2*portion,180,-180);
    path.lineTo(width, height-portion);
    path.arcTo(0, height-2*portion, width, 2*portion, 0, -180);
    path.lineTo(0, portion);

    painter->drawPath(path);
    painter->drawArc(0, 0, width, 2*portion, 0, -180*16);

    textRect = QRectF(INSIDE_MARGIN , INSIDE_MARGIN+ 2*portion, width-2*INSIDE_MARGIN, height-2*INSIDE_MARGIN-2*portion);
}

QVariant CustomNodeGraphic::itemChange(GraphicsItemChange change, const QVariant &value)
{
    if (change == ItemPositionChange && scene())
    {
        foreach(LinkGraphic* link, getGraphics()->getLinks(this)) link->initLine();
        // on peut renvoyer une autre position pour forcer la position
    }
    return QGraphicsItem::itemChange(change, value);
}
