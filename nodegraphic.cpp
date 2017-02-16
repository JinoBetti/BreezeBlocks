#include "nodegraphic.h"
#include "customgraphics.h"
#include "mainwindow.h"
#include "anchorgraphic.h"
#include "modifydclfdialog.h"
#include "modifystepdialog.h"
#include "modifycustomnodedialog.h"
#include "modifyfunctiondialog.h"
#include "modifyconditiondialog.h"
#include "utils.h"
#include "reloaddetaildialog.h"

QString NodeGraphic::getAbsoluteDetailPath() const
{
    if (detailPath.isEmpty()) return detailPath;

    QFileInfo pathGiven(detailPath);
    QString finalPath = detailPath;
    if(pathGiven.isRelative())
    {
        QDir workspace(WORKSPACE);
        finalPath = workspace.cleanPath(workspace.absoluteFilePath(detailPath));
    }

    return finalPath;
}

void NodeGraphic::repositionSubComponents()
{
    inputAnchor->setPosition();
    outputAnchor->setPosition();
}

QPointF NodeGraphic::center() { return pos() + QPointF(width/2, height/2); }

NodeGraphic::NodeGraphic(float _x, float _y, QString _type, QString _comment, CustomGraphics *_graphics, bool _automatic):
    QGraphicsObject(0),
    automatic(_automatic),
    resizing(false),
    moving(false),
    branchId(-1),
    group(0),
    previousSelectedState(false),
    scanned(false),
    median(0),
    x(_x),
    y(_y),
    type(_type),
    comment(_comment),
    graphics(_graphics),
    contextMenu(new QMenu),
    clickPosition(QPointF(0,0))
{
    //qDebug()<<"NodeGraphic::NodeGraphic(float "<<_x<<", float _y, QString _type, QString _comment, MainWindow *_main):QGraphicsObject(0),x(_x),y(_y),type(_type),comment(_comment),main(_main)";
    //Affectation d'un identifiant utilisé pour les liens
    if(graphics)
    {
        if(graphics->getNodeGraphic(graphics->getIdCpt()) == 0) id = graphics->getIdCpt();
        else id = graphics->getMaxId()+1;
        graphics->incrementIdCpt();
    }

    //Ajout des ancres pour lier les noeuds entre eux
    inputAnchor = new AnchorGraphic(false, this);
    outputAnchor = new AnchorGraphic(true, this);
    inputAnchor->setParentItem(this);
    outputAnchor->setParentItem(this);
    if(!ANCHORS_DISPLAYED)
    {
        inputAnchor->hide();
        outputAnchor->hide();
    }

    //Création du menu contextuel
    linkToDetail = contextMenu->addAction("Ouvrir le schéma du détail");
    sendToBack = contextMenu->addAction("Envoyer à l'arrière plan");
    sendToFront = contextMenu->addAction("Envoyer au premier plan");
    editNode = contextMenu->addAction("Modifier le noeud");
    editColor = contextMenu->addAction("Modifier la couleur des noeuds sélectionnés");
    resetColor = contextMenu->addAction("Réinitialiser la couleur des noeuds sélectionnés");
    contextMenu->addSeparator();
    copyNode = contextMenu->addAction("Copier/Coller le noeud");
    copySelection = contextMenu->addAction("Copier/Coller la selection");
    sortSelection = contextMenu->addAction("Trier la séléction");
    splitNode = contextMenu->addAction("Séparer le noeud en plusieurs noeuds");
    makeCustom = contextMenu->addAction("Transformer en noeud personnalisable");
    contextMenu->addSeparator();
    deleteLinkToDetail = contextMenu->addAction("Supprimer le lien vers le détail");
    deleteLinks = contextMenu->addAction("Supprimer les liens");
    deleteNode = contextMenu->addAction("Supprimer le noeud");
    deleteSelection = contextMenu->addAction("Supprimer la sélection");

}

