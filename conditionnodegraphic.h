#ifndef CONDITIONNODEGRAPHIC_H
#define CONDITIONNODEGRAPHIC_H

#include "nodegraphic.h"

/*! \class ConditionNodeGraphic
 *
 * \brief Type de noeud graphique permettant de réprésenter une condition dans le flux d'un programme
 *
 */
class ConditionNodeGraphic  : public NodeGraphic
{
private:
    QString text; /*! Texte affiché dans le noeud */

public:
    ConditionNodeGraphic(const QString &_text, const QString &_comment, CustomGraphics *_graphics, bool _automatic);
    QString getText() const { return text; }
    void setText(const QString &value) { text = value; }

    NodeGraphic* createCopy(CustomGraphics* _graphics);

    QRectF boundingRect() const;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);

};

#endif // CONDITIONNODEGRAPHIC_H
