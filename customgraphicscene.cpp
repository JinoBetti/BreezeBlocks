#include "customgraphicscene.h"
#include <QDebug>
#include <QMimeData>
#include <QGraphicsSceneDragDropEvent>
#include <QKeyEvent>
#include<QMenu>
#include <QPainter>
#include "utils.h"

CustomGraphicScene::~CustomGraphicScene()
{ qDebug()<<"Destructeur CustomGraphicsScene()";}

void CustomGraphicScene::dragEnterEvent(QGraphicsSceneDragDropEvent *e){ if (e->mimeData()->hasUrls()) e->acceptProposedAction(); }
void CustomGraphicScene::dragMoveEvent(QGraphicsSceneDragDropEvent *e){ if (e->mimeData()->hasUrls()) e->acceptProposedAction(); }

void CustomGraphicScene::drawBackground(QPainter *painter, const QRectF &rect)
{
    if(!DISPLAY_GRID) return;
    QPen pen(Qt::lightGray);
    QVector<qreal> dashes;
    dashes<<8<<4;
    pen.setDashPattern(dashes);
    painter->setPen(pen);

    qreal left = int(rect.left()) - (int(rect.left()) % ALIGN_GRID_X);
    qreal top = int(rect.top()) - (int(rect.top()) % ALIGN_GRID_Y);

    QVarLengthArray<QLineF, 100> lines;

    for (qreal x = left; x < rect.right(); x += ALIGN_GRID_X)
        lines.append(QLineF(x, rect.top(), x, rect.bottom()));
    for (qreal y = top; y < rect.bottom(); y += ALIGN_GRID_Y)
        lines.append(QLineF(rect.left(), y, rect.right(), y));

    //qDebug() << lines.size();

    painter->drawLines(lines.data(), lines.size());

    QVector<qreal> dashes2;
    dashes2<<2<<10;
    pen.setDashPattern(dashes2);

    painter->setPen(pen);

    QVarLengthArray<QLineF, 100> semiLines;

    for (qreal x = left; x < rect.right(); x += ALIGN_GRID_X)
    {
        for(int i=1; i<ALIGN_GRID_X_DIVISION; i++)
        {
            semiLines.append(QLineF(x+i*ALIGN_GRID_X/ALIGN_GRID_X_DIVISION, rect.top(), x+i*ALIGN_GRID_X/ALIGN_GRID_X_DIVISION, rect.bottom()));
        }
    }
    for (qreal y = top; y < rect.bottom(); y += ALIGN_GRID_Y)
    {
        for(int j=1; j<ALIGN_GRID_Y_DIVISION; j++)
        {
            semiLines.append(QLineF(rect.left(), y+j*ALIGN_GRID_Y/ALIGN_GRID_Y_DIVISION, rect.right(), y+j*ALIGN_GRID_Y/ALIGN_GRID_Y_DIVISION));
        }
    }

    painter->drawLines(semiLines.data(), semiLines.size());
}