void NodeGraphic::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    qDebug()<<center();
    //qDebug()<<"NodeGraphic::mousePressEvent(QGraphicsSceneMouseEvent *"<<event->button()<<") Maj ? "<<(bool)(event->modifiers() & Qt::ShiftModifier)<<" Maj et branche ? "<<(bool)(event->modifiers() & Qt::ShiftModifier || SELECT_BRANCH);
    event->accept();

    // Sauvegarde des données avant modification
    widthSaved = width;
    heightSaved = height;
    QRect rect = QRect(width-10,height-10,15,15);

    //Redimensionnement au clic sur l'icone Fleche
    if(rect.contains(event->pos().toPoint()) && isSelected())
    {
        getGraphics()->saveTemporaryFile("Redimensionnement du noeud " + getName());
        resizing = true;
        return;
    }

    //menu contextuel au clic droit
    if(event->button()==Qt::RightButton && !(event->modifiers() & Qt::ShiftModifier))
    {
        sendToBack->setVisible(type=="CUSTOM");
        sendToFront->setVisible(type=="CUSTOM");
        makeCustom->setVisible(type!="CUSTOM");
        copySelection->setVisible(SELECTABLE_STATE);
        linkToDetail->setVisible(!detailPath.isEmpty());
        deleteLinkToDetail->setVisible(!detailPath.isEmpty());
        deleteSelection->setVisible(isSelected());
        copySelection->setVisible(isSelected());
        sortSelection->setVisible(isSelected());
        editNode->setVisible(isSelected());

        QAction* myAction = contextMenu->exec(event->screenPos());
        //qDebug()<<myAction;
        if(myAction == editNode) edit();
        else if(myAction == deleteNode)
        {
            graphics->saveTemporaryFile("Suppression du noeud " + getName());
            QString nameSaved = getName();
            graphics->deleteNode(this);
            graphics->update();
            graphics->getMain()->setStatus("Noeud "+nameSaved+" supprimé" );
        }
        else if (myAction == editColor)
        {
            graphics->modifyColorOfSelection();
        }
        else if (myAction == resetColor)
        {
            graphics->resetColorOfSelection();
        }
        else if(myAction == splitNode)
        {
            graphics->saveTemporaryFile("Séparation du noeud " + getName());
            graphics->splitNode(this);
            graphics->getMain()->setStatus("Noeud " + getName() + "séparé en plusieurs noeuds");
        }
        else if(myAction == deleteLinks)
        {
            graphics->saveTemporaryFile("Suppression des liens du noeud " + getName());
            graphics->deleteAssociatedLinks(this);
            graphics->getMain()->setStatus("Liens liés au noeud "+getName()+" supprimés");
        }
        else if(myAction == copyNode)
        {
            graphics->saveTemporaryFile("Copie du noeud " + getName());
            graphics->addNode(createCopy(graphics));
            graphics->update();
            graphics->getMain()->setStatus("Noeud "+getName()+" copié");
        }
        else if(myAction == copySelection)
        {
            //graphics->saveTemporaryFile(); => fait ailleurs
            graphics->copySelectedNodes();
            graphics->update();
        }
        else if(myAction == makeCustom)
        {
            graphics->makeCustom(this);
            //graphics->update();
        }
        else if(myAction == deleteSelection)
        {
            //graphics->saveTemporaryFile(); => fait ailleurs
            graphics->deleteSelectedNodes();
            graphics->update();
        }
        else if(myAction == linkToDetail)
        {
            QFileInfo fileInfo(getAbsoluteDetailPath());
            if(fileInfo.exists())
            {
                graphics->getMain()->load(fileInfo.filePath());
                graphics->getMain()->writeSettings(); // pour sauvegarde fichiers récents
            }
            else
            {
                ReloadDetailDialog relocateDialog(this, graphics);
                relocateDialog.exec();
            }
        }
        else if(myAction == deleteLinkToDetail)
        {
            graphics->saveTemporaryFile("Suppression du lien du noeud " + getName());
            setDetailPath("");
            graphics->getMain()->setStatus("Lien du noeud "+getName()+" supprimé");
        }
        else if(myAction == sendToBack)
        {
            graphics->saveTemporaryFile("Envoi du noeud " + getName() + " à l'arrière plan");
            setZValue(0);
            graphics->getMain()->setStatus("Noeud "+getName()+" envoyé à l'arrière plan");
        }
        else if(myAction == sendToFront)
        {
            graphics->saveTemporaryFile("Envoi du noeud " + getName() + " au premier plan");
            setZValue(10);
            graphics->getMain()->setStatus("Noeud "+getName()+" envoyé au premier plan");
        }
        else if(myAction == sortSelection) graphics->getMain()->reSortSelection();
        else event->ignore();
    }
    //sélection au clic gauche
    else if (event->button()==Qt::LeftButton)
    {
        clickPosition = mapToScene(event->pos())-pos();
        previousSelectedState = isSelected();
        // Si on clique sur un élément déselectionné on déselectionne les noeuds selectionnés auparavant
        if(!(event->modifiers() & Qt::ControlModifier) && SELECT_MODE && !isSelected()) getGraphics()->unselectNodes();

        // Si on active le déplacement par branche, on aligne les branches
        if((event->modifiers() & Qt::ShiftModifier || SELECT_BRANCH) && SELECTABLE_STATE) graphics->alignBranch(this, (event->modifiers() & Qt::ControlModifier));

        if((event->modifiers() & Qt::ControlModifier) && SELECTABLE_STATE && !SELECT_BRANCH) setSelected(!previousSelectedState);
    }
}

