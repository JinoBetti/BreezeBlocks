#include "linkgraphic.h"
#include<QPainter>
#include<QPainterPath>
#include<QDebug>
#include "customgraphics.h"
#include "utils.h"
#include "modifylinkdialog.h"

/*
enum PenStyle :
    NoPen,
    SolidLine,
    DashLine,
    DotLine,
    DashDotLine,
    DashDotDotLine,
    CustomDashLine
*/

Qt::PenStyle LinkGraphic::getPenStyle() const { return penStyle; }

void LinkGraphic::setPenStyle(Qt::PenStyle value)
{
    if(value <= Qt::NoPen || value > Qt::DashDotLine) penStyle = Qt::SolidLine;
    else penStyle = value;
}

int LinkGraphic::getEndShape() const { return endShape; }

void LinkGraphic::setEndShape(int value) { endShape = value; }

LinkGraphic::LinkGraphic(NodeGraphic *_input, NodeGraphic *_output, const QString &_DDName, CustomGraphics *_graphics, bool _automatic):
    automatic(_automatic),
    input(_input),
    output(_output),
    DDName(_DDName),
    line(),
    graphics(_graphics),
    shapeType(0),
    penStyle(Qt::SolidLine),
    ponderation(50),
    clicked(false),
    deformed(false),
    scanned(false),
    endShape(true),
    cutValue(0),
    text(new LinkTextGraphic(this)),
    contextMenu(new QMenu)
{
    //qDebug()<<"LinkGraphic";
    setWeight(2);
    if(input->getType()=="DCLF") input->read();
    if(output->alreadyRead() && _automatic && ! (output->getType()=="VIRTUAL" || input->getType()=="VIRTUAL"))
    {
        qDebug()<<"LinkGraphic::LinkGraphic creating copy of node"<<output->getName();
        NodeGraphic* copy = output->createCopy(getGraphics());
        graphics->addNode(copy);
        output = copy;
    }

    setZValue(2);
    setToolTip(getDDName());
    setAcceptHoverEvents(true);
    text->setParentItem(this);
    initLine();
    alreadyInMenu = false;

    deleteLink = contextMenu->addAction("Supprimer le lien");
}

void LinkGraphic::mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event)
{
    event->accept();
    if(event->button()==Qt::LeftButton )
    {
        ModifyLinkDialog modifyLinkWindow(this, graphics);
        modifyLinkWindow.exec();
    }
}

void LinkGraphic::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    clicked = true;
    if(event->button()==Qt::RightButton &&!alreadyInMenu)
    {
        alreadyInMenu = true;
        QAction* myAction = contextMenu->exec(event->screenPos());
        if(myAction == deleteLink)
        {
            graphics->deleteLink(getInput(),getOutput());
        }
        else event->ignore();
    }
    else alreadyInMenu = false;

    event->accept();
}

void LinkGraphic::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    QGraphicsPathItem::mouseReleaseEvent(event);
    event->accept();
    clicked = false;
    if(deformed) graphics->getMain()->setStatus("Lien déformé");
    deformed = false;
    graphics->getTitle()->setPosition();
}

void LinkGraphic::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    /// Si lien direct : on ne fait rien, la pondération n'étant pas utilisée
    int linkTypeWithDefault = LINK_TYPE;
    if(shapeType != 0) linkTypeWithDefault = shapeType - 1;
    if(linkTypeWithDefault == MyShapes::StraightLine) return;

    if(clicked)
    {
        if(!graphics->mapToScene(graphics->rect()).boundingRect().contains(mapToScene(event->pos()).toPoint())) return;

        if(!deformed)
        {
            deformed = true;
            graphics->saveTemporaryFile("Déformation d'un lien");
        }
        prepareGeometryChange();
        float x1 = input->pos().x() + input->getWidth()/2,
              y1 = input->pos().y() + input->getHeight(),
              x2 = output->pos().x() + output->getWidth()/2,
              y2 = output->pos().y();
        int newPonderation;

        /// Cas d'un lien qui remonte
        if(y1>y2)
        {
            if((x1<event->pos().x() && event->pos().x()<=x2) ||(x2<event->pos().x() && event->pos().x()<=x1)) newPonderation = (event->pos().x()-x1)*100/(x2-x1);
            else if(event->pos().x()>x1 && event->pos().x()>x2)
            {
                newPonderation = x1 - event->pos().x(); //ok
                if(x2>x1) newPonderation = 100 + event->pos().x() - x2 ;//ok
            }
            else if(event->pos().x()<x1 && event->pos().x()<x2)
            {
                newPonderation = 100 + x2-event->pos().x();//ok
                if(x2>x1) newPonderation = event->pos().x()-x1;
            }
            if(x2==x1)
            {
                if(event->pos().x()<x1) newPonderation = event->pos().x()-x1;
                else newPonderation = 100 + event->pos().x()-x1;
            }

        }
        /// Cas normal d'un lien qui descend
        else  newPonderation = (event->pos().y()-y1)*100/(y2-y1);
        setPonderation(newPonderation);
    }
}

