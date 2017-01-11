#include "functiongraphic.h"
#include "linkgraphic.h"
#include <QTextOption>
#include "customgraphics.h"
#include <QPainter>


QString FunctionGraphic::getName() const { return name; }
void FunctionGraphic::setName(const QString &value){ name = value; }

QString FunctionGraphic::getLevel() const{ return level; }
void FunctionGraphic::setLevel(const QString &value) { level = value; }

QString FunctionGraphic::getCallMethod() const { return callMethod; }
void FunctionGraphic::setCallMethod(const QString &value) { callMethod = value; }

QString FunctionGraphic::getPrecision() const { return precision; }
void FunctionGraphic::setPrecision(const QString &value) { precision = value; }

FunctionGraphic::FunctionGraphic(const QString &_name, const QString &_level, const QString &_precision, const QString &_callMethod, const QString &_comment, CustomGraphics* _graphics, bool _automatic):
    NodeGraphic(-1,-1,"FUNCTION", _comment, _graphics, _automatic),
    name(_name),level(_level),
    callMethod(_callMethod),
    precision(_precision)
{
    setColorSaved(Qt::white);
    setColor(getColorSaved());

    setAcceptHoverEvents(true);

    setHeight(DEFAULT_HEIGHT);
    setWidth(DEFAULT_WIDTH);

    setFlag(QGraphicsItem::ItemIsMovable, true);
    setFlag(QGraphicsItem::ItemIsSelectable, SELECTABLE_STATE);
    setFlag(QGraphicsItem::ItemSendsGeometryChanges, true);
    setZValue(1);
}

QRectF FunctionGraphic::boundingRect() const
{
    if(isSelected())
    {
        int max = SHADOW_OFFSET;
        if(5 > SHADOW_OFFSET) max = 5;
        return QRectF(0,0 , width + max, height + max);
    }
    return QRectF(0, 0, width + SHADOW_OFFSET , height + SHADOW_OFFSET);
}

QString FunctionGraphic::getText() const
{
    QString text("");
    if(level!="" && level!="0") text += level + "\n";
    if(callMethod!="") text += callMethod + "\n";
    text += name;
    if(!precision.simplified().isEmpty()) text += "\n" + precision;

    return text;
}

void FunctionGraphic::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    //qDebug()<<"void FunctionGraphic::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)";
    Q_UNUSED(option);
    Q_UNUSED(widget);

    QString commentHTML = getComment();
    commentHTML.replace("<","&lt;");
    commentHTML.replace("\r\n","<br />");

    QString richText("<html><p style=\"white-space: pre; font-family: courier new;\">"+commentHTML+"</p></html>");

    setToolTip(richText);
    if(getComment().simplified().isEmpty()) setToolTip("");

    QFont font;
    font.setPixelSize(FONT_SIZE);
    painter->setFont(font);

    painter->setRenderHint(QPainter::Antialiasing);

    painter->setPen(Qt::NoPen);
    painter->setBrush(Qt::darkGray);
    painter->drawRoundedRect(SHADOW_OFFSET, SHADOW_OFFSET, width, height, 10, 10);

    painter->setBrush(getColor());
    if(isSelected()) painter->setBrush(SELECTED_COLOR);
    int weight = 1;
    if(!getDetailPath().isEmpty() && DISPLAY_SCHEMATICS) weight = DETAILED_WEIGHT;
    painter->setPen(QPen(Qt::black, weight));
    painter->drawRoundedRect(0, 0, width, height, 10, 10);

    QTextOption textOptions(QTextOption(Qt::AlignAbsolute | Qt::AlignHCenter | Qt::AlignVCenter));

    painter->setPen(Qt::black);
    painter->setBrush(Qt::black);
    painter->drawText(QRectF(INSIDE_MARGIN, INSIDE_MARGIN, width-2*INSIDE_MARGIN, height-2*INSIDE_MARGIN), getText(), textOptions);

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
        painter->setFont(font);
        painter->drawPath(arrow);
    }

}


void FunctionGraphic::hoverEnterEvent ( QGraphicsSceneHoverEvent *  )
{
    //qDebug()<<"void DCLFGraphic::hoverEnterEvent ( QGraphicsSceneHoverEvent *  )";
    getGraphics()->setColorToFunction(name, FILE_HOVER_COLOR);
    getGraphics()->updateScene();
}

void FunctionGraphic::hoverLeaveEvent ( QGraphicsSceneHoverEvent *  )
{
    //qDebug()<<"void DCLFGraphic::hoverLeaveEvent ( QGraphicsSceneHoverEvent *  )";
    getGraphics()->setColorToFunction(name, getColorSaved());
    getGraphics()->updateScene();
}

NodeGraphic* FunctionGraphic::createCopy(CustomGraphics *_graphics)
{
    //qDebug()<<"NodeGraphic* DCLFGraphic::createCopy()";
    FunctionGraphic* _copy = new FunctionGraphic(name, level, precision, callMethod, getComment(), _graphics, false);//name, classe, getComment(), getFormat(), getSegments(), getLength(), getGraphics());
    _copy->setPos(pos().x() + COPY_OFFSET, pos().y() + COPY_OFFSET);
    _copy->setHeight(height);
    _copy->setWidth(width);
    _copy->setColor(getColorSaved());
    _copy->setColorSaved(getColorSaved());
    _copy->setDetailPath(getDetailPath());
    _copy->setFlag(QGraphicsItem::ItemIsSelectable,SELECTABLE_STATE);

    return _copy;
}

QVariant FunctionGraphic::itemChange(GraphicsItemChange change, const QVariant &value)
{
    if (change == ItemPositionChange && scene())
    {
        foreach(LinkGraphic* link, getGraphics()->getLinks(this)) link->initLine();
        // on peut renvoyer une autre position pour forcer la position
    }
    return QGraphicsItem::itemChange(change, value);
}
