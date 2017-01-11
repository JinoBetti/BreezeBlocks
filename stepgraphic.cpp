#include "utils.h"
#include "stepgraphic.h"
#include<QPainter>
#include"sysingraphic.h"
#include "customgraphics.h"


void StepGraphic::repositionSubComponents()
{
    NodeGraphic::repositionSubComponents();
    sysin->setPosition();
}

StepGraphic::StepGraphic(const QString &_stepName, const QString &_type, const QString &_program, const QString &_comment, CustomGraphics *_graphics, bool _automatic):
    NodeGraphic(-1,-1,"STEP", _comment, _graphics, _automatic),
    subtype(_type),
    stepName(_stepName),
    program(_program),
    precision(""),
    sysin(new SysinGraphic("", this)),
    condition(new ConditionGraphic("", this)),
    isTechnical(false)
{
    setFlag(QGraphicsItem::ItemIsMovable, true);
    setFlag(QGraphicsItem::ItemIsSelectable,SELECTABLE_STATE);
    setFlag(QGraphicsItem::ItemSendsGeometryChanges, true);
    setZValue(1);

    setHeight(DEFAULT_HEIGHT);
    setWidth(DEFAULT_WIDTH);

    if(TECHNICAL_LIST.contains(program)) isTechnical = true;

    if(subtype=="PATTERN") setColorSaved(PATTERN_STEP_COLOR);
    else if(isTechnical) setColorSaved(TECHNICAL_STEP_COLOR);
    else setColorSaved(NORMAL_STEP_COLOR);

    // Création de la flèche sysin, cachée par défaut
    sysin->setParentItem(this);
    sysin->hide();

    // Création du coin rouge condition, caché par défaut
    condition->setParentItem(this);
    condition->hide();

    setToolTip(getComment());
}

QRectF StepGraphic::boundingRect() const
{
    int weight = getDetailPath().isEmpty() ? 0 : DETAILED_WEIGHT;
    return QRectF(-weight, -weight, width + SHADOW_OFFSET + weight+2, height + SHADOW_OFFSET+weight+2).adjusted(-1, -1, 1, 1);
}

QString StepGraphic::getText() const
{
    QString text("");
    if(stepName!="" && DISPLAY_STEPNAME) text += stepName + "\n";
    text += program;
    if(precision!="") text += "\n" + precision;
    return text;
}

void StepGraphic::paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *)
{
    painter->setRenderHint(QPainter::Antialiasing);
    QFont font;
    font.setPixelSize(FONT_SIZE);
    if(isTechnical) font.setPixelSize(TECHNICAL_FONT_SIZE);
    painter->setFont(font);

    QPolygon polygonShadow, polygon;

    if(isTechnical)
    {
        polygon<<QPoint(width/2,0)<<QPoint(width,height/2)<<QPoint(width/2,height)<<QPoint(0,height/2);
        polygonShadow<<QPoint(width/2+SHADOW_OFFSET,SHADOW_OFFSET)<<QPoint(width+SHADOW_OFFSET,height/2+SHADOW_OFFSET)<<QPoint(width/2+SHADOW_OFFSET,height+SHADOW_OFFSET)<<QPoint(5,height/2+SHADOW_OFFSET);
    }
    else
    {
        polygon = QRect(0,0,width, height);
        polygonShadow = QRect(SHADOW_OFFSET,SHADOW_OFFSET,width, height);
    }

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
    painter->drawText(QRectF(INSIDE_MARGIN, INSIDE_MARGIN, width-2*INSIDE_MARGIN, height-2*INSIDE_MARGIN), getText(), textOptions);

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

QString StepGraphic::getSysinValue() const{ return sysin->getSysin();}

QString StepGraphic::getConditionValue() const{ return condition->getCondition();}

void StepGraphic::setSysin(const QString &_sysin)
{
    sysin->setSysin(_sysin);
    sysin->setVisible(!SYSIN_HIDDEN && !_sysin.simplified().isEmpty());
}

NodeGraphic* StepGraphic::createCopy(CustomGraphics *_graphics)
{
    StepGraphic* _copy = new StepGraphic(stepName, subtype, program, getComment(), _graphics, false);
    _copy->setSysin(getSysinValue());
    _copy->setCondition(getConditionValue());
    _copy->setPos(pos().x() + COPY_OFFSET, pos().y() + COPY_OFFSET);
    _copy->setHeight(height);
    _copy->setWidth(width);
    _copy->setColor(getColorSaved());
    _copy->setColorSaved(getColorSaved());
    _copy->setPrecision(precision);
    _copy->setIsTechnical(isTechnical);
    _copy->setFlag(QGraphicsItem::ItemIsSelectable,SELECTABLE_STATE);
    _copy->setDetailPath(getDetailPath());

    return _copy;
}

void StepGraphic::setCondition(const QString &value)
{
    condition->setCondition(value);
    condition->setVisible(CONDITIONS_DISPLAYED && !value.simplified().isEmpty());
}


QVariant StepGraphic::itemChange(GraphicsItemChange change, const QVariant &value)
{
    if (change == ItemPositionChange && scene())
    {
        foreach(LinkGraphic* link, getGraphics()->getLinks(this)) link->initLine();
        // on peut renvoyer une autre position pour forcer la position
    }
    return QGraphicsItem::itemChange(change, value);
}
