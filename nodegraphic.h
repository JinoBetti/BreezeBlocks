#ifndef NODEGRAPHIC_H
#define NODEGRAPHIC_H
#include "utils.h"
#include <QGraphicsItem>
#include<QMenu>
#include<QGraphicsSceneMouseEvent>
#include<QDebug>


class MainWindow;
class CustomGraphics;
class AnchorGraphic;
class SysinGraphic;
class ConditionGraphic;

class NodeGraphic : public QGraphicsObject
{
    Q_OBJECT
    Q_PROPERTY(int height READ getHeight WRITE setHeight)
    Q_PROPERTY(int width READ getWidth WRITE setWidth)

protected:
    int height;
    int width;

private:
    bool automatic;
    int heightSaved;
    int widthSaved;
    bool resizing;
    bool moving;
    int branchId;

    int id;
    int group;
    bool previousSelectedState;
    bool scanned;
    int median;

    float x;
    float y;
    QString type;
    QString comment;
    QString detailPath;
    CustomGraphics * graphics;
    QColor color;
    QColor colorSaved;
    QMenu *contextMenu;
    QAction *deleteLinks;
    QAction *editNode;
    QAction *editColor;
    QAction *resetColor;
    QAction *splitNode;
    QAction *deleteNode;
    QAction *copyNode;
    QAction *copySelection;
    QAction *deleteSelection;
    QAction *linkToDetail;
    QAction *deleteLinkToDetail;
    QAction *sendToBack;
    QAction *sendToFront;
    QAction *sortSelection;
    QAction *makeCustom;
    AnchorGraphic *inputAnchor;
    AnchorGraphic *outputAnchor;

    QPointF clickPosition;/// différence entre le point où l'on clique et la position du noeud

public:
    int getHeight(){return height;}
    int getWidth(){return width;}

    QPointF center();
    NodeGraphic(float _x, float _y, QString _type, QString _comment, CustomGraphics *_graphics, bool _automatic);
    ~NodeGraphic();
    void setType(QString _type){type=_type;}

    QString getType() const {return type;}
    int getId() const {return id;}
    void setId(int _id){id=_id;}
    QString getComment() const {return comment;}
    void setComment(const QString &_comment){comment = _comment; setToolTip(_comment);}
    virtual QString getName() const {return "";}
    virtual bool alreadyRead(){return false;}
    virtual void read(){}
    virtual NodeGraphic *createCopy(CustomGraphics* ){return 0;}
    virtual SysinGraphic *getSysinGraphic(){return 0;}
    virtual ConditionGraphic *getCondition(){return 0;}

    QColor getColor() const {return color;}
    void setColorSaved(const QColor &_colorSaved){colorSaved = _colorSaved;}
    QColor getColorSaved() const {return colorSaved;}
    void setColor(const QColor &_color){color = _color;}

    QColor getDefaultColor();

    virtual QString getClass() const {return "";}
    virtual QString getSegments() const {return "";}
    virtual QString getFormat() const {return "";}
    virtual int getLength() const {return 0;}
    virtual MyShapes::Shape getShape() const {return MyShapes::Rectangle;}
    virtual int getHzAlign() const {return 1;}
    virtual int getVtAlign() const {return 1;}
    virtual QString getProgram() const {return "";}
    virtual QString getSysinValue() const {return "";}
    virtual QString getConditionValue() const {return "";}
    virtual QString getPrecision() const {return "";}
    virtual QString getCallMethod() const {return "";}
    virtual QString getLevel() const {return "";}
    virtual bool getIsTechnical() const {return false;}
    virtual bool getHasShadow() const {return false;}
    virtual bool getTransparentBackground() const {return false;}
    virtual bool getTransparentContour() const {return false;}
    virtual bool getMovable() const {return true;}
    virtual QFont getFont() const {return QFont();}
    virtual int getFontSize() const {return 0;}
    virtual QString getSubtype() const {return "";}
    virtual QString getText() const {return "";}

    CustomGraphics *getGraphics() const {return graphics;}
    void setGraphics(CustomGraphics* _graphics);

    AnchorGraphic *getInputAnchor(){return inputAnchor;}
    AnchorGraphic *getOutputAnchor(){return outputAnchor;}

    float getYPos() const {return y;}
    float getXPos() const {return x;}
    void setXPos(float _x);
    void setYPos(float _y);
    void setXPosNoMove(float _x) { x = _x;}
    void setYPosNoMove(float _y){ y = _y; }

    void setHeight(int _height);
    void setWidth(int _width);

    QString getDetailPath() const { return detailPath; }
    void setDetailPath(const QString &value) { detailPath = value; }

    int getGroup() const { return group; }
    void setGroup(int value) { group = value; }

    bool getScanned() const { return scanned; }
    void setScanned(bool value) { scanned = value; }

    QString getAbsoluteDetailPath() const;

    virtual void repositionSubComponents();
    bool getAutomatic() const { return automatic; }
    void setAutomatic(bool value) { automatic = value; }

    int getMedian() const { return median; }
    void setMedian(int value) { median = value; }

    int getBranchId() const { return branchId; }
    void setBranchId(int value) { branchId = value; }

public slots:
    int edit();

public:
    void mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event);
    void mousePressEvent(QGraphicsSceneMouseEvent *event);
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event);
    void autoAdjustSize();
};

#endif // NODEGRAPHIC_H
