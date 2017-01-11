#ifndef DCLFGRAPHIC_H
#define DCLFGRAPHIC_H

#include "nodegraphic.h"
#include <QDebug>

class DCLFGraphic : public NodeGraphic
{
private:  
    QString name;
    QString classe;
    QString format;
    QString precision;
    int length;
    QRectF textRect;
    bool hasBeenRead; /// Allows the program to create a copy if a cycle is detected

public:
    void read(){ hasBeenRead = true; }
    bool alreadyRead(){ return hasBeenRead;}
    DCLFGraphic(const QString &_name, const QString &_class, const QString &_comment, const QString &_format, const QString &_precision, int _length, CustomGraphics* _graphics, bool _automatic);
    ~DCLFGraphic(){}
    QString getClass() const { return classe; }
    void setClass(const QString &_class){ classe = _class; }
    virtual void hoverEnterEvent ( QGraphicsSceneHoverEvent *  );
    virtual void hoverLeaveEvent ( QGraphicsSceneHoverEvent *  );
    NodeGraphic* createCopy(CustomGraphics* _graphics);

    QRectF boundingRect() const;
    void setName(const QString &_name){ name = _name; }
    QString getName() const { return name; }
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);

    int getLength() const { return length; }
    void setLength(int value) { length = value; }
    QString getFormat() const { return format; }
    void setFormat(const QString &value) { format = value; }

    static QColor getColorFromRules(QString _classe); // pas const => classe modifiée selon séparateur
    void drawCylinder(QPainter *painter);

    QString getPrecision() const { return precision; }
    void setPrecision(const QString &value) { precision = value; }

    QVariant itemChange(GraphicsItemChange change, const QVariant &value);
    QString getText() const;
};

#endif // DCLFGRAPHIC_H