void LinkGraphic::initLine()
{
    setPath(getPathAtPonderation(0));
    text->resetPos();
}

/**
 * \fn void LinkGraphic::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
 * \brief Fonction de dessin des liens entre les noeuds
 *
 * \param painter QPainter permettant de dessiner le lien
 * \param option Non utilisé
 * \param widget Non utilisé
 * \return Aucun
 *
*/

void LinkGraphic::paint(QPainter *painter, const QStyleOptionGraphicsItem*, QWidget*)
{
    QPen pen(Qt::black, weight, (Qt::PenStyle)penStyle, Qt::RoundCap, Qt::RoundJoin);
    painter->setPen(pen);
    painter->setRenderHint(QPainter::Antialiasing);

    initLine();
    painter->drawPath(path());

    /// Dessiner la flèche finale
    painter->setBrush(Qt::black);
    /// Calcul des constantes
    float x2 = output->pos().x() + output->getWidth()/2,
          y2 = output->pos().y();
    float size= 5* weight;
    if(endShape)
    {
        getArrowBasePoint(); //initialize alpha to avoid alpha=nan which causes bugs on undo and redo
        QPointF mouvement(size/2*qCos(qDegreesToRadians(alpha)),-size/2*qSin(qDegreesToRadians(alpha)));
        arrowHead.clear();
        arrowHead<<getArrowBasePoint()<<getArrowBasePoint()+mouvement<<QPointF(x2,y2)<<getArrowBasePoint()-mouvement;
        painter->setPen(QPen());

        painter->drawPolygon(arrowHead);
    }
//    painter->setPen(Qt::black);
//    painter->setBrush(Qt::transparent);
//    painter->drawRect(text->boundingRect().translated(text->pos()));
}

void LinkGraphic::hoverEnterEvent(QGraphicsSceneHoverEvent *)
{
    setWeight(6);
    update();
}

void LinkGraphic::hoverLeaveEvent(QGraphicsSceneHoverEvent *)
{
    setWeight(2);
    update();
}

void LinkGraphic::setPonderation(int value)
{
    ponderation = value;
    float y1 = input->pos().y() + input->getHeight(),
          y2 = output->pos().y();
    if(y1>=y2) return;
    if(ponderation > 100) ponderation = 100;
    if(ponderation < 0) ponderation = 0;
}