void NodeGraphic::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    //qDebug()<<"NodeGraphic::mouseReleaseEvent(QGraphicsSceneMouseEvent *"<<event->button()<<")";
    QGraphicsItem::mouseReleaseEvent(event);

    if(resizing)
    {
        resizing = false;
        getGraphics()->getMain()->setStatus("Noeud "+getName()+" redimensionné");
        return;
    }

    if(moving)
    {
        QVector<NodeGraphic*> nodes = graphics->getNodeGraphics();
        QPointF point = mapToScene(event->pos())-clickPosition + QPointF(getWidth()/2,getHeight()/2);
        ///TODO : pas prendre point.x() mais point.x() corrigé selon l'endroit ou on a cliqué dans la case

        setXPosNoMove(point.x()/ALIGN_GRID_X);
        setYPosNoMove(point.y()/ALIGN_GRID_Y);

        for(int i=0; i<nodes.size(); i++)
        {
            double newX = point.x() + nodes[i]->pos().x() - pos().x();
            double newY = point.y() + nodes[i]->pos().y() - pos().y();
            if(!nodes[i]->isSelected()) continue;

            nodes[i]->setXPosNoMove(newX/ALIGN_GRID_X);
            nodes[i]->setYPosNoMove(newY/ALIGN_GRID_Y);
        }
        moving = false;
        getGraphics()->getMain()->setStatus("Noeuds déplacés");
    }

    //En mode SELECT_BRANCH sans SELECT_MODE, on déselectionne la branche après l'avoir déplacée
    // ... sauf si on reste appuyé sur la touche ctrl
    if(SELECT_BRANCH && !SELECT_MODE && !(event->modifiers() & Qt::ControlModifier)) graphics->unselectNodes();

    // Si ctrl est pressé on ne déselectionne pas à la relache du clic
    if((event->modifiers() & Qt::ControlModifier)&& !SELECT_BRANCH) setSelected(!previousSelectedState);

    graphics->resizeScene();
}

void NodeGraphic::mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event)
{
    //qDebug()<<"void NodeGraphic::mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event)";
    event->accept();
    // On désactive l'édition des liens lorsque les touches MAJ ou CTRL sont appuyées
    if(event->button()==Qt::LeftButton && !(event->modifiers() & Qt::ShiftModifier || event->modifiers() & Qt::ControlModifier)) { edit(); }
}

void NodeGraphic::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    if(resizing)
    {
        prepareGeometryChange();
        QPointF diff = event->pos()-event->buttonDownPos(Qt::LeftButton);

        int rx = (int)(diff.x()/RESIZE_STEP)*RESIZE_STEP;
        int ry = (int)(diff.y()/RESIZE_STEP)*RESIZE_STEP;
        setHeight(heightSaved + ry);
        setWidth(widthSaved + rx);
    }
    else
    {
        if(!moving) getGraphics()->saveTemporaryFile("Déplacement de noeuds");
        QGraphicsItem::mouseMoveEvent(event);
        moving = true;
    }
}


