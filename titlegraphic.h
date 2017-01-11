#ifndef TITLEGRAPHIC_H
#define TITLEGRAPHIC_H

#include <QGraphicsItem>
#include "mainwindow.h"

class TitleGraphic : public QGraphicsItem
{
private:
    QString title;
    QString version;
    CustomGraphics* graphics;
    float x;
    float y;
    int width;
    int height;

public:
    TitleGraphic(const QString &_title, CustomGraphics* _graphics);
    ~TitleGraphic(){}
    QRectF boundingRect() const { return QRectF(0, 0, width , height);}
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);

    void setPosition();
    int getWidth() const { return width;}
    void setWidth(int value) { width = value; }
    int getHeight() const{ return height;}
    void setHeight(int value) { height = value;}
    QString getTitle() const {return title;}
    void setTitle(const QString &value);

    CustomGraphics *getGraphics() const{ return graphics; }
    void setGraphics(CustomGraphics *value) { graphics = value;}

    QString getVersion() const;
    void setVersion(const QString &value);

    QString getText() const;
protected:
    void mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event);
};

#endif // TITLEGRAPHIC_H