QPointF LinkGraphic::getArrowBasePoint()
{
    float x1 = input->pos().x() + input->getWidth()/2,
          y1 = input->pos().y() + input->getHeight(),
          x2 = output->pos().x() + output->getWidth()/2,
          y2 = output->pos().y();
    float truePonderation = float(ponderation);
    if(truePonderation > 100) truePonderation = 100;
    if(truePonderation < 0) truePonderation = 0;
    float a = truePonderation/100,
          yctrl = ((1-a)*y1+a*y2);
    float size = 5*weight;

    /// Angle de rotation de la flèche
    alpha = qRadiansToDegrees(qAtan((y2-y1)/(x2-x1)));

    // Par défaut
    if(x1==x2) alpha = 0;
    else alpha = alpha-90*(x2-x1)/qAbs(x2-x1);
    // Si lien en lignes droites
    if(shapeType==3 || (!shapeType && LINK_TYPE==MyShapes::BrokenLine))
    {
        alpha = 0;
    }
    // Si lien courbe
    if(shapeType==2 || (!shapeType && LINK_TYPE==MyShapes::CurvedLine))
    {
        /// Cas particuliers => alpha = 0 et on zappe le calcul
        if(y2<y1 || x2==x1 || size==0) alpha = 0;
        else
        {
            /// Dichotomie pour trouver le bon angle approximativement
            double xi, yi, di, ti=0.5, deltai;
            /// On cherche le point de concours entre le cercle de rayon size et la courbe de Bézier
            for(int i=0; i<5; i++)
            {
                xi = x1*qPow(1-ti,3)+3*x1*ti*qPow(1-ti,2)+3*x2*qPow(ti,2)*(1-ti)+x2*qPow(ti,3);
                yi = y1*qPow(1-ti,3)+3*yctrl*ti*qPow(1-ti,2)+3*yctrl*qPow(ti,2)*(1-ti)+y2*qPow(ti,3);
                di = (xi-x2)*(xi-x2) + (yi-y2)*(yi-y2);
                deltai = qPow(0.5,i+2);
                if(di<(size-2*weight)*(size-2*weight)) deltai = -deltai;
                ti += deltai;
            }

            xi = x1*qPow(1-ti,3)+3*x1*ti*qPow(1-ti,2)+3*x2*qPow(ti,2)*(1-ti)+x2*qPow(ti,3);
            yi = y1*qPow(1-ti,3)+3*yctrl*ti*qPow(1-ti,2)+3*yctrl*qPow(ti,2)*(1-ti)+y2*qPow(ti,3);

            alpha = qRadiansToDegrees(qAtan((yi-y2)/(xi-x2)))-90*(x2-x1)/qAbs(x2-x1);
        }
    }

    alpha = -alpha;

    return QPointF(x2 - size*qSin(qDegreesToRadians(alpha)), y2 - size*qCos(qDegreesToRadians(alpha)));
}

QRectF LinkGraphic::boundingRect() const { return QGraphicsPathItem::boundingRect().adjusted(-16,-16,16,16); }

QPainterPath LinkGraphic::shape() const
{
    //QPainterPath myPath = QGraphicsPathItem::shape();
    float x1 = input->pos().x() + input->getWidth()/2,
    y1 = input->pos().y() + input->getHeight();
    QPainterPath myPath = QPainterPath(QPointF(x1,y1));
    myPath.addPath(getPathAtPonderation(10));
    myPath.addPath(getPathAtPonderation(-10));
    myPath.addPolygon(arrowHead);
    return myPath;
}

QPainterPath LinkGraphic::getPathAtPonderation(float ponderationDelta) const
{
    /// Calcul des constantes
    float x1 = input->pos().x() + input->getWidth()/2,
          y1 = input->pos().y() + input->getHeight(),
          x2 = output->pos().x() + output->getWidth()/2,
          y2 = output->pos().y();

    //bidouille avec la pondération pour les pondérations <0 ou >100 pour faire des boucles
    float truePonderation = float(ponderation)+ponderationDelta;
    if(truePonderation > 100 + ponderationDelta) truePonderation = 100 + ponderationDelta;
    if(truePonderation < 0-ponderationDelta) truePonderation = 0 - ponderationDelta;
    float difference = (float(ponderation) + ponderationDelta - truePonderation);
    if(x1>x2) difference = -difference;

    float a = truePonderation/100,
          b = (x2-x1)*a,
          c = qMin((float)DEFAULT_HEIGHT,(float)qAbs(y2-y1)),
          yctrl = ((1-a)*y1+a*y2);

    /// Points de contrôle
    QPointF ctrl1(x1, yctrl),
            ctrl2(x2, yctrl),
            ptInt1(x1,y1+c),
            ptInt4(x2, y2-c);

    /// Initialisation du chemin
    QPainterPath path;
    path.moveTo(x1, y1);

    // Default Value
    int linkTypeWithDefault = LINK_TYPE;
    // Overloaded value
    if(shapeType != 0) linkTypeWithDefault = shapeType - 1;

    QPointF finalPoint = endShape ? QPointF(x2 - 5*weight*qSin(qDegreesToRadians(alpha)), y2 - 5*weight*qCos(qDegreesToRadians(alpha))) : QPointF(x2, y2);

    /// Construction du chemin selon le type de lien
    switch(linkTypeWithDefault)
    {
    ///< Lien direct
    case MyShapes::StraightLine :
        path.lineTo(finalPoint);
        break;

    ///< Lien avec lignes horizontale et verticales
    case MyShapes::BrokenLine :
        if(y1<=y2)
        {
            path.lineTo(ctrl1);
            path.lineTo(ctrl2);
            path.lineTo(finalPoint);
        }
        else
        {
            b += difference;

            path.lineTo(ptInt1);
            path.lineTo(QPointF(x1+b, y1+c));
            path.lineTo(QPointF(x1+b, y2-c));
            path.lineTo(ptInt4);
            path.lineTo(finalPoint);
        }
        break;
    case MyShapes::CurvedLine :
        if(y1<=y2) path.cubicTo(ctrl1, ctrl2, finalPoint);
        else
        {
            b += difference;
            path.cubicTo(QPointF(x1,y1+c), QPointF(x1+b,y1+c), QPointF(x1+b,y1));
            path.lineTo(QPointF(x1+b, y2));
            path.cubicTo(QPointF(x1+b, y2-c), QPointF(x2, y2-c), finalPoint);
        }
        break;
    default:
        path.lineTo(finalPoint);
        break;
    }
    return path;

}


