#ifndef ANCHORGRAPHIC_H
#define ANCHORGRAPHIC_H

#include <QGraphicsItem>
#include <QPainter>
#include "nodegraphic.h"
#include "customgraphics.h"
#include "linkgraphic.h"

/*! \class AnchorGraphic
 * \brief Classe représentant les ancres permettant de créer ou de supprimer des liens entres les noeuds
 *
 * Définition du comportement des ancres permettant de créer ou de supprimer des liens entres les noeuds :
 * Comportement au survol, au clic, création de lien, ...
 */
class AnchorGraphic : public QGraphicsItem
{
private:
    bool isOutput; /*!< Flag permettant de différencier une ancre d'entrée d'une ancre de sortie*/
    QColor color; /*!< Couleur de l'ancre*/
    bool selected; /*!< Flag permettant de savoir si l'ancre est selectionnée*/
    NodeGraphic* node; /*!< Noeud auquel l'ancre est liée*/

    QRectF boundingRect() const { return QRectF(0, 0, ANCHOR_SIZE, ANCHOR_SIZE);}

protected :
    virtual void paint (QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);
    virtual void hoverEnterEvent ( QGraphicsSceneHoverEvent* ) { setColor(Qt::red);}
    virtual void hoverLeaveEvent ( QGraphicsSceneHoverEvent* ) { if(!selected) setColor(Qt::black);}
    virtual void mousePressEvent ( QGraphicsSceneMouseEvent* event );

public:
    AnchorGraphic(bool, NodeGraphic* _component);
    ~AnchorGraphic(){}
    void setCreatingLink(bool _createdLink);
    void setPosition();
    void setColor(Qt::GlobalColor _color);
    CustomGraphics* getGraphics(){ return node->getGraphics(); }
};

#endif // ANCHORGRAPHIC_H
