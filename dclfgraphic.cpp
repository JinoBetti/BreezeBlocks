#include "utils.h"
#include "dclfgraphic.h"
#include<QPainter>
#include"customgraphics.h"

DCLFGraphic::DCLFGraphic(const QString &_name, const QString &_class, const QString &_comment, const QString &_format, const QString &_precision, int _length, CustomGraphics* _graphics, bool _automatic):
    NodeGraphic(-1,-1,"DCLF", _comment, _graphics, _automatic),
    name(_name),
    classe(_class),
    format(_format),
    precision(_precision),
    length(_length),
    hasBeenRead(false)
{
    setColorSaved(getColorFromRules(classe));
    setColor(getColorSaved());

    setAcceptHoverEvents(true);

    setHeight(DEFAULT_HEIGHT);
    setWidth(DEFAULT_WIDTH);

    setFlag(QGraphicsItem::ItemIsMovable, true);
    setFlag(QGraphicsItem::ItemIsSelectable,SELECTABLE_STATE);
    setFlag(QGraphicsItem::ItemSendsGeometryChanges,true);
    setZValue(1);
}

QRectF DCLFGraphic::boundingRect() const
{
    if(isSelected())
    {
        int max = SHADOW_OFFSET;
        if(5 > SHADOW_OFFSET) max = 5;
        return QRectF(0,0 , width + max+2, height + max+2);
    }
    return QRectF(0, 0, width + SHADOW_OFFSET , height + SHADOW_OFFSET).adjusted(-1, -1, 1, 1);
}

QString DCLFGraphic::getText() const
{
    QString text("");
    QString nameAlone = name;
    nameAlone.remove(0,name.lastIndexOf(".")+1);
    if(classe!="" && DISPLAY_DCLFCLASS) text += classe + "\n";
    text += nameAlone;
    if(!precision.isEmpty()) text += "\n" + precision;
    return text;
}

void DCLFGraphic::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    //qDebug()<<"void DCLFGraphic::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)";
    Q_UNUSED(option);
    Q_UNUSED(widget);

    setToolTip(getComment());
    QFont font;
    font.setPixelSize(FONT_SIZE);
    painter->setFont(font);

    painter->setRenderHint(QPainter::Antialiasing);

    drawCylinder(painter);

    painter->setPen(Qt::black);

    QTextOption textOptions(QTextOption(Qt::AlignAbsolute | Qt::AlignHCenter | Qt::AlignVCenter));

    painter->drawText(textRect, getText(), textOptions);

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
    //qDebug()<<"dclf"<<getName()<<"painted"<<QDateTime::currentDateTime();

}

void DCLFGraphic::drawCylinder(QPainter *painter)
{
    int portion = height/10;

    painter->setPen(Qt::NoPen);
    painter->setBrush(Qt::darkGray);

    QPainterPath shadowPath;
    shadowPath.moveTo(SHADOW_OFFSET, SHADOW_OFFSET);
    shadowPath.arcTo(SHADOW_OFFSET, SHADOW_OFFSET, width, 2*portion,180,-180);
    shadowPath.lineTo(width+SHADOW_OFFSET, height-portion);
    shadowPath.arcTo(SHADOW_OFFSET, height-2*portion+SHADOW_OFFSET, width, 2*portion, 0, -180);
    shadowPath.lineTo(SHADOW_OFFSET, portion);

    painter->drawPath(shadowPath);

    int weight = 1;
    if(!getDetailPath().isEmpty() && DISPLAY_SCHEMATICS) weight = DETAILED_WEIGHT;

    painter->setPen(QPen(Qt::black, weight));
    painter->setBrush(getColor());

    if(isSelected()) painter->setBrush(SELECTED_COLOR);

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

void DCLFGraphic::hoverEnterEvent (QGraphicsSceneHoverEvent *)
{
    //qDebug()<<"void DCLFGraphic::hoverEnterEvent ( QGraphicsSceneHoverEvent *  )";
    getGraphics()->setColorToDCLF(name, FILE_HOVER_COLOR);
    getGraphics()->updateScene();
}

void DCLFGraphic::hoverLeaveEvent (QGraphicsSceneHoverEvent *)
{
    //qDebug()<<"void DCLFGraphic::hoverLeaveEvent ( QGraphicsSceneHoverEvent *  )";
    getGraphics()->setColorToDCLF(name, getColorSaved());
    getGraphics()->updateScene();
}

NodeGraphic* DCLFGraphic::createCopy(CustomGraphics* _graphics)
{
    //qDebug()<<"NodeGraphic* DCLFGraphic::createCopy()";
    DCLFGraphic* _copy = new DCLFGraphic(name, classe, getComment(), getFormat(), getPrecision(), getLength(), _graphics, false);
    _copy->setPos(pos().x() + COPY_OFFSET, pos().y() + COPY_OFFSET);
    _copy->setHeight(height);
    _copy->setWidth(width);
    _copy->setColor(getColorSaved());
    _copy->setColorSaved(getColorSaved());
    _copy->setDetailPath(getDetailPath());
    _copy->setFlag(QGraphicsItem::ItemIsSelectable,SELECTABLE_STATE);

    return _copy;
}

QColor DCLFGraphic::getColorFromRules(QString _classe)
{
    foreach(const ColorRule &rule, COLOR_RULES)
    {
        if(!CLASS_SEPARATOR.isEmpty()) _classe = _classe.split(CLASS_SEPARATOR)[0];
        if(rule.classe == _classe) return rule.color;
    }
    return NORMAL_FILE_COLOR;
}

QVariant DCLFGraphic::itemChange(GraphicsItemChange change, const QVariant &value)
{
    if (change == ItemPositionChange && scene())
    {
        foreach(LinkGraphic* link, getGraphics()->getLinks(this)) link->initLine();
        // on peut renvoyer une autre position pour forcer la position
    }
    return QGraphicsItem::itemChange(change, value);
}
