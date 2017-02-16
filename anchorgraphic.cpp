#include "anchorgraphic.h"
#include "utils.h"

/*!
 *  \fn AnchorGraphic::AnchorGraphic(bool _isOutput, NodeGraphic* _node, CustomGraphics *_graphics)
 *  \brief Constructeur de la classe AnchorGraphic
 *
 *  Initialisation des attributs de la classe, et des différents paramètres de représentation
 *
 *  \param _isOutput : Statut de l'ancre : entrée ou sortie
 *  \param _node : Noeud auquel l'ancre sera liée
 *  \param _graphics : Scène dans laquelle l'ancre sera dessinée
 */
AnchorGraphic::AnchorGraphic(bool _isOutput, NodeGraphic* _node) :
    isOutput(_isOutput),
    selected(false),
    node(_node)
{
    setColor(Qt::black);
    setZValue(10);
    setAcceptHoverEvents(true);
    setCursor(Qt::ArrowCursor);
}

/*!
 *  \fn void AnchorGraphic::setPosition()
 *  \brief Positionnement de l'ancre
 *
 *  Positionnement de l'ancre, en fonction des coordonnées du noeud auquel elle est liée, et de son statut d'entrée ou sortie
 */
void AnchorGraphic::setPosition()
{
    qreal xPos = node->getWidth()/2 - ANCHOR_SIZE/2,
          yPos = -ANCHOR_SIZE/2;
    if(isOutput)
    {
        xPos = node->getWidth()/2 - ANCHOR_SIZE/2;
        yPos = node->getHeight()-ANCHOR_SIZE/2;
    }
    setPos(xPos, yPos);
}

/*!
 *  \fn void AnchorGraphic::paint(QPainter* painter, const QStyleOptionGraphicsItem *, QWidget *)
 *  \brief Dessin de l'ancre
 *
 *  Positionnement de l'ancre avec AnchorGraphic::setPosition()
 *  Réglage du pinceau (Couleur du contour, du fond...)
 *  Dessin d'un cercle, dont la taille est paramétrable dans la fenêtre préférences
 *
 * \param painter : QPainter ("pinceau") utilisé pour dessiner l'ancre
 *
 */
void AnchorGraphic::paint(QPainter* painter, const QStyleOptionGraphicsItem *, QWidget *)
{
    painter->setRenderHint(QPainter::Antialiasing);
    painter->setPen(Qt::white);
    painter->setBrush(color);
    painter->drawEllipse(0,0,ANCHOR_SIZE,ANCHOR_SIZE);
}

/*!
 *  \fn void AnchorGraphic::setCreatingLink(bool _createdLink)
 *  \brief Définition du statut (sélectionné ou non) de l'ancre
 *
 *  Modification de l'attribut selected
 *  Coloration en noir de l'ancre
 *  Cette méthode n'est en fait utilisée que pour mettre l'attribut selected à false lorsqu'une action sur une autre ancre implique une déselection de toutes les ancres.
 *
 * \param _createdLink : statut de l'ancre, sélectionnée ou non
 *
 */
void AnchorGraphic::setCreatingLink(bool _createdLink)
{
    selected = _createdLink;
    if(!selected) setColor(Qt::black);
}

/*!
 *  \fn void AnchorGraphic::setColor(Qt::GlobalColor _color)
 *  \brief Change la couleur de l'ancre
 *
 * \param _color : Nouvelle couleur de l'ancre
 *
 */
void AnchorGraphic::setColor(Qt::GlobalColor _color)
{
    color = _color;
    update(boundingRect());
}

/*!
 *  \fn void AnchorGraphic::mousePressEvent ( QGraphicsSceneMouseEvent * event )
 *  \brief Fonction lancée lors du clic sur une ancre
 *
 *  \param event : Evenement correspondant au clic
 */
void AnchorGraphic::mousePressEvent ( QGraphicsSceneMouseEvent * event )
{
    event->setAccepted(true);
    ///> Inversion du statut de sélection
    selected = !selected;

    ///> clic sur une ancre alors qu'aucune autre n'est sélectionnée : on sauvegarde l'ancre
    if(selected && getGraphics()->getNbClickedAnchors() == 0)
    {
        getGraphics()->setFirstClickedAnchor(this);
        getGraphics()->setNbClickedAnchors(1);
    }
    ///> clic sur une ancre alors qu'une est déjà sélectionnée
    else if(selected && getGraphics()->getNbClickedAnchors() == 1)
    {
        ///>> Si les deux ancres sont de même type (2 entrées ou 2 sorties) => on échange simplement les ancres
        if(isOutput == getGraphics()->getFirstClickedAnchor()->isOutput)
        {
            getGraphics()->getFirstClickedAnchor()->setCreatingLink(false);
            getGraphics()->setFirstClickedAnchor(this);
        }
        ///>> Si on a une entrée et une sortie, on créé le lien ou on le supprime s'il existait
        else
        {
            selected = false;

            getGraphics()->setNbClickedAnchors(0);
            getGraphics()->getFirstClickedAnchor()->setCreatingLink(false);

            ///>>> initialisation de l'entrée et de la sortie
            NodeGraphic* input = node, *output = node;
            if(!isOutput) input = getGraphics()->getFirstClickedAnchor()->node;
            else output = getGraphics()->getFirstClickedAnchor()->node;


            ///>>> Création du lien
            if(!getGraphics()->linkExists(input, output))
            {
                getGraphics()->saveTemporaryFile("Création du lien entre " + input->getName() + " et " + output->getName());
                LinkGraphic* link = new LinkGraphic(input, output, "", getGraphics(), false);
                getGraphics()->getMain()->setStatus("Lien créé entre " + input->getName() + " et " + output->getName());
                getGraphics()->addLink(link);
            }
            ///>>> Suppression du lien
            else
            {
                getGraphics()->saveTemporaryFile("Suppression du lien entre " + input->getName() + " et " + output->getName());
                getGraphics()->deleteLink(input, output);
                getGraphics()->getMain()->setStatus("Lien supprimé entre " + input->getName() + " et " + output->getName());
            }
        }
    }
    ///> Si on reclique sur la même ancre, on la déselectionne et on considère qu'on n'est plus en train de créer un lien
    else getGraphics()->setNbClickedAnchors(0);
}