//----------------------------------------------------------------------------------------------------------------------------------
//    LinkTextGraphic
//----------------------------------------------------------------------------------------------------------------------------------

void LinkTextGraphic::paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *)
{
    if(DISPLAY_DDNAME)
    {
        QFont font;
        font.setPixelSize(DDNAME_FONT_SIZE);
        painter->setFont(font);

        QTextOption textOptions(QTextOption(Qt::AlignAbsolute | Qt::AlignHCenter | Qt::AlignVCenter));
        QLabel label("");
        label.setFont(font);
        label.setText(link->getDDName());

        int width = label.fontMetrics().boundingRect(label.text()).width()+10, height = label.fontMetrics().boundingRect(label.text()).height();

        resetPos();

        painter->setBrush(QColor(255,255,255,200));
        painter->setPen(Qt::transparent);
        painter->drawRect(QRectF(0, 0, width, height));

        painter->setPen(Qt::black);
        painter->drawText(QRectF(0, 0, width, height), link->getDDName(), textOptions);
    }
}

QRectF LinkTextGraphic::boundingRect() const
{
    QLabel label("");
    QFont font;
    font.setPixelSize(DDNAME_FONT_SIZE);
    label.setFont(font);
    label.setText(link->getDDName());
    int width = label.fontMetrics().boundingRect(label.text()).width()+10, height = label.fontMetrics().boundingRect(label.text()).height();
    return QRectF(0, 0, width, height).adjusted(-1,-1,1,1);
}

void LinkTextGraphic::resetPos()
{
    QFont font;
    font.setPixelSize(DDNAME_FONT_SIZE);

    float truePonderation = float(link->getPonderation());
    if(truePonderation > 100) truePonderation = 100;
    if(truePonderation < 0) truePonderation = 0;
    float difference = (float(link->getPonderation())-truePonderation);
    float a = truePonderation/100;
    float x1 = link->getInput()->pos().x() + link->getInput()->getWidth()/2,
          y1 = link->getInput()->pos().y() + link->getInput()->getHeight(),
          x2 = link->getOutput()->pos().x() + link->getOutput()->getWidth()/2,
          y2 = link->getOutput()->pos().y();
    if(x1>x2) difference =-difference;
    int shape = link->getShape();
    if(shape==1 || (!shape && LINK_TYPE==MyShapes::StraightLine)) a=0.5;

    QLabel label("");
    label.setFont(font);
    label.setText(link->getDDName());

    int width = label.fontMetrics().boundingRect(label.text()).width()+10, height = label.fontMetrics().boundingRect(label.text()).height();
    float xText = (x1 + x2)/2 - width/2;
    float yText = (1-a)*y1 + a*y2 - height/2;
    if(shape==2 || (!shape && LINK_TYPE==MyShapes::CurvedLine)) yText = (y1 + y2 + 6*((1-a)*y1 + a*y2))/8 - height/2;

    if(y1>=y2)
    {
        xText = (1-a)*x1 + a*x2 - width/2 + difference;
        yText = (y1+y2)/2 - height/2;
    }

    setPos(xText, yText);
}

void LinkTextGraphic::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    event->setPos(mapToScene(event->pos()));
    link->mouseMoveEvent(event);
}

//----------------------------------------------------------------------------------------------------------------------------------
//   LinkTextGraphic fin
//----------------------------------------------------------------------------------------------------------------------------------
