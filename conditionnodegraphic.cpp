#include "conditionnodegraphic.h"
#include<QPainter>
#include<QTextOption>

NodeGraphic *ConditionNodeGraphic::createCopy(CustomGraphics *_graphics)
{
    ConditionNodeGraphic* _copy = new ConditionNodeGraphic(text, getComment(), _graphics, false);
    return _copy;
}

QRectF ConditionNodeGraphic::boundingRect() const
{
    if(isSelected())
    {
        int max = SHADOW_OFFSET;
        if(5 > SHADOW_OFFSET) max = 5;
        return QRectF(0, 0, width + max + 2, height + max + 2);
    }
    return QRectF(0, 0, width + SHADOW_OFFSET, height + SHADOW_OFFSET);
}

void ConditionNodeGraphic::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    setToolTip(getComment());

    Q_UNUSED(option);
    Q_UNUSED(widget);

    QPolygon polygonShadow, polygon;

    polygon<<QPoint(width/2,0)<<QPoint(width,height/2)<<QPoint(width/2,height)<<QPoint(0,height/2);
    polygonShadow<<QPoint(width/2+SHADOW_OFFSET,SHADOW_OFFSET)<<QPoint(width+SHADOW_OFFSET,height/2+SHADOW_OFFSET)<<QPoint(width/2+SHADOW_OFFSET,height+SHADOW_OFFSET)<<QPoint(5,height/2+SHADOW_OFFSET);

    // Draw shadow
    painter->setPen(Qt::NoPen);
    painter->setBrush(Qt::darkGray);
    painter->drawPolygon(polygonShadow);

    // Draw Box
    int weight = 1;
    if(!getDetailPath().isEmpty() && DISPLAY_SCHEMATICS) weight = DETAILED_WEIGHT;
    painter->setPen(QPen(Qt::black, weight));
    painter->setBrush(getColorSaved());
    if(isSelected()) painter->setBrush(SELECTED_COLOR);
    painter->drawPolygon(polygon);

    QTextOption textOptions(QTextOption(Qt::AlignAbsolute | Qt::AlignHCenter | Qt::AlignVCenter));
    painter->drawText(QRectF(INSIDE_MARGIN, INSIDE_MARGIN, width-2*INSIDE_MARGIN, height-2*INSIDE_MARGIN), text, textOptions);

    if(isSelected())
    {
        QPainterPath arrow;
        painter->setBrush(Qt::NoBrush);
        arrow.moveTo(width-10, height-10);
        arrow.lineTo(width-5, height-10);
        arrow.moveTo(width-10, height-10);
        arrow.lineTo(width-10, height-5);
        arrow.moveTo(width-10, height-10);
        arrow.lineTo(width+5, height+5);
        arrow.lineTo(width+5, height);
        arrow.moveTo(width+5, height+5);
        arrow.lineTo(width, height+5);
        painter->setPen(QPen(Qt::black, 3));
        painter->drawPath(arrow);
    }
}

ConditionNodeGraphic::ConditionNodeGraphic(const QString &_text, const QString &_comment, CustomGraphics* _graphics, bool _automatic):
    NodeGraphic(-1,-1,"CONDITION", _comment, _graphics, _automatic),
    text(_text)
{
    setColorSaved(Qt::white);
    setAcceptHoverEvents(true);

    setHeight(DEFAULT_HEIGHT);
    setWidth(DEFAULT_WIDTH);

    setFlag(QGraphicsItem::ItemIsMovable, true);
    setFlag(QGraphicsItem::ItemIsSelectable,SELECTABLE_STATE);
    setZValue(1);
}