int NodeGraphic::edit()
{
    //qDebug()<<"void NodeGraphic::edit()";
    if(type=="DCLF")
    {
        ModifyDCLFDialog modifyDCLFWindow(this, graphics);
        return modifyDCLFWindow.exec();
    }
    else if(type=="STEP")
    {
        ModifyStepDialog modifyStepWindow(this, graphics);
        return modifyStepWindow.exec();
    }
    else if(type=="CUSTOM")
    {
        ModifyCustomNodeDialog modifyNodeWindow(this, graphics);
        return modifyNodeWindow.exec();
    }
    else if(type=="FUNCTION")
    {
        ModifyFunctionDialog modifyFunctionWindow(this, graphics);
        return modifyFunctionWindow.exec();
    }
    else if(type=="CONDITION")
    {
        ModifyConditionDialog modifyConditionWindow(this, graphics);
        return modifyConditionWindow.exec();
    }
    else return 0;
}

NodeGraphic::~NodeGraphic()
{
    // Pas de delete des item enfants, supprimés implicitement à la suppression du noeud
    // Pas de delete des actions qui sont déjà supprimées en supprimant le menu
    delete contextMenu;
}

QColor NodeGraphic::getDefaultColor()
{
    if(type=="STEP" && ((StepGraphic*)(this))->getIsTechnical()) return TECHNICAL_STEP_COLOR;
    if(type=="STEP") return NORMAL_STEP_COLOR;
    if(type=="DCLF") return DCLFGraphic::getColorFromRules(static_cast<DCLFGraphic*>(this)->getClass());
    return Qt::white;
}

void NodeGraphic::setGraphics(CustomGraphics *_graphics)
{
    graphics =_graphics;
    graphics->addToScene(getInputAnchor());
    graphics->addToScene(getOutputAnchor());
    setFlag(QGraphicsItem::ItemIsSelectable, SELECTABLE_STATE);
    if(getSysinGraphic()) graphics->addToScene(getSysinGraphic());
    if(getCondition()) graphics->addToScene(getCondition());
}

void NodeGraphic::setXPos(float _x)
{
    x=_x;
    setX(x*ALIGN_GRID_X - getWidth()/2);
}

void NodeGraphic::setYPos(float _y)
{
    y=_y;
    setY(y*ALIGN_GRID_Y - getHeight()/2);
}

void NodeGraphic::setHeight(int _height)
{
    if(_height >= 10 && (_height < 1000 || type=="CUSTOM"))
    {
        height = _height;
        repositionSubComponents();
    }
}

void NodeGraphic::setWidth(int _width)
{
    if(_width >= 10 && (_width < 1000 || type=="CUSTOM"))
    {
        width = _width;
        repositionSubComponents();
    }
}

void NodeGraphic::autoAdjustSize()
{
    QGraphicsTextItem * mockItem = new QGraphicsTextItem(getText());
    QFont font;
    font.setPixelSize(FONT_SIZE);
    if(getFontSize()) font.setPixelSize(getFontSize());
    mockItem->setFont(font);
    mockItem->adjustSize();

    int extraHeight = 0;
    if(type=="DCLF" || (type =="CUSTOM" && getShape()==MyShapes::Cylinder))
    {
        extraHeight = (int) ((qMax((int)mockItem->boundingRect().height() + 2* INSIDE_MARGIN, DEFAULT_HEIGHT))/4);
    }
    float newHeight = qMax((int)mockItem->boundingRect().height() + 2* INSIDE_MARGIN, DEFAULT_HEIGHT)+extraHeight,
           newWidth = qMax((int)mockItem->boundingRect().width()  + 2* INSIDE_MARGIN, DEFAULT_WIDTH);


    setPos(pos().x()+(getWidth()-newWidth)/2,pos().y()+(getHeight()-newHeight)/2);
    setHeight(newHeight);
    setWidth(newWidth);
}
