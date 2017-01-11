#ifndef LINKGRAPHIC_H
#define LINKGRAPHIC_H
#include <QGraphicsItem>
#include <QLabel>
#include "dclfgraphic.h"
#include "stepgraphic.h"

class CustomGraphics;
class LinkTextGraphic;
class ArrowHeadGraphic;

class LinkGraphic: public QGraphicsPathItem
{
private:
    bool automatic;
    NodeGraphic* input;
    NodeGraphic* output;
    QLineF line;
    CustomGraphics* graphics;
    QString DDName;
    int weight;
    int shapeType;
    Qt::PenStyle penStyle;
    int ponderation;
    bool clicked;
    bool deformed;
    bool scanned;
    bool endShape;

    bool alreadyInMenu;

    int cutValue;

    LinkTextGraphic* text;

    qreal alpha;
    QPolygonF arrowHead;

    QMenu *contextMenu;
    QAction *deleteLink;

public:
    LinkGraphic(NodeGraphic *_input, NodeGraphic *_output, const QString &_DDName, CustomGraphics* _graphics, bool _automatic);
    ~LinkGraphic(){}
    QPointF getArrowBasePoint();
    QPointF center() const { return pos()+QPointF(boundingRect().width()/2, boundingRect().height()/2); }

    NodeGraphic *getInput() const { return input;}
    void setInput(NodeGraphic *value) { input = value;}

    NodeGraphic *getOutput() const { return output;}
    void setOutput(NodeGraphic *value) { output = value;}

    QString getDDName() const { return DDName;}
    void setDDName(const QString &value) { DDName = value;}

    float getSlack() { return output->getYPos() - input->getYPos() - 1;}

    CustomGraphics *getGraphics() const { return graphics;}
    void setGraphics(CustomGraphics *value) { graphics = value;}

    int getShape() const { return shapeType;}
    void setShape(int value) { shapeType = value;}

    int getPonderation() const { return ponderation;}
    void setPonderation(int value);

    bool getScanned() const { return scanned;}
    void setScanned(bool value) { scanned = value;}

    void initLine();

    bool getAutomatic() const { return automatic;}
    void setAutomatic(bool value) { automatic = value;}

    int getCutValue() const { return cutValue; }
    void setCutValue(int value) { cutValue = value;}

    int getGroup() { return input->getGroup();}

    void mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event);
    void mousePressEvent(QGraphicsSceneMouseEvent *event);
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event);

    virtual void hoverEnterEvent ( QGraphicsSceneHoverEvent *  );
    virtual void hoverLeaveEvent ( QGraphicsSceneHoverEvent *  );
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);

    int getWeight() const { return weight; }
    void setWeight(int value) { weight = value; }

    QRectF boundingRect() const;
    QPainterPath shape() const;
    Qt::PenStyle getPenStyle() const;
    void setPenStyle(Qt::PenStyle value);
    int getEndShape() const;
    void setEndShape(int value);
    QPainterPath getPathAtPonderation (float ponderationDelta) const;
};


class LinkTextGraphic : public QGraphicsItem
{
public:
    LinkTextGraphic(LinkGraphic* _link):link(_link)
    {
        setZValue(1);
        setAcceptHoverEvents(true);
    }
    LinkGraphic *getLink() const { return link; }
    void setLink(LinkGraphic *value) { link = value; }

private:
    LinkGraphic* link;
protected:
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);

public:
    QRectF boundingRect() const;
    void resetPos();

    void hoverEnterEvent(QGraphicsSceneHoverEvent *event) {link->hoverEnterEvent(event);}
    void hoverLeaveEvent(QGraphicsSceneHoverEvent *event){link->hoverLeaveEvent(event);}
    void mousePressEvent(QGraphicsSceneMouseEvent *event){link->mousePressEvent(event);}
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event);
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event){link->mouseReleaseEvent(event);}
    void mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event){link->mouseDoubleClickEvent(event);}
};

#endif // LINKGRAPHIC_H
