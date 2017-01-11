#ifndef CUSTOMGRAPHICSCENE_H
#define CUSTOMGRAPHICSCENE_H

#include<QGraphicsScene>

class CustomGraphicScene : public QGraphicsScene
{
    Q_OBJECT
private:
    QPainterPath path;
public:
    explicit CustomGraphicScene(QObject *parent = 0) : QGraphicsScene(parent){}
    ~CustomGraphicScene();
    void drawBackground(QPainter *painter, const QRectF &rect);
    void dragEnterEvent(QGraphicsSceneDragDropEvent *e);
    void dragMoveEvent(QGraphicsSceneDragDropEvent *e);
};

#endif // CUSTOMGRAPHICSCENE_H
