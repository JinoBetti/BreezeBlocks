#include "customgraphics.h"
#include <QApplication>
#include <QGraphicsItem>
#include <QPointF>
#include <QFileDialog>
#include <QLabel>
#include <QColorDialog>
#include "versionupdatedialog.h"
#include "utils.h"
#include "writexml.h"
#include "loadxml.h"
#include "splitdclfdialog.h"
#include "splitdclfwidget.h"
#include "pacbasetranslator.h"
#include "ortitranslator.h"
#include "searchdockwidget.h"
#include "variablevaluesdialog.h"

/*!
 *  \fn CustomGraphics::CustomGraphics( QWidget* _parent, MainWindow* _main)
 *  \brief Constructeur de la classe CustomGraphics
 *
 * Initialisation des variables
 * Ajout d'un titre vide obligatoire
 *
 * \param _parent : MainWindow dans laquelle apparaitra la zone graphique
 *
 */
CustomGraphics::CustomGraphics( QWidget* _parent):
    QGraphicsView(_parent),
    xmlLoaded(false),
    shiftPressedFirst(false),
    ctrlPressedFirst(false),
    savedSelectedState(false),
    main(dynamic_cast<MainWindow*>(_parent)),
    myScene(new CustomGraphicScene()),
    currentFilename(""),
    modified(false),
    firstClickedAnchor(0),
    nbClickedAnchors(0),
    scaleFactor(1),
    idCpt(0),
    nbOfGroups(0)
{
    setContextMenuPolicy(Qt::CustomContextMenu);
    setScene(myScene);

    setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);

    setSceneRect(QRect(-100,-100,600,300));
    setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform);

    title = new TitleGraphic("Titre vide a modifier", this);
    myScene->addItem(title);

    setAcceptDrops(true);

    setStyleSheet(QString::fromUtf8("QScrollBar:vertical { "
        "    border: 1px solid #e3e3e3;"
        "    background:white;"
        "    width:15px;    "
        "    margin: 0px 0px 0px 0px;"
        "}"
        "QScrollBar::handle:vertical {"
        "    background: qlineargradient(x1:0, y1:0, x2:1, y2:0,"
        "    stop: 0  rgb(172, 172, 172), stop: 0.5 rgb(212, 212, 212),  stop:1 rgb(232, 232, 232));"
        "    min-height: 0px;"
        ""
        "}"
        "QScrollBar::add-line:vertical {"
        "    background: qlineargradient(x1:0, y1:0, x2:1, y2:0,"
        "    stop: 0  rgb(172, 172, 172), stop: 0.5 rgb(212, 212, 212),  stop:1 rgb(232, 232, 232));"
        "    height: 0px;"
        "    subcontrol-position: bottom;"
        "    subcontrol-origin: margin;"
        "}"
        "QScrollBar::sub-line:vertical {"
        "    background: qlineargradient(x1:0, y1:0, x2:1, y2:0,"
        "    stop: 0  rgb(172, 172, 172), stop: 0.5 rgb(212, 212, 212),  stop:1 rgb(232, 232, 232));"
        "    height: 0px;"
        "    subcontrol-position: top;"
        "    subcontrol-origin: margin;"
        "}"
        "QScrollBar:horizontal { "
        "    border: 1px solid #e3e3e3;"
        "    background:white;"
        "    height:15px;    "
        "    margin: 0px 0px 0px 0px;"
        "}"
        "QScrollBar::handle:horizontal {"
        "    background: qlineargradient(x1:0, y1:0, x2:0, y2:1,"
        "    stop: 0  rgb(172, 172, 172), stop: 0.5 rgb(212, 212, 212),  stop:1 rgb(232, 232, 232));"
        "    min-width: 0px;"
        ""
        "}"
        "QScrollBar::add-line:horizontal {"
        "    background: qlineargradient(x1:0, y1:0, x2:0, y2:1,"
        "    stop: 0  rgb(172, 172, 172), stop: 0.5 rgb(212, 212, 212),  stop:1 rgb(232, 232, 232));"
        "    width: 0px;"
        "}"
        "QScrollBar::sub-line:horizontal {"
        "    background: qlineargradient(x1:0, y1:0, x2:0, y2:1,"
        "    stop: 0  rgb(172, 172, 172), stop: 0.5 rgb(212, 212, 212),  stop:1 rgb(232, 232, 232));"
        "    width: 0px;"
        "}"
        ""));
}

CustomGraphics::~CustomGraphics()
{
    //qDebug()<<"~CustomGraphics()"<<getTabName();
    delete title;
    //myScene->deleteLater(); => NON car delete auto de tous les composants dont les ancres etc, et double delete des sous composants
    foreach(LinkGraphic* link, linkGraphic) delete link;
    //qDebug()<<"~CustomGraphics()=>liens supprimés";
    foreach(NodeGraphic* node, nodeGraphic) delete node;
    //qDebug()<<"~CustomGraphics()=>Noeuds supprimés";
}

void CustomGraphics::dragEnterEvent(QDragEnterEvent *e){ if (e->mimeData()->hasUrls()) e->accept();}
void CustomGraphics::dragMoveEvent(QDragEnterEvent *e){ if (e->mimeData()->hasUrls()) e->accept();}

void CustomGraphics::dropEvent(QDropEvent *e){ main->dropEvent(e); }


/*!
 *  \fn CustomGraphics::wheelEvent(QWheelEvent *event)
 *  \brief Zoom à la rotation de la molette de la souris
 *
 *  Appel de la fonction CustomGraphics::zoomIn() ou CustomGraphics::zoomOut() selon le sens de rotation de la molette
 * \param event : Evenement de rotation de la molette
 *
 */
void CustomGraphics::wheelEvent(QWheelEvent *event)
{
    setTransformationAnchor(QGraphicsView::AnchorUnderMouse);

    if(event->delta() > 0) zoomIn();
    else zoomOut();
}

/*!
 *  \fn void CustomGraphics::zoomIn()
 *  \brief Zoom avant
 *
 */
void CustomGraphics::zoomIn()
{
    double scaleChange = SCALE_CHANGE;
    scale(scaleChange, scaleChange);
    scaleFactor*=scaleChange;
}

/*!
 *  \fn void CustomGraphics::zoomOut()
 *  \brief Zoom arrière
 *
 */
void CustomGraphics::zoomOut()
{
    double scaleChange = SCALE_CHANGE;
    scale(1.0 / scaleChange, 1.0 / scaleChange);
    scaleFactor*= 1.0 /scaleChange;
}

/*!
 *  \fn void CustomGraphics::keyPressEvent(QKeyEvent *event)
 *  \brief Actions associées aux modes de sélection avec appui sur Shift ou Ctrl
 *
 * \param event : événement d'appui sur une touche du clavier
 *
 */
void CustomGraphics::keyPressEvent(QKeyEvent *event)
{
    /// Permettre de bouger dans la vue avec les flèches
    QGraphicsView::keyPressEvent(event);

    /// Shift : inverser le mode selection par branche
    if(event->key()==Qt::Key_Shift && !(event->modifiers() & Qt::ControlModifier))
    {
        savedSelectedState = SELECTABLE_STATE;
        shiftPressedFirst = true;
        getMain()->branchSelection(!SELECT_BRANCH);
    }

    /// Ctrl : préparer la sélection multiple
    if(event->key()==Qt::Key_Control && !(event->modifiers() & Qt::ShiftModifier))
    {
        QList<QGraphicsItem*> items = scene()->items();
        savedSelectedState = SELECTABLE_STATE;
        SELECTABLE_STATE = true;
        ctrlPressedFirst = true;

        ///> Activer la sélectabilité de tous les noeuds
        for(int i=0; i<items.size(); i++)
        {
            items[i]->setFlag(QGraphicsItem::ItemIsSelectable, SELECTABLE_STATE);
        }
    }

    /// Alt : affichage temporaire des ancres
    if(event->key()==Qt::Key_Space && !event->isAutoRepeat()) displayAnchors(!ANCHORS_DISPLAYED, true);

}

/*!
 *  \fn void CustomGraphics::keyReleaseEvent(QKeyEvent *event)
 *  \brief Action associé au relachement
 *
 * \param event : évenement de relachement d'une touche du clavier
 *
 */
void CustomGraphics::keyReleaseEvent(QKeyEvent *event)
{
    QGraphicsView::keyReleaseEvent(event);
    /// Shift : Inversion de la sélection par branche
    if(event->key()==Qt::Key_Shift && shiftPressedFirst)
    {
        shiftPressedFirst = false;
        getMain()->branchSelection(!SELECT_BRANCH);
    }
    /// Ctrl : Arreter la sélection multiple
    if(event->key()==Qt::Key_Control && ctrlPressedFirst)
    {
        SELECTABLE_STATE = savedSelectedState;

        ctrlPressedFirst = false;

        ///> Remise de la sélectabilité des noeuds à son état d'origine
        foreach(QGraphicsItem* item, scene()->items()) item->setFlag(QGraphicsItem::ItemIsSelectable, SELECTABLE_STATE);

        ///> Déselection des noeuds si on n'est pas en mode sélection
        if(!SELECT_MODE)
        {
            foreach(QGraphicsItem* item, scene()->items()) item->setSelected(false);
        }
        setDragMode(savedSelectedState ? QGraphicsView::RubberBandDrag : QGraphicsView::ScrollHandDrag);
    }
    /// Alt : affichage temporaire des ancres

    if(event->key()==Qt::Key_Space && !event->isAutoRepeat()) displayAnchors(ANCHORS_DISPLAYED, true);
}

/*!
 *  \fn bool CustomGraphics::notDoneEtaging()
 *  \brief Test pour savoir si tous les noeuds on été placés verticalement
 *
 * \return bool : Renvoie true si le positionnement vertical n'est pas terminé, false s'il est terminé
 *
 */

QDate CustomGraphics::getModificationDate() const { return modificationDate; }
void CustomGraphics::setModificationDate(const QDate &value){ modificationDate = value; }

QTime CustomGraphics::getModificationTime() const { return modificationTime; }
void CustomGraphics::setModificationTime(const QTime &value) { modificationTime = value; }

QString CustomGraphics::getModificationUser() const { return modificationUser; }
void CustomGraphics::setModificationUser(const QString &value) { modificationUser = value; }

bool CustomGraphics::notDoneEtaging()
{
    foreach(NodeGraphic* node, nodeGraphic){ if(node->getYPos()==-1) return true; }
    return false;
}

/*!
 *  \fn int CustomGraphics::getMaxInputLevel(NodeGraphic* _item)
 *  \brief Recherche de la position verticale du noeud en entrée du noeud testé le plus bas
 *
 * \param _item : Noeud dont on veut connaitre la position verticale maximale des entrées
 * \return int : Renvoie la position verticale de la plus basse de toutes les entrées du noeud (-1 si une des entrées n'a pas encore été positionnée verticalement)
 *
 */
int CustomGraphics::getMaxInputLevel(NodeGraphic* _item)
{
    int maxlevel=-1;
    /// Parcours de tous les liens
    foreach(LinkGraphic* link, linkGraphic)
    {
        ///> Si le lien a pour sortie le noeud testé, on regarde la hauteur de l'entrée
        if(link->getOutput() == _item)
        {
            int level = int(link->getInput()->getYPos());
            ///>> si une entrée n'est pas positionnée, renvoie -1
            if(level == -1) return -1;

            ///>> si l'entrée testée est plus basse que toutes les autres on sauvegarde sa hauteur
            if (level > maxlevel) maxlevel = level;
        }
    }
    /// Renvoi de la plus grande hauteur trouvée parmi les entrées du noeud testé
    return maxlevel;
}

/*!
 *  \fn int CustomGraphics::getMinOutputLevel(NodeGraphic* _item, int _maxLevel)
 *  \brief Recherche de la position verticale du noeud en sortie du noeud testé le plus haut
 *
 * \param _item : Noeud dont on veut chercher la sortie la plus haute
 * \param _maxLevel : Niveau le plus bas utilisé jusqu'à présent pour initialisation du minimum
 * \return int : Renvoie la hauteur de la sortie la plus haute du noeud testé (ou -1 si une des sorties n'a pas encore été positionné verticalement)
 *
 */
int CustomGraphics::getMinOutputLevel(NodeGraphic* _item, int _maxLevel)
{
    int minLevel = _maxLevel;
    /// Parcours de tous les liens
    foreach(LinkGraphic* link, linkGraphic)
    {
        ///> Si le lien a pour entrée le noeud testé, on regarde la hauteur de la sortie
        if(link->getInput() == _item)
        {
            int level = int(link->getOutput()->getYPos());
            ///>> si une sortie n'est pas positionnée, renvoie -1
            if(level==-1) return -1;

            ///>> si la sortie testée est plus haute que toutes les autres on sauvegarde sa hauteur
            if (level < minLevel) minLevel = level;
        }
    }
    /// Renvoi de la plus faible hauteur trouvée parmi les sorties du noeud testé
    return minLevel;
}

/*!
 *  \fn int CustomGraphics::getMaxYLevel(int _group)
 *  \brief Renvoie la position verticale maximale pour un groupe de noeuds donné, ou pour l'ensemble des noeuds.
 *
 * \param _group : Identifiant du groupe à tester, 0 pour tous les groupes (_group=0 par défaut)
 * \return int : Renvoie la plus grande valeur de la position verticale parmi les noueds du groupes demandé (initialisée à -1)
 *
 */
int CustomGraphics::getMaxYLevel(int _group)
{
    int maxlevel=-1;

    foreach(NodeGraphic* node, nodeGraphic)
    {
        if(_group && node->getGroup()!=_group) continue;
        if(node->getYPos() > maxlevel) maxlevel = node->getYPos();
    }
    return maxlevel;
}

int CustomGraphics::getMaxXLevel(int _group)
{
    if(!_group) return 0;
    int maxlevel = 0;

    foreach(NodeGraphic* node, getNodesInGroup(_group)) { if (node->getXPos() > maxlevel) maxlevel = node->getXPos(); }

    return maxlevel+2;
}

/*!
 *  \fn DCLFGraphic* CustomGraphics::getDclfGraphic(QString _name) const
 *  \brief Recherche du dernier noeud DCLF ajouté avec un nom donné
 *
 * \param _name : Nom du noeud DCLF à chercher
 * \return DCLFGraphic* : Renvoie un pointeur vers la dernière DCLFGraphic avec le nom "_name" dans le vecteur de noeuds, 0 si absente
 *
 */
DCLFGraphic* CustomGraphics::getDclfGraphic(const QString &_name) const
{
    for(int i=nodeGraphic.size()-1; i>=0; i--)
    {
        if(nodeGraphic[i]->getName() == _name && nodeGraphic[i]->getType()=="DCLF") return (DCLFGraphic*)(nodeGraphic[i]);
    }
    return 0;
}

QVector<DCLFGraphic *> CustomGraphics::getDclfGraphics() const
{
    QVector<DCLFGraphic *> dclfGraphics;
    foreach(NodeGraphic* node, nodeGraphic) { if(node->getType()=="DCLF") dclfGraphics.push_back((DCLFGraphic*)(node)); }
    return dclfGraphics;
}

/*!
 *  \fn NodeGraphic* CustomGraphics::getNodeGraphic(int _id) const
 *  \brief Recherche de noeud par identifiant
 *
 * \param _id : Identifiant du noeud recherché
 * \return NodeGraphic* : Renvoie un pointeur vers le noeud avec l'identifiant demandé, 0 si absent
 *
 */
NodeGraphic* CustomGraphics::getNodeGraphic(int _id) const
{
    foreach(NodeGraphic* node, nodeGraphic) if(node->getId()==_id) return node;
    return 0;
}

/*!
 *  \fn int CustomGraphics::getInputLinksNb(QGraphicsItem* _item)
 *  \brief Recherche du nombre d'entrées d'un noeud
 *
 * \param _item : noeud dont on compte les entrées
 * \return int : Renvoie le nombre d'entrée du noeud "_item"
 */
int CustomGraphics::getInputLinksNb(QGraphicsItem* _item)
{
    int numberOfInputLinks = 0;
    foreach(LinkGraphic* link, linkGraphic) if(link->getOutput() == _item) numberOfInputLinks++;
    return numberOfInputLinks;
}

/*!
 *  \fn int CustomGraphics::getOutputLinksNb(QGraphicsItem* _item)
 *  \brief Recherche du nombre de sorties d'un noeud
 *
 * \param _item : Noeud dont on compte les sorties
 * \return int : Renvoie le nombre de sorties du noeud "_item"
 */
int CustomGraphics::getOutputLinksNb(QGraphicsItem* _item)
{
    int numberOfOutputLinks = 0;
    foreach(LinkGraphic* link, linkGraphic) if(link->getInput() == _item) numberOfOutputLinks++;
    return numberOfOutputLinks;
}

/*!
 *  \fn void CustomGraphics::setModified(bool _modified)
 *  \brief Mutateur de l'attribut "modified"
 *
 * \param _modified : Nouvelle valeur à donner à l'attribut
 *
 */
void CustomGraphics::setModified(bool _modified)
{
    modified = _modified;
    /// Renommage de la fenêtre pour ajout ou suppression de l'étoile
    main->setWindowTitle(renameWindow());
}

/*!
 *  \fn QString CustomGraphics::renameWindow()
 *  \brief Renommage de la fenêtre selon le chemin du fichier courant
 *
 * \return QString : Renvoie le nouveau nom à donner à la fenêtre
 *
 */
QString CustomGraphics::renameWindow()
{
    QString star = modified ? "*" : "";
    if(!currentFilename.isEmpty())return PROGRAM + " " + VERSION + " " + currentFilename + star;
    return BASE_NAME;
}

/*!
 *  \fn void CustomGraphics::cleanScene()
 *  \brief Supprime tout le contenu de la zone graphique
 *
 */
void CustomGraphics::cleanScene(bool _delete)
{
    /// Suppression des liens
    if(_delete) foreach(LinkGraphic* link, linkGraphic) delete link;
    linkGraphic.erase(linkGraphic.begin(), linkGraphic.end());

    /// Suppression des noeuds
    if(_delete) foreach(NodeGraphic* node, nodeGraphic) node->deleteLater();
    nodeGraphic.erase(nodeGraphic.begin(), nodeGraphic.end());
}

/*!
 *  \fn int CustomGraphics::getMaxId()
 *  \brief Recherche du plus grand des identifiants des noeuds
 *
 * \return int : Renvoie le plus grand des identifiants des noeuds de la scène (initialisé à 0)
 *
 */
int CustomGraphics::getMaxId()
{
    int maxId=0;
    foreach(NodeGraphic* node, nodeGraphic) { if(node->getId()>maxId) maxId = node->getId(); }
    return maxId;
}

void CustomGraphics::setTitle(const QString &_title)
{
    title->setTitle(_title);
    title->setVisible(!HIDE_TITLE);
}

/*!
 *  \fn void CustomGraphics::addNode(NodeGraphic *_node)
 *  \brief Ajout d'un noeud à la scène
 *
 * \param _node : Noeud à ajouter à la scène
 *
 */
void CustomGraphics::addNode(NodeGraphic *_node)
{
    nodeGraphic.push_back(_node);
    addToScene(_node);
}

/*!
 *  \fn void CustomGraphics::addLink(LinkGraphic *_link)
 *  \brief Ajout d'un lien à la scène
 *
 * \param _link : Lien à ajouter à la scène
 *
 */
void CustomGraphics::addLink(LinkGraphic *_link)
{
    linkGraphic.push_back(_link);
    addToScene(_link);
}

/*!
 *  \fn void CustomGraphics::translateToGraphics(Jobset _jobset)
 *  \brief Traduction du jobset en éléments graphiques
 *
 * \param _jobset : Jobset issu de l'analyse du PCL
 *
 */
void CustomGraphics::translateToGraphics(const Jobset &_jobset)
{
    qDebug()<<"MainWindow::translateToGraphics(Jobset)";

    setTabName(_jobset.getDDName());

   ///Retour si job absent

    if(!_jobset.getJobs().size())
    {
        QMessageBox message;
        message.setText("Aucun Job trouvé");
        message.setIcon(QMessageBox::Critical);
        message.exec();
        return;
    }

    /// Création des DCLF

    for(int i=0; i<_jobset.getDclf().size(); i++)
    {
        DCLF currentDCLF = _jobset.getDclf()[i];
        QString comment = currentDCLF.getComment() +
                (currentDCLF.getComment().isEmpty()? "" : "\n") +
                (currentDCLF.getDefinition().isEmpty() ? "" : currentDCLF.getDefinition());
        DCLFGraphic* newDclfGraphic = new DCLFGraphic(currentDCLF.getDDName(), currentDCLF.getClass(), comment, currentDCLF.getFormat(), "", currentDCLF.getLength().toInt(), this, true);
        addToScene(newDclfGraphic);
        nodeGraphic.push_back(newDclfGraphic);
    }

    /// Création des étapes et des liens

    for(int i=0; i<_jobset.getJobs().size();i++)
    {
        Job currentJob = _jobset.getJobs()[i];

        for(int j=0; j<currentJob.getSteps().size(); j++)
        {
            ///> Création des noeuds étape
            Step currentStep = currentJob.getSteps()[j];

            StepGraphic* newStepGraphic = new StepGraphic(currentStep.getDDName(), currentStep.getType(), currentStep.getProgram(), currentStep.getComment(), this, true);
            newStepGraphic->setCondition(currentStep.getCondition().getCondition());
            addToScene(newStepGraphic);
            nodeGraphic.push_back(newStepGraphic);

            ///> Création des liens spécifiques
            QString program = currentStep.getProgram();
            createLinksFromRules(program, currentStep, newStepGraphic);

            ///> Créate des liens classiques, et des sysins
            for(int k=0; k<currentStep.getData().size(); k++)
            {
                Data currentData = currentStep.getData()[k];
                ///>> Lien étape - fichier (PCL)
                if(currentData.getType()=="FILE")
                {
                    bool stepToDclf = true;
                    QString mode = currentData.getParameter("MODE").getValue();
                    if(mode=="I") stepToDclf = false;
                    if(mode=="N") continue;
                    if(getDclfGraphic(currentData.getParameter("NAME").getValue()))
                    {
                        LinkGraphic* link;
                        if(stepToDclf) link = new LinkGraphic(newStepGraphic, getDclfGraphic(currentData.getParameter("NAME").getValue()),currentData.getDDName(), this, true);
                        else link = new LinkGraphic(getDclfGraphic(currentData.getParameter("NAME").getValue()), newStepGraphic,currentData.getDDName(), this, true);
                        linkGraphic.push_back(link);
                        addToScene(link);
                    }

                }
                ///>> Lien étape - fichier (JCL)
                if(currentData.getType()=="FILEJCL")
                {
                    bool stepToDclf = true;
                    QString mode = currentData.getParameter("DISP").getValue();
                    if(mode.isEmpty()) continue;
                    if(mode.contains("SHR") || mode.contains("OLD")) stepToDclf = false;
                    QString name = currentData.getParameter("DSN").getValue();

                    if(getDclfGraphic(name))
                    {
                        LinkGraphic* link;
                        if(stepToDclf) link = new LinkGraphic(newStepGraphic, getDclfGraphic(name),currentData.getDDName(), this, true);
                        else link = new LinkGraphic(getDclfGraphic(name), newStepGraphic,currentData.getDDName(), this, true);
                        linkGraphic.push_back(link);
                        addToScene(link);
                    }

                }
                ///>> Datas
                if(currentData.getType()=="DATA" || currentData.getType()=="DATAJCL")
                {
                    QString dataContent = "=>" + currentData.getDDName() + "\n" + currentData.getDataContent();
                    newStepGraphic->setSysin(newStepGraphic->getSysinValue()+(newStepGraphic->getSysinValue().isEmpty()?"":"\n")+dataContent);
                }

                ///>> Lien étape - état
                if(currentData.getType()=="REPORT")
                {
                    QString value = currentData.getParameter("SYSOUT").getValue();
                    QRegExp reportParams("\\((.*),(.*),(.*)\\)");
                    if(value.contains(reportParams))
                    {
                        QStringList subparams = reportParams.capturedTexts();
                        LinkGraphic* link = new LinkGraphic(newStepGraphic, getDclfGraphic(subparams[2]), currentData.getDDName(), this, true);
                        linkGraphic.push_back(link);
                        addToScene(link);
                    }
                }
            }
        }
    }


    /// Initialisation du titre
    title->setTitle("Dessin de chaine du "+ _jobset.getSource() + " " + _jobset.getDDName());
    myScene->addItem(title);

    qDebug()<<"MainWindow::translateToGraphics() -> end";
}

/*!
 *  \fn void CustomGraphics::translateToGraphics(PacbaseProgram _program)
 *  \brief Traduction du programme en éléments graphiques
 *
 * \param _program : Programme issu de l'analyse du source PacBase
 *
 */
void CustomGraphics::translateToGraphics(const PacbaseProgram &_program)
{
    qDebug()<<"MainWindow::translateToGraphics(Pacbase) -> begin";

    setTabName(_program.getName());

    //Noeuds Working Storage, macros, structures de données...
    if(!_program.getWorkingStorage().isEmpty())
    {
        CustomNodeGraphic* workingNode = new CustomNodeGraphic("Working Storage Section", _program.getWorkingStorage(), MyShapes::RoundedRectangle, this, true);
        workingNode->setHeight(100);
        workingNode->setColorSaved(Qt::white);
        addToScene(workingNode);
        nodeGraphic.push_back(workingNode);
    }

    if(!_program.getDataStructures().isEmpty())
    {
        CustomNodeGraphic* dataNode = new CustomNodeGraphic("Structures de données", _program.getDataStructures(), MyShapes::RoundedRectangle, this, true);
        dataNode->setHeight(100);
        dataNode->setColorSaved(Qt::white);
        addToScene(dataNode);
        nodeGraphic.push_back(dataNode);
    }

    if(!_program.getMacroDefinitions().isEmpty())
    {
        CustomNodeGraphic* macroNode = new CustomNodeGraphic("Macros", _program.getMacroDefinitions(), MyShapes::RoundedRectangle, this, true);
        macroNode->setHeight(100);
        macroNode->setColorSaved(Qt::white);
        addToScene(macroNode);
        nodeGraphic.push_back(macroNode);
    }

    // Fonctions
    QVector<Function*> functions = _program.getFunctions();

    // Pour chaque fonction
    for(int i=0; i<functions.size(); i++)
    {
        //int nodesSoFar = nodeGraphic.size();
        QVector<SubFunction*> subfunctions = functions[i]->getSubfonctions();

        //> Pour chaque sous fonction on créé un noeud graphique qu'on associe à la sousfonction
        for(int j=0; j<subfunctions.size(); j++)
        {
            QString subcomment = subfunctions[j]->getFullText();
            QString precision = subfunctions[j]->getTitle();

            //>> Création d'un noeud Fonction
            FunctionGraphic* newSubNode = new FunctionGraphic("F" + functions[i]->getName() + subfunctions[j]->getName(),
                                                              QString::number(subfunctions[j]->getLevel()) +
                                                              subfunctions[j]->getType(), precision, "", subcomment, this, true);
            if(!newSubNode->getPrecision().isEmpty())
            {
                newSubNode->setWidth(newSubNode->getWidth()*1.4);
                newSubNode->setHeight(newSubNode->getHeight()*1.6);
            }

            //>> Si la fonction est "SUP" => affichage en rouge, sinon on reprend la couleur du noeud fonction
            QColor color = functions[i]->getDefaultColor();
            newSubNode->setColorSaved(color);
            newSubNode->setColor(color);
            if(subfunctions[j]->getIsSup())
            {
                newSubNode->setColorSaved(Qt::red);
                newSubNode->setColor(Qt::red);
            }

            addToScene(newSubNode);
            nodeGraphic.push_back(newSubNode);
            subfunctions[j]->setAssociatedNode(newSubNode);
        }

        //Noeud Fin de fonction
        FunctionGraphic* newEndNode = new FunctionGraphic("Fin fonction", "", "", "", "", this, true);
        newEndNode->setColorSaved(Qt::darkCyan);
        newEndNode->setColor(Qt::darkCyan);

        addToScene(newEndNode);
        nodeGraphic.push_back(newEndNode);

        //> Pour chaque sous fonction, on créé les liens avec les autres noeuds
        for(int j=0; j<subfunctions.size(); j++)
        {
            QVector<NodeGraphic*> finalNodes = createInstructionNodes(subfunctions[j],_program);
            //Rattachement d'une fonction de type BL ou EL ou une boucle
            if(subfunctions[j]->getType()=="BL" || subfunctions[j]->getType()=="EL" ||
               subfunctions[j]->getType()=="DO" || subfunctions[j]->getType()=="DW" || subfunctions[j]->getType()=="DU")
            {
                NodeGraphic* outputNode = 0;
                if(j+1>=subfunctions.size()) outputNode = newEndNode;
                else
                {
                    if(subfunctions[j+1]->getType()!="EL")
                    {
                        outputNode = subfunctions[j+1]->getAssociatedNode();
                    }
                    else
                    {
                        int k=j+1;
                        while(subfunctions[k]->getType()=="EL")
                        {
                            int elseLevel = subfunctions[k]->getLevel();
                            while(k+1<subfunctions.size() && subfunctions[k+1]->getLevel()>elseLevel) k++;
                            k++;
                            if(k>=subfunctions.size()) break;
                        }

                        if(k<subfunctions.size()) outputNode = subfunctions[k]->getAssociatedNode();
                        else outputNode = newEndNode;
                    }
                }
                createLinks(finalNodes,(functions[i]->getName()>"90" && outputNode->getLevel().left(2)=="10") ? newEndNode : outputNode, "");
            }
            //Rattachement d'une fonction de type CO
            else if(subfunctions[j]->getType()=="CO")
            {
                NodeGraphic* outputNode = 0;
                int ITLevel = 0;
                bool caseFound = false;
                for(int k=j+1; k<subfunctions.size();k++)
                {
                    // Si on ne monte pas dans les niveaux =>  il n'y a pas de cas
                    if(!ITLevel && subfunctions[j]->getLevel() >= subfunctions[k]->getLevel()) break;
                    // initialisation du level des cas
                    if(!ITLevel && subfunctions[k]->getType()=="IT") ITLevel = subfunctions[k]->getLevel();
                    //Si on redescend en dessous du niveau du IT du CO, on arrête de chercher les autres cas
                    if(subfunctions[k]->getLevel()<ITLevel) break;
                    // si on est sur un cas du case of on créé un lien e
                    if(subfunctions[k]->getLevel()==ITLevel && (subfunctions[k]->getType()=="IT"||subfunctions[k]->getType()=="EL"||subfunctions[k]->getType()=="BL"))
                    {
                        subfunctions[k]->setIsITOfCaseOf(true);
                        outputNode = subfunctions[k]->getAssociatedNode();

                        caseFound = true;
                        createLinks(finalNodes,(functions[i]->getName()>"90" && outputNode->getLevel().left(2)=="10") ? newEndNode : outputNode, subfunctions[k]->getType()+
                                    (subfunctions[k]->getType()=="IT" ? ":" : "")+subfunctions[k]->getConditions());
                    }
                }

                //Si aucun cas trouvé
                if(!caseFound)
                {
                    outputNode = newEndNode;
                    createLinks(finalNodes, outputNode, "");
                }
            }

            //Rattachement d'une fonction de type IT qui n'est pas un cas de case of
            else if(subfunctions[j]->getType()=="IT")
            {
                NodeGraphic *outputNodeIf=0, *outputNodeElse=0;
                // Lien IF -> on saute les blocs Else et les blocs qui sont des cas de case of
                if(j+1 >= subfunctions.size()) outputNodeIf = newEndNode;
                else
                {
                    int m=j+1;
                    while(subfunctions[m]->getType()=="EL" || subfunctions[m]->getIsITOfCaseOf())
                    {
                        int elseLevel = subfunctions[m]->getLevel();
                        while(m+1<subfunctions.size() && subfunctions[m+1]->getLevel()>elseLevel) m++;
                        m++;
                        if(m >= subfunctions.size()) break;
                    }

                    if(m<subfunctions.size()) outputNodeIf = subfunctions[m]->getAssociatedNode();
                    else outputNodeIf = newEndNode;
                }
                createLinks(finalNodes,(functions[i]->getName()>"90" && outputNodeIf->getLevel().left(2)=="10") ? newEndNode : outputNodeIf, "");

                // Lien ELSE => quoi qu'il arrive rattacher au prochain noeud de niveau inf. ou égal
                if(!subfunctions[j]->getIsITOfCaseOf())
                {
                    int k=j+1;
                    bool hasSubFunctions = false;
                    while(k+1<subfunctions.size() && subfunctions[k+1]->getLevel()>subfunctions[j]->getLevel())
                    {
                        k++;
                        hasSubFunctions = true;
                    }
                    if(hasSubFunctions) k++;

                    if(k<subfunctions.size()) outputNodeElse = subfunctions[k]->getAssociatedNode();
                    else outputNodeElse = newEndNode;
                    if(functions[i]->getName()>"90" && outputNodeElse->getLevel().left(2)=="10") outputNodeElse = newEndNode;

                    createLink(subfunctions[j]->getAssociatedNode(),outputNodeElse,"Else");

                }

            }
            /// \todo Faire le lien de la boucle
            if(subfunctions[j]->getType()=="DO" || subfunctions[j]->getType()=="DW" || subfunctions[j]->getType()=="DU")
            {

            }
        }

        /// On éclate le noeud final
        splitNode(newEndNode);
    }

    /// Initialisation du titre
    title->setTitle("Dessin de chaine du programme "+ _program.getName()+"\r\n"+_program.getDescription());

    qDebug()<<"MainWindow::translateToGraphics() -> end";
}


/*!
 *  \fn void CustomGraphics::translateToGraphics(PacbaseProgram _program)
 *  \brief Traduction du programme en éléments graphiques
 *
 * \param _program : Programme issu de l'analyse du source PacBase
 *
 */
void CustomGraphics::translateToGraphics(const OrtiFile &_ortiFile)
{
    qDebug()<<"MainWindow::translateToGraphics(Orti) -> begin";

    setTabName(_ortiFile.getArtefact());

    CustomNodeGraphic* capsuleNode = 0;

    // Création des jobs
    QList<OrtiJob*> jobs = _ortiFile.getJobs();
    foreach(OrtiJob* currentJob, jobs)
    {
        QString comment =
                "Application : " + currentJob->getApplication() +"\r\n"+
                "Groupe : " + currentJob->getGroup() +"\r\n"+
                "Planification : " + currentJob->getPlanification();
        CustomNodeGraphic* node = new CustomNodeGraphic(currentJob->getJobname(), comment, MyShapes::RoundedRectangle, this, true);
        currentJob->setNodeGraphicId(node->getId());

        bool internArtefact = (_ortiFile.getArtefact() == currentJob->getGroup()) || (_ortiFile.getArtefact() == currentJob->getJobname());
        bool isTheCapsule = (_ortiFile.getArtefact().remove(0,1) == currentJob->getJobname().remove(0,1) && currentJob->getJobname().length()==8);
        node->setColorSaved(internArtefact? QColor("#aaffff") : QColor("#ffff7f"));
        node->setColor(internArtefact? QColor("#aaffff") : QColor("#ffff7f") );

        if(!internArtefact)
        {
            node->setHeight(node->getHeight()*0.6);
            node->setWidth(node->getWidth()*0.7);
        }

        if(isTheCapsule && internArtefact)
        {
            capsuleNode = node;
            node->setColorSaved(Qt::white);
            node->setColor(Qt::white);
            node->setWidth(node->getWidth()*2);
            node->setZValue(0);
            node->setFontSize(30);
            node->setHzAlign(2);
            node->setVtAlign(2);
        }

        QFileInfo file(WORKSPACE + "\\" + currentJob->getJobname()+".ddc");
        if (file.exists() && file.isFile()) node->setDetailPath(currentJob->getJobname()+".ddc");

        addToScene(node);
        nodeGraphic.push_back(node);
    }

    // Création des ressources
    QList<OrtiResource*> resources = _ortiFile.getResources();
    foreach(OrtiResource* currentResource, resources)
    {
        if(currentResource->getDesactivated()) continue;
        CustomNodeGraphic* node = new CustomNodeGraphic(currentResource->getName(), "", MyShapes::Ellipse, this, true);
        currentResource->setNodeGraphicId(node->getId());

        node->setHeight(node->getHeight()*0.7);

        addToScene(node);
        nodeGraphic.push_back(node);
    }

    // Création des tops horaires
    QList<OrtiTopH*> topHs = _ortiFile.getTopHs();
    foreach(OrtiTopH* currentTopH, topHs)
    {
        CustomNodeGraphic* node = new CustomNodeGraphic(currentTopH->getName(), "", MyShapes::Ellipse, this, true);
        currentTopH->setNodeGraphicId(node->getId());

        node->setHeight(node->getHeight()*0.6);
        node->setWidth(node->getWidth()*0.7);

        addToScene(node);
        nodeGraphic.push_back(node);
    }


    //Création des liens
    qDebug()<<"Création des liens";
    QList<OrtiLink*> links = _ortiFile.getLinks();
    foreach(OrtiLink* currentLink, links)
    {

        if(currentLink->getLinkType()==Orti::Resource)
        {
            if(currentLink->getResource()->getDesactivated()) continue;
        }
        NodeGraphic* predecessor = getNodeGraphic(currentLink->getPredecessorNodeGraphicId());
        NodeGraphic* successor = getNodeGraphic(currentLink->getSuccessor()->getNodeGraphicId());

        createLink(predecessor, successor, currentLink->getType());
    }

    // Eclatement des noeuds ressources
    foreach(NodeGraphic* node, nodeGraphic)
    {
        if(node->getType()=="CUSTOM" && node->getShape() == MyShapes::Ellipse) splitNode(node);
        if(node->getType()=="CUSTOM" && node->getColorSaved() == QColor("#ffff7f")) splitNode(node);
    }

    // Initialisation du titre
    title->setTitle("Dessin d'ordonnancement au voisinage de "+ _ortiFile.getArtefact());

    if(capsuleNode)
    {
        foreach(NodeGraphic* node, nodeGraphic)
        {
            setGroups();
            node->setSelected(node->getGroup()==capsuleNode->getGroup());
        }

        QString capsuleText = capsuleNode->getText();
        reSort();
        cut();
        reSort();
        int width = getBoundaries()[2]+DEFAULT_WIDTH;
        int height = getBoundaries()[3]+2*DEFAULT_HEIGHT;
        float centerX = getBoundaries()[0] + getBoundaries()[2]/2;
        float centerY = getBoundaries()[1] + getBoundaries()[3]/2;

        paste();

        foreach (NodeGraphic* node, nodeGraphic)
        {
            if (!node->isSelected()) continue;
            if(node->getText()==capsuleText) capsuleNode = static_cast<CustomNodeGraphic*>(node);
        }

        capsuleNode->setWidth(width);
        capsuleNode->setHeight(height);

        foreach (LinkGraphic* link, linkGraphic)
        {
            if (link->getInput()!=capsuleNode) continue;
            link->getOutput()->setY(link->getOutput()->pos().y()+height);
        }
        translateSelectedNodes(centerX - capsuleNode->center().x(), centerY - capsuleNode->center().y());

        unselectNodes();

        align();
    }

    else reSort();

    qDebug()<<"MainWindow::translateToGraphics() -> end";
}


QVector<NodeGraphic*> CustomGraphics::createInstructionNodes(SubFunction* _subfunction, const PacbaseProgram &_program)
{
    QVector<Instruction*> instructions = _subfunction->getInstructions();
    QString lastCondition = "";
    QVector<NodeGraphic*> createdNodes;
    createdNodes.push_back(_subfunction->getAssociatedNode());

    bool previousIsJump = false;

    /// Création des noeuds pour chaque instruction représentée
    foreach(Instruction* instruction, instructions)
    {
        if(previousIsJump && !instruction->getLevel())
        {
            instruction->setLevel(99);
            instruction->setType("BL");
        }
        if(previousIsJump) previousIsJump = false;

        //>>> Si instruction conditionnée de niveau 99, création d'un noeud personnalisable
        if(instruction->getLevel() == 99 )
        {
            QString instText = QString::number(instruction->getLevel()) + instruction->getType()
                    +((instruction->getType()=="BL") ? "" : "\n") + _subfunction->getLevel99Condition(instruction,lastCondition);
            lastCondition = _subfunction->getLevel99Condition(instruction,lastCondition);
            if(instText.isEmpty()) instText = "99BL";

            QString instComment = _subfunction->getLevel99Text(instruction);
            CustomNodeGraphic* newInstNode = new CustomNodeGraphic(instText, instComment, MyShapes::Diamond, this, true);
            newInstNode->setColorSaved(Qt::white);
            newInstNode->autoAdjustSize();

            addToScene(newInstNode);
            nodeGraphic.push_back(newInstNode);
            createdNodes.push_back(newInstNode);
        }

        ///>>> Si intruction P (Perform), création d'un noeud fonction
        if(instruction->getOperation() == "P  ")
        {
            QString instComment;

            if(_program.getSubfunction(instruction->getOperandes().simplified().right(4))) instComment = _program.getSubfunction(instruction->getOperandes().simplified().right(4))->getFullText();
            else instComment = "SOUS FONCTION NON TROUVEE !";

            FunctionGraphic* newInstNode = new FunctionGraphic(instruction->getOperandes().simplified(),"" , "", "PERFORM", instComment, this, true);

            QString function = instruction->getOperandes().remove(0,1);
            QColor color = Qt::white;
            if(function<"8" && function>"20") color = Qt::green;
            if(function<"20") color = Qt::cyan;
            if(function>"90") color = Qt::yellow;
            newInstNode->setColorSaved(color);
            newInstNode->setColor(color);

            addToScene(newInstNode);
            nodeGraphic.push_back(newInstNode);
            createdNodes.push_back(newInstNode);
        }

        ///>>> Si instruction "CAL" (CALL), création d'un noeud fonction
        if(instruction->getOperation() == "CAL")
        {
            QString program = instruction->getOperandes().simplified();
            QString comment = "";
            QRegExp usingClause("^(.*)(USING.*)");
            if(usingClause.indexIn(program)!=-1)
            {
                QStringList smatch = usingClause.capturedTexts();
                program = smatch[1];
                comment = smatch[2];
            }

            FunctionGraphic* newInstNode = new FunctionGraphic(program, "", "", "CALL", comment, this, true);
            newInstNode->setColorSaved(Qt::lightGray);
            newInstNode->setColor(Qt::lightGray);

            addToScene(newInstNode);
            nodeGraphic.push_back(newInstNode);
            createdNodes.push_back(newInstNode);

        }

        if(instruction->getOperation() == "GDI"
        || instruction->getOperation() == "GT "
        || instruction->getOperation() == "GFT"
        ||(instruction->getOperation() == "COB" && instruction->getOperandes().startsWith("GO TO")))
        {
            QString program = instruction->getOperandes().simplified();
            QString callMethod = instruction->getOperation();

            if(program.isEmpty())
            {
                program = callMethod;
                callMethod = "";
            }

            FunctionGraphic* newInstNode = new FunctionGraphic(program, "", "", callMethod, "", this, true);
            newInstNode->setColorSaved(Qt::darkCyan);
            newInstNode->setColor(Qt::darkCyan);

            addToScene(newInstNode);
            nodeGraphic.push_back(newInstNode);
            createdNodes.push_back(newInstNode);

            previousIsJump = true;
        }

        /// Noeud Evaluate
        if(instruction->getOperation() == "EVA")
        {
            QString evaluatedEntity = instruction->getOperandes().simplified();

            CustomNodeGraphic* newInstNode = new CustomNodeGraphic("Evaluate " + evaluatedEntity, "", MyShapes::RoundedRectangle, this, true);
            newInstNode->setColorSaved(Qt::white);
            newInstNode->setColor(Qt::white);

            addToScene(newInstNode);
            nodeGraphic.push_back(newInstNode);
            createdNodes.push_back(newInstNode);

        }

        /// Noeud End Evaluate
        if(instruction->getOperation() == "EEV")
        {
            CustomNodeGraphic* newInstNode = new CustomNodeGraphic("End Evaluate", "", MyShapes::RoundedRectangle, this, true);
            newInstNode->setColorSaved(Qt::white);
            newInstNode->setColor(Qt::white);

            addToScene(newInstNode);
            nodeGraphic.push_back(newInstNode);
            createdNodes.push_back(newInstNode);

        }
    }

    // Création des liens
    QVector<NodeGraphic*> endNodes;

    NodeGraphic* last99IT = 0;
    bool inEvaluate = false;

    for(int i=0; i<createdNodes.size(); i++)
    {
        NodeGraphic* currentSubNode = createdNodes[i];
        NodeGraphic* nextNodeToBeLinked = 0;
        if(i<createdNodes.size()-1) nextNodeToBeLinked = createdNodes[i+1];

        //Lien else
        if(currentSubNode->getShape()==MyShapes::Diamond)
        {
            if(last99IT)
            {
                createLink(last99IT, currentSubNode, "Else", "");
                last99IT = 0;
            }
            if(currentSubNode->getProgram().contains("99IT")) last99IT = currentSubNode;
        }

        // Pas de lien si noeud renvoi
        if(currentSubNode->getType()=="FUNCTION" &&   ( currentSubNode->getName()=="GDI"
                                                     || currentSubNode->getName()=="GFT"
                                                     || currentSubNode->getCallMethod()=="COB"
                                                     || currentSubNode->getCallMethod()=="GT ")) continue;

        //lier les noeuds evaluate aux 99WH
        if(currentSubNode->getText().startsWith("Evaluate"))
        {
            inEvaluate = true;
            for(int j=i+1; j<createdNodes.size(); j++)
            {
                if(createdNodes[j]->getText().startsWith("End Evaluate")) break;
                if(createdNodes[j]->getText().startsWith("99WH")) createLink(currentSubNode, createdNodes[j], "");
            }
        }

        //lier les noeuds end evaluate aux 99WH
        if(currentSubNode->getText().startsWith("End Evaluate"))
        {
            inEvaluate = false;
            bool activate = true;

            for(int j=i-1; j>0; j--)
            {
                if(createdNodes[j]->getText().startsWith("Evaluate")) break;
                if(activate)
                {
                    createLink(createdNodes[j], currentSubNode, "");
                    activate = false;
                }
                if(createdNodes[j]->getText().startsWith("99WH")) activate = true;

            }
        }

        if(nextNodeToBeLinked && nextNodeToBeLinked->getProgram().contains("99EL"))
        {
            int n=i+2;
            while(n<createdNodes.size())
            {
                if(createdNodes[n]->getShape()==MyShapes::Diamond)
                {
                    nextNodeToBeLinked = createdNodes[n];
                    break;
                }
                n++;
                //si plus de noeuds à rattacher
                if(n==createdNodes.size()) nextNodeToBeLinked = 0;
            }
        }

        if(nextNodeToBeLinked && !nextNodeToBeLinked->getText().startsWith("99WH")
                && !nextNodeToBeLinked->getText().startsWith("End Evaluate")) createLink(currentSubNode, nextNodeToBeLinked, "");
        else if(!inEvaluate) endNodes.push_back(currentSubNode);

    }

    if(createdNodes.size() && !(( createdNodes.last()->getName()=="GDI"
    || createdNodes.last()->getName()=="GFT"
    || createdNodes.last()->getCallMethod()=="COB"
    || createdNodes.last()->getCallMethod()=="GT "))) endNodes.push_back(createdNodes.last());

    return endNodes;
}

bool CustomGraphics::createLink(NodeGraphic* _input, NodeGraphic* _output, QString _ddname, QString _alternativeDdname, bool _automatic)
{
    if(!linkExists(_input,_output))
    {
        LinkGraphic* link = new LinkGraphic(_input, _output, _ddname, this, _automatic);
        linkGraphic.push_back(link);
        addToScene(link);
        return true;
    }
    else
    {
        linkExists(_input,_output)->setDDName(_alternativeDdname);
        return false;
    }
}


void CustomGraphics::createLinks(QVector<NodeGraphic*> _inputs, NodeGraphic* _output, QString _ddname, QString _alternativeDdname, bool _automatic)
{
    foreach (NodeGraphic* _input, _inputs)
    {
        if(!linkExists(_input,_output))
        {
            LinkGraphic* link = new LinkGraphic(_input, _output, _ddname, this, _automatic);
            linkGraphic.push_back(link);
            addToScene(link);
        }
        else
        {
            linkExists(_input,_output)->setDDName(_alternativeDdname);
        }
    }
}

/*!
 *  \fn void CustomGraphics::setGroups()
 *  \brief Assignation d'un groupe à chacun des noeuds
 * \warning l'identifiant des groupes commence à 1, 0 correspondant à un noeud non assigné à un groupe
 *
 */
void CustomGraphics::setGroups()
{
    bool notDoneSettingGroups = true;
    setLinksScanned(false);

    NodeGraphic* initialNode = nodeGraphic[0];
    int currentGroup = 1;
    /// Tant que tous les noeuds ne sont pas assignés à un groupe
    while(notDoneSettingGroups)
    {
        notDoneSettingGroups = false;
        ///> Assignation récursive du groupe courant en suivant les liens
        recursiveSetGroup(initialNode, currentGroup);
        ///> Si on trouve encore un noeud non assigné, on recommence un nouveau groupe
        foreach(NodeGraphic* node, nodeGraphic)
        {
            if(node->getGroup()==0)
            {
                initialNode = node;
                notDoneSettingGroups = true;
                currentGroup++;
                break;
            }
        }
    }
    nbOfGroups = currentGroup;
}

/*!
 *  \fn void CustomGraphics::recursiveSetGroup(NodeGraphic* _node, int _group)
 *  \brief Assignation récursive d'un groupe à tous les noeuds accessibles par des liens
 *
 * \param _node : Noeud de départ à assigner au groupe "_group"
 * \param _group : Groupe à assigner au noeud "_node"
 *
 */
void CustomGraphics::recursiveSetGroup(NodeGraphic* _node, int _group)
{
    /// Assignation du noeud au groupe
    _node->setGroup(_group);
    /// Lecture des liens associés au noeud
    foreach(LinkGraphic* link, linkGraphic)
    {
        if(link->getInput()==_node || link->getOutput()==_node)
        {
            ///> Saut de l'appel récursif si l'assignation a déjà été faite pour sortir de la boucle
            if(link->getScanned()) continue;
            link->setScanned(true);

            ///> Appel récursif de la fonction sur le noeud relié au noeud de départ
            if(link->getInput()==_node) recursiveSetGroup(link->getOutput(), _group);
            else recursiveSetGroup(link->getInput(), _group);
        }
    }
}

/*!
 *  \fn int CustomGraphics::initRank()
 *  \brief Initialise la hauteur des noeuds en fonction des liens
 *
 * \return int : Hauteur la plus importante donnée à un noeud
 *
 */
int CustomGraphics::initRank()
{
    /// Initialisation de la hauteur à 0 pour tous les noeuds d'entrée de chaine
    foreach(NodeGraphic* node, nodeGraphic) { if(!getInputLinksNb(node)) node->setYPos(0); }

    // Compteur qui assure la sortie de la boucle : dans le pire des cas on fait nodeGraphic.size() itérations
    int cpt=0;
    int maxLevel = 0;

    /// Initialisation à N + 1 pour tous les autres noeuds, ou N est la hauteur du noeud d'entrée la plus importante
    while(notDoneEtaging() && cpt < nodeGraphic.size())
    {
        cpt++;
        foreach(NodeGraphic* node, nodeGraphic)
        {
            int level = getMaxInputLevel(node) + 1;
            if(level > maxLevel) maxLevel = level;
            if(level) node->setYPos(level);
        }
    }

    return maxLevel;
}

/*!
 *  \fn void CustomGraphics::findTightSubTree(NodeGraphic* _node)
 *  \brief Tope le sous arbre étroit d'un noeud en topant récursivement chaque noeud
 *
 *  Tous les noeuds atteignables par un chemin n'utilisant que des liens minimaux (composante verticale = 1) sont topés comme étant scannés
 * \param _node : Noeud de départ
 *
 */
void CustomGraphics::findTightSubTree(NodeGraphic* _node)
{
    if(_node->getScanned()) return;
    _node->setScanned(true);

    foreach(LinkGraphic* link, linkGraphic)
    {
        if(!link->getSlack())
        {
            if(link->getInput()==_node)
            {
                if(!link->getOutput()->getScanned()) link->setScanned(true);
                findTightSubTree(link->getOutput());
            }
            if(link->getOutput()==_node)
            {
                if(!link->getInput()->getScanned()) link->setScanned(true);
                findTightSubTree(link->getInput());
            }
        }
    }
}

/*!
 *  \fn void CustomGraphics::setNodesScanned(bool _value)
 *  \brief
 *
 * \param
 * \return
 *
 */
void CustomGraphics::setNodesScanned(bool _value) { foreach(NodeGraphic* node, nodeGraphic) node->setScanned(_value); }

/*!
 *  \fn void CustomGraphics::setLinksScanned(bool _value)
 *  \brief
 *
 * \param
 * \return
 *
 */
void CustomGraphics::setLinksScanned(bool _value) { foreach(LinkGraphic* link, linkGraphic) link->setScanned(_value); }

QVector<NodeGraphic*> CustomGraphics::getSelectedNodes()
{
    QVector<NodeGraphic*> selectedNodes;
    foreach(NodeGraphic* node, nodeGraphic) { if(node->isSelected()) selectedNodes.push_back(node); }
    return selectedNodes;
}

QVector<LinkGraphic*> CustomGraphics::getSelectedLinks()
{
    QVector<LinkGraphic*> selectedLinks;
    foreach(LinkGraphic* link, linkGraphic) { if(link->getInput()->isSelected() && link->getOutput()->isSelected()) selectedLinks.push_back(link); }
    return selectedLinks;
}

/*!
 *  \fn void CustomGraphics::tightenTree(int _levels)
 *  \brief
 *
 * \param
 * \return
 *
 */
void CustomGraphics::tightenTree()
{
    /// Pour chaque groupe prendre le premier noeud du groupe et appeler la fonction de scan
    for(int i=1; i<=nbOfGroups; i++)
    {
        QVector<NodeGraphic*> nodesInGroup = getNodesInGroup(i);
        int toppedNodesInGroup;

        do
        {
            toppedNodesInGroup = 0;
            setNodesScanned(false);

            /// Scanner le groupe pour trouver un arbre serré
            findTightSubTree(nodesInGroup[0]);

            /// Compter les noeuds de l'arbre serré
            for(int k=0; k<nodesInGroup.size();k++)
            {
                if(nodesInGroup[k]->getScanned()) toppedNodesInGroup++;
            }

            /// Si le groupe entier n'est pas serré, le reserrer
            if(nodesInGroup.size()!=toppedNodesInGroup)
            {
                /// On considère qu'on ne dépassera jamais 1000 étages lors de la génération d'un dessin de chaine de PCL
                int delta = 1000;
                /// Trouver le delta duquel on va bouger l'arbre serré : minimum des slacks non nulles (signé)
                for(int j=0; j<linkGraphic.size(); j++)
                {
                    if(((linkGraphic[j]->getInput()->getScanned()  && !linkGraphic[j]->getOutput()->getScanned())
                      ||(linkGraphic[j]->getOutput()->getScanned() && !linkGraphic[j]->getInput()->getScanned())))
                    {
                        if(qAbs(linkGraphic[j]->getSlack())<qAbs(delta) && linkGraphic[j]->getSlack())
                        {
                            delta = linkGraphic[j]->getSlack();
                            if(linkGraphic[j]->getOutput()->getScanned()) delta = -delta;
                        }
                    }
                }
                if(qAbs(delta)>=1000) break;

                /// Rapprocher tous les noeuds de l'arbre de la valeur du slack, +-
                for(int j=0; j<nodesInGroup.size(); j++)
                {
                    if(nodesInGroup[j]->getScanned()) nodesInGroup[j]->setYPos(nodesInGroup[j]->getYPos()+delta);
                }
            }
        }
        while(nodesInGroup.size()!=toppedNodesInGroup);

        /// Trouver la plus petite valeur de position verticale
        int miniY;
        bool first = true;
        for(int j=0; j<nodesInGroup.size(); j++)
        {
            if(first) miniY = nodesInGroup[j]->getYPos();
            first = false;
            if(nodesInGroup[j]->getYPos() < miniY) miniY = nodesInGroup[j]->getYPos();
        }

        /// Tout remettre en 0 et pas au dessus
        for(int j=0; j<nodesInGroup.size(); j++)
        {
            nodesInGroup[j]->setYPos(nodesInGroup[j]->getYPos()-miniY);
        }
    }
}

/*!
 *  \fn QVector<NodeGraphic*> CustomGraphics::getNodesInGroup(int _group)
 *  \brief
 *
 * \param
 * \return
 *
 */
QVector<NodeGraphic*> CustomGraphics::getNodesInGroup(int _group)
{
    QVector<NodeGraphic*> nodesInGroup;
    foreach(NodeGraphic* node, nodeGraphic)  { if(node->getGroup()==_group) nodesInGroup.push_back(node); }
    return nodesInGroup;
}

/*!
 *  \fn void CustomGraphics::computeCutValues()
 *  \brief
 *
 * \param
 * \return
 *
 */
void CustomGraphics::initCutValues()
{
    setLinksScanned(false);

    for(int i=1; i<=nbOfGroups; i++)
    {
        setNodesScanned(false);
        QVector<NodeGraphic*> nodesInGroup = getNodesInGroup(i);

        findTightSubTree(nodesInGroup[0]);

        for(int k=0; k<linkGraphic.size(); k++)
        {
            int cutValue = 0;
            //sauter les liens hors arbre et init
            if(!linkGraphic[k]->getScanned()) continue;
            setNodesScanned(false);

            //on coupe le lien
            linkGraphic[k]->setScanned(false);

            propagateTail(linkGraphic[k]->getOutput());

            //et on le remet
            linkGraphic[k]->setScanned(true);
            for(int j=0; j<linkGraphic.size(); j++)
            {
                if(linkGraphic[j]->getGroup()==i)
                {
                    if(!linkGraphic[j]->getInput()->getScanned() && linkGraphic[j]->getOutput()->getScanned()) cutValue++;
                    if(linkGraphic[j]->getInput()->getScanned() && !linkGraphic[j]->getOutput()->getScanned()) cutValue--;
                }
            }
            linkGraphic[k]->setCutValue(cutValue);

        }
    }
}

/*!
 *  \fn void CustomGraphics::propagateTail(NodeGraphic* _node)
 *  \brief
 *
 * \param
 * \return
 *
 */
void CustomGraphics::propagateTail(NodeGraphic* _node)
{
    if(_node->getScanned()) return;
    _node->setScanned(true);

    for(int i=0; i<linkGraphic.size(); i++)
    {
        if(linkGraphic[i]->getScanned())
        {
            if(linkGraphic[i]->getInput()==_node) propagateTail(linkGraphic[i]->getOutput());
            if(linkGraphic[i]->getOutput()==_node) propagateTail(linkGraphic[i]->getInput());
        }
    }
}

/*!
 *  \fn void CustomGraphics::cutValueLoop()
 *  \brief
 *
 * \param
 * \return
 *
 */
void CustomGraphics::cutValueLoop()
{
    for(int i=1; i<=nbOfGroups; i++)
    {
        LinkGraphic* leaveEdge = getLeaveEdge(i);
        int cpt = 0;
        while(leaveEdge != 0)
        {
            cpt++;
            exchangeEdges(i, leaveEdge);
            if(cpt>100) break;
        }
    }
}

LinkGraphic* CustomGraphics::getLeaveEdge(int _group)
{
    for(int i=0; i<linkGraphic.size(); i++)
    {
        if(linkGraphic[i]->getGroup()==_group
        && linkGraphic[i]->getCutValue() < 0
        && linkGraphic[i]->getScanned()) return linkGraphic[i];
    }
    return 0;
}

void CustomGraphics::exchangeEdges(int _group, LinkGraphic* _leaveEdge)
{
    LinkGraphic* enterEdge = 0;
    int minSlack = 1000;
    setNodesScanned(false);

    //on coupe le lien
    _leaveEdge->setScanned(false);

    propagateTail(_leaveEdge->getOutput());

    for(int i=0; i<linkGraphic.size(); i++)
    {
        if(linkGraphic[i]->getGroup()==_group)
        {
            if(linkGraphic[i]->getInput()->getScanned() && !linkGraphic[i]->getOutput()->getScanned())
            {
                if(linkGraphic[i]->getSlack()<minSlack)
                {
                    minSlack = linkGraphic[i]->getSlack();
                    enterEdge = linkGraphic[i];
                }
            }
        }
    }
    //et on remet l'autre
    enterEdge->setScanned(true);

    // repositionnement
    QVector<NodeGraphic*> nodesInGroup = getNodesInGroup(_group);
    for(int i=0; i<nodesInGroup.size(); i++)
    {
        if(nodesInGroup[i]->getScanned()) nodesInGroup[i]->setYPos(nodesInGroup[i]->getYPos() + minSlack);
    }

    // trouver la plus petite valeur de position verticale
    int miniY;
    bool first = true;
    for(int j=0; j<nodesInGroup.size(); j++)
    {
        if(first) miniY = nodesInGroup[j]->getYPos();
        first = false;
        if(nodesInGroup[j]->getYPos() < miniY) miniY = nodesInGroup[j]->getYPos();
    }

    //tout remettre en 0 et pas au dessus
    for(int j=0; j<nodesInGroup.size(); j++)
    {
        nodesInGroup[j]->setYPos(nodesInGroup[j]->getYPos()-miniY);
    }

    initCutValues();
}

void CustomGraphics::addVirtualNodes()
{
    int nbOfLinks = linkGraphic.size();
    for(int i=0; i<nbOfLinks; i++)
    {
        int slack = linkGraphic[i]->getSlack();
        if(!slack) continue;

        if(slack < 0)
        {
            qWarning()<<"Noeud bouclant détecté, slack ="<<slack;
            //deleteLink(linkGraphic[i]->getInput(), linkGraphic[i]->getOutput());
            continue;
        }
        NodeGraphic* in = linkGraphic[i]->getInput(), * out = 0;
        int group = in->getGroup();
        for(int j=0; j<linkGraphic[i]->getSlack(); j++)
        {
            CustomNodeGraphic* virtualNode = new CustomNodeGraphic("VIRTUAL","",MyShapes::Rectangle,this, true);
            out = virtualNode;
            virtualNode->setType("VIRTUAL");
            virtualNode->setYPos(linkGraphic[i]->getInput()->getYPos()+j+1);
            virtualNode->setGroup(group);
            LinkGraphic* newLink = new LinkGraphic(in, out, "",this, true);
            addNode(virtualNode);
            addLink(newLink);
            in = out;
        }
        out = linkGraphic[i]->getOutput();
        LinkGraphic* newLink = new LinkGraphic(in, out, "",this, true);
        addLink(newLink);

    }
}

void CustomGraphics::deleteVirtualNodes()
{
    for(int i=0; i<nodeGraphic.size(); i++)
    {
        if(nodeGraphic[i]->getType()=="VIRTUAL")
        {
            deleteNode(nodeGraphic[i]);
            i--;
        }
    }
}

/*!
 *  \fn void CustomGraphics::positionNodes()
 *  \brief
 *
 * \param
 * \return
 *
 */
void CustomGraphics::positionNodes()
{
    qDebug()<<"positionNodes()";

    if(!nodeGraphic.size()) return;

    /// Assigner un groupe aux noeuds en scannant les liens

    main->setStatus("Assignation des groupes");
    setGroups();
    main->getProgressBar()->setValue(20);

    /// Initialisation de la coordonnées y : 0 pour les noeuds d'entrée
    main->setStatus("Initialisation des rangs");
    initRank();

    /// Réduction de la longueur des liens pour éviter d'avoir trop de noeuds de rang nul
    main->setStatus("Reserrage de l'arbre");
    tightenTree();

    // Réorganisation des noeuds dans l'arbre pour les noeuds ayant plusieurs hauteurs acceptables
    //A priori pas besoin de faire la suite, tout est induit par le reste du traitement
    //initCutValues();
    //cutValueLoop();

    /// Ajout de noeuds virtuels sur les liens longs
    main->setStatus("Ajout des noeuds virtuels");
    qDebug()<<"addvirtualnodes()";
    addVirtualNodes();

    /// Assigner une coordonnée en x linéairement
    main->setStatus("Initialisation des positions en X");
    initXPosition();
    main->getProgressBar()->setValue(20);

    main->setStatus("Ordonnancement des noeuds en X");
    qDebug()<<"ordering();";
    ordering();

    main->setStatus("Assignation des coordonnées en X");
    qDebug()<<"xCoordinates()";
    xCoordinates();
    main->getProgressBar()->setValue(97);

    // Suppression des noeuds virtuels
    main->setStatus("Supression des noeuds virtuels");
    deleteVirtualNodes();

    qDebug()<<"positionNodes()<<fin";

}

void CustomGraphics::ordering()
{
    for(int i=0; i<24; i++)
    {
        main->setStatus("Ordonnancement des noeuds en X : "+QString::number(i)+"/24");
        wmedian(i);
        transpose();
        main->getProgressBar()->setValue(20+(i+1)*3);
    }
}

void CustomGraphics::wmedian(int _iteration)
{
    /// Pour chaque groupe
    for(int i=1; i<=nbOfGroups; i++)
    {
        /// Pour chaque rang
        for(int j=0; j<=getMaxYLevel(i); j++)
        {
            int currentRank = j;
            int adjacentRank = currentRank-1;

            if(_iteration % 2 == 1)
            {
                currentRank = getMaxYLevel(i) - j;
                adjacentRank = currentRank + 1;
            }

            QVector<NodeGraphic*> nodesInRank = getNodesInRank(currentRank, i);
            for(int k=0; k<nodesInRank.size(); k++)
            {
                nodesInRank[k]->setMedian(medianValue(nodesInRank[k], adjacentRank));
            }

            qSort(nodesInRank.begin(), nodesInRank.end(), &CustomGraphics::sortMedian);

            int currentPos = 0;
            for(int k=0; k<nodesInRank.size(); k++)
            {
                if(nodesInRank[k]->getMedian()!=-1)
                {
                    while(getNodeFromPos(float(currentPos)+getMaxXLevel(i-1),nodesInRank[k]->getYPos())
                          && getNodeFromPos(float(currentPos)+getMaxXLevel(i-1),nodesInRank[k]->getYPos())->getMedian() == -1)
                    {
                        currentPos++;
                    }

                    nodesInRank[k]->setXPos(float(currentPos)+getMaxXLevel(i-1));
                    currentPos++;
                }
            }
        }
    }
}

float CustomGraphics::medianValue(NodeGraphic* _node, int _adjacentRank)
{
    QVector<float> adjacentPositions;
    QVector<NodeGraphic*> adjacentNodes = getNodesInRank(_adjacentRank, _node->getGroup());
    for(int i=0; i<adjacentNodes.size(); i++)
    {
        if(linkedToEachOther(adjacentNodes[i],_node))
        {
            adjacentPositions.push_back(adjacentNodes[i]->getXPos());
        }
    }

    int m = adjacentPositions.size()/2;

    if(!adjacentPositions.size()) return -1.0;
    if(adjacentPositions.size()%2 == 1) return adjacentPositions[m];
    if(adjacentPositions.size() == 2) return (adjacentPositions[0]+adjacentPositions[1])/2;

    float left = adjacentPositions[m-1]-adjacentPositions[0],
          right = adjacentPositions[adjacentPositions.size()-1]-adjacentPositions[m];

    return (adjacentPositions[m-1]*right+adjacentPositions[m]*left)/(left+right);
}

bool CustomGraphics::linkedToEachOther(NodeGraphic* _n1, NodeGraphic* _n2)
{
    for(int i =0; i<linkGraphic.size(); i++)
    {
        if(linkGraphic[i]->getInput() == _n1 && linkGraphic[i]->getOutput() == _n2) return true;
        if(linkGraphic[i]->getInput() == _n2 && linkGraphic[i]->getOutput() == _n1) return true;
    }
    return false;
}

void CustomGraphics::transpose()
{
    for(int i=1; i<=nbOfGroups; i++)
    {
        bool improved = true;
        while(improved)
        {
            improved = false;
            for(int j=0; j<=getMaxYLevel(i);j++)
            {
                QVector<NodeGraphic*> nodesInRank = getNodesInRank(j,i);
                for(int k=0; k<nodesInRank.size()-1; k++)
                {
                    if(crossings(nodesInRank[k],nodesInRank[k+1])>crossings(nodesInRank[k+1],nodesInRank[k]))
                    {
                        improved = true;
                        float savedPos = nodesInRank[k]->getXPos();
                        nodesInRank[k]->setXPos(nodesInRank[k+1]->getXPos());
                        nodesInRank[k+1]->setXPos(savedPos);
                    }
                }
            }
        }
    }
}

int CustomGraphics::crossings(NodeGraphic* _nLeft, NodeGraphic* _nRight)
{
    int crossingCount = 0;
    bool exchanged = false;
    if(_nLeft->getXPos()>_nRight->getXPos())
    {
        exchanged = true;
        float savedPos = _nLeft->getXPos();
        _nLeft->setXPos(_nRight->getXPos());
        _nRight->setXPos(savedPos);
    }

    QVector<LinkGraphic*> linksInRank = getLinks(_nLeft->getYPos());
    for(int i=0; i<linksInRank.size(); i++)
    {
        for(int j=i+1; j<linksInRank.size(); j++)
        {
            if(linksInRank[i]->getInput()->getYPos()!=linksInRank[j]->getInput()->getYPos()) continue;

            float Ax1 = linksInRank[i]->getInput()->getXPos(),
                  Bx1 = linksInRank[j]->getInput()->getXPos(),
                  Ax2 = linksInRank[i]->getOutput()->getXPos(),
                  Bx2 = linksInRank[j]->getOutput()->getXPos();
            if((Ax1>Bx1&&Ax2<Bx2)||(Ax1<Bx1&&Ax2>Bx2))
            {
                crossingCount++;
            }
        }
    }

    if(exchanged)
    {
        float savedPos = _nLeft->getXPos();
        _nLeft->setXPos(_nRight->getXPos());
        _nRight->setXPos(savedPos);
    }
    return crossingCount;
}

QVector<NodeGraphic*> CustomGraphics::getNodesInRank(int _rank, int _group)
{
    QVector<NodeGraphic*> nodesInGroup = getNodesInGroup(_group), nodesInRank;
    for(int i=0; i<nodesInGroup.size(); i++)
    {
        if(nodesInGroup[i]->getYPos()==_rank) nodesInRank.push_back(nodesInGroup[i]);
    }
    qSort(nodesInRank.begin(),nodesInRank.end(), &CustomGraphics::sortX);
    return nodesInRank;
}

bool CustomGraphics::sortX( NodeGraphic* &_n1,  NodeGraphic* &_n2)
{
    return (_n1->getXPos() < _n2->getXPos());
}

bool CustomGraphics::sortMedian( NodeGraphic* &_n1,  NodeGraphic* &_n2)
{
    return (_n1->getMedian() < _n2->getMedian());
}

void CustomGraphics::xCoordinates()
{
    clearBranchIds();

    for(int i=1; i<=nbOfGroups; i++)
    {

        QVector<NodeGraphic*> nodesInGroup = getNodesInGroup(i);
        int nbofBranches = giveBranchIds(nodesInGroup);
        for(int j=0; j<nbofBranches; j++)
        {
            QVector<NodeGraphic*> nodesInBranch = getNodesInBranch(j,i);
            int maxX = -1;

            for(int k=0; k<nodesInBranch.size(); k++)
            {
                if(nodesInBranch[k]->getXPos()>maxX) maxX = nodesInBranch[k]->getXPos();
            }

            moveBranchTo(maxX, nodesInBranch);
        }
    }
}

void CustomGraphics::clearBranchIds(){ foreach(NodeGraphic* node, nodeGraphic) node->setBranchId(-1); }

int CustomGraphics::giveBranchIds(QVector<NodeGraphic*> _nodes)
{
    int currentId = 0;
    for(int i=0; i<_nodes.size();i++)
    {
        if(_nodes[i]->getBranchId()==-1)
        {
            _nodes[i]->setBranchId(currentId);
            NodeGraphic* previousNode = 0, *followingNode = 0;

            previousNode = getPreviousNode(_nodes[i],true);
            while(previousNode && outputCount(previousNode)==1)
            {
                previousNode->setBranchId(currentId);
                previousNode = getPreviousNode(previousNode,true);
            }

            followingNode = getFollowingNode(_nodes[i],true);
            while(followingNode && inputCount(followingNode)==1)
            {
                followingNode->setBranchId(currentId);
                followingNode = getFollowingNode(followingNode,true);
            }

            currentId++;
        }
    }

    return currentId;
}

QVector<NodeGraphic*> CustomGraphics::getNodesInBranch(int _branchId, int _group)
{
    QVector<NodeGraphic*> vector;
    foreach(NodeGraphic* node, nodeGraphic)  { if(node->getGroup()==_group && node->getBranchId()==_branchId) vector.push_back(node); }
    return vector;
}

void CustomGraphics::moveBranchTo(int _posX, QVector<NodeGraphic*> _nodesInBranch)
{
    if(!_nodesInBranch.size()) return;
    if(_posX-getMaxXLevel(_nodesInBranch[0]->getGroup()-1)>50) return;

    for(int i=0; i<_nodesInBranch.size(); i++)
    {
        if(_nodesInBranch[i]->getXPos()>_posX) _posX = _nodesInBranch[i]->getXPos();
    }

    for(int i=0; i<_nodesInBranch.size(); i++)
    {
        for(int j=_nodesInBranch[i]->getXPos()+1; j<=_posX; j++)
        {
            NodeGraphic* collisionNode = getNodeFromPos(j,_nodesInBranch[i]->getYPos());
            if(collisionNode)
            {
                if(collisionNode->getGroup()==_nodesInBranch[0]->getGroup())moveBranchTo(_posX+1, getNodesInBranch(collisionNode->getBranchId(),collisionNode->getGroup()));
                else
                {
                    for(int k=_nodesInBranch[0]->getGroup()+1; k<=nbOfGroups; k++)
                    {
                        QVector<NodeGraphic*> nodesInGroup = getNodesInGroup(k);
                        for(int m=0; m<nodesInGroup.size(); m++)
                        {
                            nodesInGroup[m]->setXPos(nodesInGroup[m]->getXPos()+1);
                        }
                    }
                }
            }
        }
        _nodesInBranch[i]->setXPos(_posX);
    }
}

/*!
 *  \fn void CustomGraphics::createCATSpecificLinks(QString _program, Step _step, StepGraphic *_stepGraphic)
 *  \brief
 *
 * \param
 * \return
 *
 */
void CustomGraphics::createLinksFromRules(const QString &_program, const Step &_step, StepGraphic *_stepGraphic)
{
    for(int i=0; i<PCL_RULES.size(); i++)
    {
        if(PCL_RULES[i].program == _program)
        {
            QString fileName = _step.getParameter(PCL_RULES[i].param).getSubparam(PCL_RULES[i].subparam).getValue();

            if(getDclfGraphic(fileName))
            {
                LinkGraphic* link = 0;
                if(PCL_RULES[i].outputFile) link = new LinkGraphic(_stepGraphic, getDclfGraphic(fileName), PCL_RULES[i].subparam, this, true);
                else link = new LinkGraphic(getDclfGraphic(fileName), _stepGraphic, PCL_RULES[i].subparam, this, true);
                linkGraphic.push_back(link);
                addToScene(link);
            }
        }
    }

}

/*!
 *  \fn void CustomGraphics::initXPosition()
 *  \brief
 *
 * \param
 * \return
 *
 */
void CustomGraphics::initXPosition()
{
    int previousGroupWidth = 0, previousGroupWidthSave = 0;
    setNodesScanned(false);
    for(int j=1; j<=nbOfGroups; j++)
    {
        QVector<int> xLevels(getMaxYLevel(j)+1,0);
        QVector<NodeGraphic*> nodeInGroup = getNodesInGroup(j);

        xLevels = depthFirstSearch(nodeInGroup[0], xLevels, previousGroupWidth);

        for(int i=0;i<xLevels.size();i++)
        {
            if(xLevels[i]+previousGroupWidth > previousGroupWidthSave) previousGroupWidthSave = xLevels[i] + previousGroupWidth;
        }
        previousGroupWidth = previousGroupWidthSave + 1;
    }
}

/*!
 *  \fn QVector<int> CustomGraphics::depthFirstSearch(NodeGraphic* _node, QVector<int> _xLevels, int _xOffset)
 *  \brief
 *
 * \param
 * \return
 *
 */
QVector<int> CustomGraphics::depthFirstSearch(NodeGraphic* _node, QVector<int> _xLevels, int _xOffset)
{
    if(_node->getScanned()) return _xLevels;
    _node->setScanned(true);

    _node->setXPos(_xLevels[_node->getYPos()]+_xOffset);
    _xLevels[_node->getYPos()]++;
    for(int i=0; i<linkGraphic.size(); i++)
    {
        if(linkGraphic[i]->getSlack()) continue;
        if(_node == linkGraphic[i]->getInput())_xLevels = depthFirstSearch(linkGraphic[i]->getOutput(), _xLevels, _xOffset);
        if(_node == linkGraphic[i]->getOutput())_xLevels = depthFirstSearch(linkGraphic[i]->getInput(), _xLevels, _xOffset);
    }

    return _xLevels;
}

/*!
 *  \fn void CustomGraphics::clearPositions()
 *  \brief
 *
 * \param
 * \return
 *
 */
void CustomGraphics::clearPositions()
{
    qDebug()<<"clearpositions()";
    foreach (NodeGraphic* node, nodeGraphic)
    {
        node->setXPos(-1);
        node->setYPos(-1);
        node->setGroup(0);
    }
}

/*!
 *  \fn
 *  \brief
 *
 * \param
 * \return
 *
 */
LinkGraphic* CustomGraphics::linkExists(NodeGraphic *_input, NodeGraphic *_output) const
{
    if(!_input || !_output) return 0;

    for(int i=0; i<linkGraphic.size(); i++)
    {
        if(linkGraphic[i]->getInput()==_input && linkGraphic[i]->getOutput()==_output) return linkGraphic[i];
    }

    return 0;
}
/*!
 *  \fn
 *  \brief
 *
 * \param
 * \return
 *
 */
LinkGraphic* CustomGraphics::linkExists(const QString &_dclfName, NodeGraphic *_step, bool _outputFile) const
{
    if(!_step) return 0;

    QVector<LinkGraphic*> links = linkGraphic;// getLinks(_step); => ne fonctionne pas pour une raison inconnue...

    foreach(LinkGraphic* link, links)
    {
        if(_outputFile && link->getOutput()->getType() == "DCLF" && link->getOutput()->getName() == _dclfName && link->getInput()==_step) return link;
        if(!_outputFile && link->getInput()->getType() == "DCLF" && link->getInput()->getName() == _dclfName && link->getOutput()==_step) return link;
    }

    return 0;
}

/*!
 *  \fn
 *  \brief
 *
 * \param
 * \return
 *
 */
void CustomGraphics::deleteLink(NodeGraphic* _input, NodeGraphic* _output)
{
    for(int i=0; i<linkGraphic.size(); i++)
    {
        if(linkGraphic[i]->getInput()==_input && linkGraphic[i]->getOutput()==_output)
        {
            delete linkGraphic[i];
            linkGraphic.erase(linkGraphic.begin()+i);
            return;
        }
    }
}

/*!
 *  \fn
 *  \brief
 *
 * \param
 * \return
 *
 */
void CustomGraphics::deleteNode(NodeGraphic* _node)
{
    deleteAssociatedLinks(_node);
    for(int i=0; i<nodeGraphic.size(); i++)
    {
        if(nodeGraphic[i]==_node)
        {
            nodeGraphic[i]->deleteLater();
            nodeGraphic.erase(nodeGraphic.begin()+i);
            return;
        }
    }
}

/*!
 *  \fn
 *  \brief
 *
 * \param
 * \return
 *
 */
void CustomGraphics::deleteAssociatedLinks(NodeGraphic* _node)
{
    int i=0;

    while(i<linkGraphic.size())
    {
        if(linkGraphic[i]->getInput() == _node || linkGraphic[i]->getOutput() == _node)
        {
            delete linkGraphic[i];
            linkGraphic.erase(linkGraphic.begin()+i);
        }
        else i++;
    }
}

/*!
 *  \fn
 *  \brief
 *
 * \param
 * \return
 *
 */
void CustomGraphics::setColorToDCLF(const QString &_name, const QColor &_color){ foreach(NodeGraphic* node, nodeGraphic) {  if(node->getType()=="DCLF" && node->getName() == _name) node->setColor(_color); } }

/*!
 *  \fn
 *  \brief
 *
 * \param
 * \return
 *
 */
void CustomGraphics::setColorToFunction(const QString &_name, const QColor &_color) { foreach(NodeGraphic* node, nodeGraphic) { if(node->getType()=="FUNCTION" && node->getName() == _name) node->setColor(_color); } }
void CustomGraphics::setColorToFunction(const QString _name) { foreach(NodeGraphic* node, nodeGraphic) { if(node->getType()=="FUNCTION"&& node->getName() == _name) node->setColor(node->getColorSaved()); } }

/*!
 *  \fn
 *  \brief
 *
 * \param
 * \return
 *
 */
void CustomGraphics::updateDCLF(const QString &_name, const QColor &_color, const QString &_class, const QString &_format, int _size)
{
    foreach(NodeGraphic* node, nodeGraphic)
    {
        if(node->getType()=="DCLF"&& node->getName() == _name)
        {
            DCLFGraphic* nodeAsDCLF = dynamic_cast<DCLFGraphic*>(node);
            if(_color.isValid())
            {
                nodeAsDCLF->setColorSaved(_color);
                nodeAsDCLF->setColor(_color);
            }

            nodeAsDCLF->setClass(_class);
            nodeAsDCLF->setFormat(_format);
            nodeAsDCLF->setLength(_size);
        }
    }
}

void CustomGraphics::updateDCLFColor(const QString &_name, const QColor &_color)
{
    foreach(NodeGraphic* node, nodeGraphic)
    {
        if(node->getType()=="DCLF"&& node->getName() == _name)
        {
            DCLFGraphic* nodeAsDCLF = dynamic_cast<DCLFGraphic*>(node);
            if(_color.isValid())
            {
                nodeAsDCLF->setColorSaved(_color);
                nodeAsDCLF->setColor(_color);
            }
        }
    }
}
void CustomGraphics::updateFunctions(const QString &_name, const QColor &_color)
{
    foreach(NodeGraphic* node, nodeGraphic)
    {
        if(node->getType()=="FUNCTION" && node->getName() == _name)
        {
            node->setColorSaved(_color);
            node->setColor(_color);
        }
    }
}

/*!
 *  \fn
 *  \brief
 *
 * \param
 * \return
 *
 */
void CustomGraphics::alignBranch(NodeGraphic *_node, bool _ctrlPressed)
{
    //qDebug()<<"CustomGraphics::alignBranch(NodeGraphic *"<<_node->getName()<<", bool "<<_ctrlPressed<<")";
    if(!SELECT_MODE && !_ctrlPressed) unselectNodes();

    _node->setSelected(true);

    NodeGraphic *previousNode = getPreviousNode(_node), *followingNode = getFollowingNode(_node);

    while(previousNode && outputCount(previousNode)==1)
    {
        float deltaX = _node->getWidth()/2-previousNode->getWidth()/2;
        previousNode->setPos(_node->pos().x()+deltaX,previousNode->pos().y());
        previousNode->setSelected(true);
        previousNode = getPreviousNode(previousNode);
    }

    while(followingNode && inputCount(followingNode)==1)
    {
        float deltaX = _node->getWidth()/2-followingNode->getWidth()/2;
        followingNode->setPos(_node->pos().x()+deltaX,followingNode->pos().y());
        followingNode->setSelected(true);
        followingNode = getFollowingNode(followingNode);
    }
}

/*!
 *  \fn
 *  \brief
 *
 * \param
 * \return
 *
 */
NodeGraphic* CustomGraphics::getPreviousNode(NodeGraphic *_node, bool _noNegativeSlack)
{
    //qDebug()<<"CustomGraphics::getPreviousNode(NodeGraphic *"<<_node->getName()<<")";
    NodeGraphic* previousNode = 0;
    bool found = false;
    foreach(LinkGraphic* link, linkGraphic)
    {
        if(link->getSlack() < 0 && _noNegativeSlack) continue;

        if(link->getOutput()==_node)
        {
            if(found) return 0;
            previousNode = link->getInput();
            found = true;
        }
    }
    return previousNode;
}

/*!
 *  \fn
 *  \brief
 *
 * \param
 * \return
 *
 */
NodeGraphic* CustomGraphics::getFollowingNode(NodeGraphic *_node, bool _noNegativeSlack)
{
    NodeGraphic* followingNode = 0;
    bool found = false;
    foreach(LinkGraphic* link, linkGraphic)
    {
        if(link->getSlack() < 0 && _noNegativeSlack) continue;
        if(link->getInput()==_node)
        {
            if(found) return 0;
            followingNode = link->getOutput();
            found = true;
        }
    }
    return followingNode;
}

/*!
 *  \fn
 *  \brief
 *
 * \param
 * \return
 *
 */
int CustomGraphics::outputCount(NodeGraphic *_node)
{
    int count = 0;
    foreach(LinkGraphic* link, linkGraphic) { if (link->getInput()==_node) count++; }
    return count;
}

/*!
 *  \fn
 *  \brief
 *
 * \param
 * \return
 *
 */
int CustomGraphics::inputCount(NodeGraphic *_node)
{
    int count = 0;
    foreach(LinkGraphic* link, linkGraphic) { if (link->getOutput()==_node) count++; }
    return count;
}

/*!
 *  \fn
 *  \brief
 *
 * \param
 * \return
 *
 */
QVector<LinkGraphic*> CustomGraphics::getLinks(NodeGraphic *_node)
{
    QVector<LinkGraphic*> links;
    foreach(LinkGraphic* link, linkGraphic) { if (link->getInput() == _node || link->getOutput() == _node) links.push_back(link); }
    return links;
}

/*!
 *  \fn
 *  \brief
 *
 * \param
 * \return
 *
 */
QVector<LinkGraphic*> CustomGraphics::getLinks(int _rank)
{
    QVector<LinkGraphic*> links;
    foreach(LinkGraphic* link, linkGraphic)
    {
        if(link->getSlack()) continue;
        if(link->getInput()->getYPos() == _rank || link->getOutput()->getYPos() == _rank) links.push_back(link);
    }
    return links;
}

/*!
 *  \fn
 *  \brief
 *
 * \param
 * \return
 *
 */
NodeGraphic *CustomGraphics::getNodeFromPos(float _x, float _y)
{
    foreach(NodeGraphic* node, nodeGraphic) { if (node->getXPos()==_x && node->getYPos()==_y) return node; }
    return 0;
}

/*!
 *  \fn
 *  \brief
 *
 * \param
 * \return
 *
 */
void CustomGraphics::switchPos(NodeGraphic *_node1, NodeGraphic *_node2)
{
    if(_node1->getId()==_node2->getId()) return;

    float x1 = _node1->getXPos(), y1 = _node1->getYPos(), x2=x1, y2=y1;

    if(_node2) x2 = _node2->getXPos(), y2 = _node2->getYPos();

    _node1->setXPos(x2);
    _node1->setYPos(y2);

    if(_node2)
    {
        _node2->setXPos(x1);
        _node2->setYPos(y1);
    }
}

/*!
 *  \fn
 *  \brief
 *
 * \param
 * \return
 *
 */
void CustomGraphics::splitNode(NodeGraphic *_node)
{
    QVector<LinkGraphic*> links = getLinks(_node);
    for(int i=1; i<links.size(); i++)
    {
        NodeGraphic* copy = _node->createCopy(this);
        copy->setPos(_node->pos().x() + i*COPY_OFFSET, _node->pos().y() + i*COPY_OFFSET);
        addNode(copy);
        if(links[i]->getInput()==_node) links[i]->setInput(copy);
        if(links[i]->getOutput()==_node) links[i]->setOutput(copy);
    }
}

/*!
 *  \fn
 *  \brief
 *
 * \param
 * \return
 *
 */
void CustomGraphics::deleteSelectedNodes()
{
    saveTemporaryFile("Suppression des noeuds selectionnés");

    for(int i=nodeGraphic.size()-1; i>=0; i--) { if(nodeGraphic[i]->isSelected()) deleteNode(nodeGraphic[i]); }

    getMain()->setStatus("Noeuds selectionnés supprimés");
    title->setPosition();
    update();
}

/*!
 *  \fn
 *  \brief
 *
 * \param
 * \return
 *
 */
void CustomGraphics::copySelectedNodes()
{
    QMap<int, int> copyMap;
    QVector<int> inputs, outputs;
    QVector<QString> ddNames;
    QVector<int> linkTypes, ponderations;
    saveTemporaryFile("Copie des noeuds selectionnés");

    for(int i=nodeGraphic.size()-1; i>=0; i--)
    {
        if(nodeGraphic[i]->isSelected())
        {
            foreach(LinkGraphic* link, getLinks(nodeGraphic[i]))
            {
                if(link->getInput()->isSelected() && link->getOutput()->isSelected())
                {
                    inputs.push_back(link->getInput()->getId());
                    outputs.push_back(link->getOutput()->getId());
                    ddNames.push_back(link->getDDName());
                    linkTypes.push_back(link->getShape());
                    ponderations.push_back(link->getPonderation());
                }
            }

            nodeGraphic[i]->setSelected(false);

            NodeGraphic *copy = nodeGraphic[i]->createCopy(this);
            copy->setSelected(true);
            copyMap[nodeGraphic[i]->getId()] = copy->getId();
            addNode(copy);
        }
    }

    for(int i=0; i<inputs.size(); i++)
    {
        LinkGraphic *newLink = new LinkGraphic(getNodeGraphic(copyMap[inputs[i]]), getNodeGraphic(copyMap[outputs[i]]), ddNames[i], this, false);
        newLink->setPonderation(ponderations[i]);
        newLink->setShape(linkTypes[i]);
        addLink(newLink);
    }
    getMain()->setStatus("Noeuds selectionnés copiés");
}

/*!
 *  \fn
 *  \brief
 *
 * \param
 * \return
 *
 */
void CustomGraphics::align()
{
    setModified(true);
    saveTemporaryFile("Alignement des noeuds sur la grille");

//    QVector<float> boundaries = getBoundaries();
//    translateNodes(-boundaries[0],-boundaries[1]);

    foreach(NodeGraphic* currentNode, nodeGraphic)
    {
        double xpos = double((currentNode->pos().x()+currentNode->getWidth()/2)/ALIGN_GRID_X);
        double ypos = double((currentNode->pos().y()+currentNode->getHeight()/2)/ALIGN_GRID_Y);
        float xposRounded = floor(ALIGN_GRID_X_DIVISION * xpos + 0.5)/ALIGN_GRID_X_DIVISION;
        float yposRounded = floor(ALIGN_GRID_Y_DIVISION * ypos + 0.5)/ALIGN_GRID_Y_DIVISION;
        currentNode->setXPos(xposRounded);
        currentNode->setYPos(yposRounded);
    }
    resizeScene();
    title->setPosition();
//    update();
    main->setStatus("Composants alignés sur la grille");
}

/*!
 *  \fn
 *  \brief
 *
 * \param
 * \return
 *
 */
void CustomGraphics::copy()
{
    main->clearClipboard();
    int n=0;

    foreach(NodeGraphic* currentNode, nodeGraphic)
    {
        if(currentNode->isSelected())
        {
            n++;
            NodeGraphic* copyNode = currentNode->createCopy(nullptr);
            copyNode->setId(currentNode->getId());
            main->addToClipboard(copyNode);
        }
    }
    foreach(LinkGraphic* link, linkGraphic)
    {
        if(link->getInput()->isSelected() && link->getOutput()->isSelected())
        {
            LinkGraphic* copyLink = new LinkGraphic(main->getClipboardNode(link->getInput()->getId()),main->getClipboardNode(link->getOutput()->getId()),link->getDDName(), nullptr, false);
            copyLink->setPonderation(link->getPonderation());
            copyLink->setPenStyle(link->getPenStyle());
            copyLink->setEndShape(link->getEndShape());

            main->addToClipboard(copyLink);
        }
    }

    if(n)
    {
        main->setStatus(QString::number(n) + " noeuds copiés dans le presse papier");
        main->setPasteEnabled(true);
    }
    else main->setStatus("Presse papier vidé");
}

/*!
 *  \fn
 *  \brief
 *
 * \param
 * \return
 *
 */
void CustomGraphics::cut()
{
    copy();
    deleteSelectedNodes();
}

/*!
 *  \fn
 *  \brief
 *
 * \param
 * \return
 *
 */
void CustomGraphics::duplicate()
{
    copy();
    unselectNodes();
    paste();
}

/*!
 * \fn
 * \brief
 *
 * \param
 * \return
 *
 */
void CustomGraphics::unselectNodes()
{
    for(int i=0; i<nodeGraphic.size(); i++)
    {
        nodeGraphic[i]->setSelected(false);
    }
}


/*!
 *  \fn
 *  \brief
 *
 * \param
 * \return
 *
 */
void CustomGraphics::paste()
{
    if(!main->getClipboardNodes().size()) return;

    // tout déséléctionner et passer en mode selection
    unselectNodes();
    main->setSelectMode(true);

    setModified(true);
    saveTemporaryFile("Collage du contenu du presse papier");

    /// Récupération des coordonnées du noeud le plus en haut et le plus à gauche
    QPointF center = mapToScene(this->rect().center());
    float xCible = center.x(), yCible = center.y();
    float xMin = 0, yMin = 0;
    for(int i=0; i<main->getClipboardNodes().size(); i++)
    {
        if(i==0)
        {
            xMin = main->getClipboardNodes()[i]->pos().x();
            yMin = main->getClipboardNodes()[i]->pos().y();
        }
        else
        {
            if(main->getClipboardNodes()[i]->pos().x() < xMin) xMin = main->getClipboardNodes()[i]->pos().x();
            if(main->getClipboardNodes()[i]->pos().y() < yMin) yMin = main->getClipboardNodes()[i]->pos().y();
        }
    }

    float deltaX = xCible - xMin, deltaY = yCible - yMin;
    QMap<int, int> copyMap;
    QVector<int> inputs, outputs;
    QVector<QString> ddNames;
    QVector<int> linkTypes, ponderations, endShapes;

    QVector<Qt::PenStyle> penStyles;
    /// Copie des noeuds
    for(int i=main->getClipboardNodes().size()-1; i>=0; i--)
    {
        NodeGraphic *copy = main->getClipboardNodes()[i]->createCopy(this);
        copy->setSelected(true);
        copy->setPos(copy->pos().x()+deltaX, copy->pos().y()+deltaY);
        copyMap[main->getClipboardNodes()[i]->getId()] = copy->getId();
        addNode(copy);
    }

    /// Copie des liens
    QVector<LinkGraphic*> links = main->getClipboardLinks();

    for(int j=0; j<links.size(); j++)
    {
        inputs.push_back(links[j]->getInput()->getId());
        outputs.push_back(links[j]->getOutput()->getId());
        ddNames.push_back(links[j]->getDDName());
        linkTypes.push_back(links[j]->getShape());
        ponderations.push_back(links[j]->getPonderation());
        penStyles.push_back(links[j]->getPenStyle());
        endShapes.push_back(links[j]->getEndShape());
    }

    for(int i=0; i<inputs.size(); i++)
    {
        LinkGraphic *newLink = new LinkGraphic(getNodeGraphic(copyMap[inputs[i]]), getNodeGraphic(copyMap[outputs[i]]), ddNames[i], this, false);
        newLink->setPonderation(ponderations[i]);
        newLink->setShape(linkTypes[i]);
        newLink->setPenStyle(penStyles[i]);
        newLink->setEndShape(endShapes[i]);
        addLink(newLink);
    }

    main->setStatus("Contenu du presse papier collé");
}

/*!
 *  \fn
 *  \brief
 *
 * \param
 * \return
 *
 */
void CustomGraphics::resizeScene()
{
    // initialisation
    if(!nodeGraphic.size()) return;
    int xmin = nodeGraphic[0]->pos().x(),
        ymin = nodeGraphic[0]->pos().y(),
        xmax = nodeGraphic[0]->pos().x() + nodeGraphic[0]->getWidth(),
        ymax = nodeGraphic[0]->pos().y() + nodeGraphic[0]->getHeight();

    // gérer les noeuds
    foreach(NodeGraphic* node, nodeGraphic)
    {
        xmin = qMin(xmin,(int) node->pos().x());
        ymin = qMin(ymin,(int) node->pos().y());
        xmax = qMax(xmax,(int) (node->pos().x() + node->getWidth()));
        ymax = qMax(ymax,(int) (node->pos().y() + node->getHeight()));
    }

    // gérer les liens boucles
    foreach(LinkGraphic* link, linkGraphic)
    {
        xmin = qMin(xmin, (int) link->boundingRect().x());
        xmax = qMax(xmax, (int) (link->boundingRect().x()+link->boundingRect().width()));
        ymin = qMin(ymin, (int) link->boundingRect().y());
        ymax = qMax(ymax, (int) (link->boundingRect().y()+link->boundingRect().height()));
    }

    // Gérer la taille du titre s'il est large
    if(!HIDE_TITLE)
    {
        xmin = qMin(xmin, (int) title->pos().x());
        ymin = qMin(ymin, (int) title->pos().y());
        xmax = qMax(xmax, (int)(title->pos().x() + title->getWidth()));
        ymax = qMax(ymax, (int)(title->pos().y() + title->getHeight()));
    }

    setSceneRect(QRect(xmin-SCENE_MARGIN, ymin-SCENE_MARGIN, xmax-xmin + 2*SCENE_MARGIN, ymax-ymin + 2*SCENE_MARGIN));
}

/*!
 *  \fn
 *  \brief
 *
 * \param
 * \return
 *
 */
/// sauvegarde un fichier temporaire tmp.ddc, et rajoute à la pile de sauvergarde le dessin de chaine en cours
/// sauvegarde également le chemin du dessin de chaine en cas de plantage
void CustomGraphics::saveTemporaryFile(const QString &_lastAction)
{
    redoSave.clear();
    main->setActivatedRedo(false);
    //qDebug()<<"Sauvegarde avant"<<_lastAction;
    WriteXML xmlDoc(this, QFileInfo(EXE_PATH, "tmp.ddc").absoluteFilePath(),true);

    int saveNumber=10;
    if(tmpSave.size()>saveNumber) tmpSave.erase(tmpSave.begin(),tmpSave.begin()+1);
    tmpSave.push(TemporarySave(xmlDoc.write(), _lastAction));

////    qDebug()<<"contenu sauvegardes: ";
////    foreach(TemporarySave temp, tmpSave) qDebug()<<temp.lastAction;

    // Sauvegarde du chemin, pour restaurer le fichir en cas le plantage
    QSettings settings(QFileInfo(EXE_PATH, "settings.ini").absoluteFilePath(),QSettings::IniFormat, this);
    RESTORE_PATH = currentFilename;
    settings.beginGroup("String");
    settings.setValue("RESTORE_PATH", RESTORE_PATH);
    settings.endGroup();

    main->setActivatedUndo(true);
    setModified(true);
    main->setFocusInAlertSent(false);
}


/*!
 *  \fn
 *  \brief
 *
 * \param
 * \return
 *
 */
void CustomGraphics::hideSysins(bool _hide)
{
    SYSIN_HIDDEN = _hide;
    for(int i=0; i<nodeGraphic.size(); i++)
    {
        // Seuls les noeuds steps ont des Sysin, pour les autres getSysinGraphic renvoie 0
        SysinGraphic* sysin = nodeGraphic[i]->getSysinGraphic();
        if(sysin) sysin->setVisible(!SYSIN_HIDDEN && !sysin->getSysin().simplified().isEmpty());
    }
}

/*!
 *  \fn
 *  \brief
 *
 * \param
 * \return
 *
 */
void CustomGraphics::hideConditions(bool _hide)
{
    CONDITIONS_DISPLAYED = !_hide;
    for(int i=0; i<nodeGraphic.size(); i++)
    {
        // Seuls les noeuds steps ont des ConditionGraphic, pour les autres getConditionGraphic renvoie 0
        ConditionGraphic* condition = nodeGraphic[i]->getCondition();
        if(condition) condition->setVisible(CONDITIONS_DISPLAYED && !condition->getCondition().simplified().isEmpty());
    }
}

/*!
 *  \fn
 *  \brief
 *
 * \param
 * \return
 *
 */
void CustomGraphics::hideTitle(bool _toggled)
{
    //qDebug()<<"hideTitle("<<_toggled<<")";
    HIDE_TITLE = _toggled;
    title->setVisible(!HIDE_TITLE);
    resizeScene();
    updateScene();
}

/*!
 *  \fn
 *  \brief
 *
 * \param
 * \return
 *
 */
void CustomGraphics::hideLinkedSchematics(bool _toggled)
{
    DISPLAY_SCHEMATICS = !_toggled;
    updateScene();
}


/*!
 *  \fn
 *  \brief
 *
 * \param
 * \return
 *
 */
void CustomGraphics::hideClass(bool _toggled)
{
    DISPLAY_DCLFCLASS = !_toggled;
    updateScene();
}

/*!
 *  \fn
 *  \brief
 *
 * \param
 * \return
 *
 */
void CustomGraphics::hideStepName(bool _toggled)
{
    DISPLAY_STEPNAME = !_toggled;
    updateScene();
}

void CustomGraphics::updateScene()
{
    //qDebug()<<"scene update "<<QDateTime::currentDateTime();
    myScene->update();
}

void CustomGraphics::makeCustom(NodeGraphic *_node)
{
    if(_node->getType()=="CUSTOM") return;
    saveTemporaryFile("Transformation d'un noeud en noeud personnalisable");

    QString text=_node->getText(), comment = _node->getComment();
    CustomNodeGraphic* replacementNode = new CustomNodeGraphic(text, comment, MyShapes::Rectangle, this, false);
    replacementNode->setColor(_node->getColorSaved());
    replacementNode->setColorSaved(_node->getColorSaved());
    replacementNode->setWidth(_node->getWidth());
    replacementNode->setHeight(_node->getHeight());
    replacementNode->setPos(_node->pos());
    replacementNode->setHasShadow(true);
    replacementNode->setDetailPath(_node->getDetailPath());

    if(_node->getType()=="DCLF")
    {
        if(!_node->getFormat().isEmpty()) comment += "\nFormat du fichier : " + _node->getFormat();
        if(_node->getLength()) comment += "\nLongueur du fichier : " + QString::number(_node->getLength());
        replacementNode->setShape(MyShapes::Cylinder);
    }
    if(_node->getType()=="STEP")
    {
        if(_node->getIsTechnical()) replacementNode->setShape(MyShapes::Diamond);
        if(!_node->getSysinValue().isEmpty()) comment += "\nCartes data : " + _node->getSysinValue();
        if(!_node->getConditionValue().isEmpty()) comment += "\nCondition : " + _node->getConditionValue();
    }
    if(_node->getType()=="FUNCTION")
    {
        replacementNode->setShape(MyShapes::RoundedRectangle);
    }

    replacementNode->setComment(comment);
    addNode(replacementNode);

    foreach(LinkGraphic* link, getLinks(_node))
    {
        if(link->getInput()==_node) link->setInput(replacementNode);
        if(link->getOutput()==_node) link->setOutput(replacementNode);
    }

    deleteNode(_node);

    getMain()->setStatus("Noeud transformé en noeud personnalisable");

}

void CustomGraphics::addLinkedNode(const QString &tabName, QString path)
{
    saveTemporaryFile("Création d'un noeud lié à l'onglet " + tabName);

    CustomNodeGraphic* newNode = new CustomNodeGraphic(tabName, "", MyShapes::RoundedRectangle, this, false);
    QDir workspace(WORKSPACE);
    path = workspace.relativeFilePath(path);
    newNode->setDetailPath(path);
    addNode(newNode);

    QPointF center = mapToScene(this->rect().center());
    qreal xPos = center.x()-newNode->getWidth()/2,
          yPos = center.y()-newNode->getHeight()/2;

    newNode->setPos(xPos, yPos);

    resizeScene();
    setModified(true);

    main->setStatus("Nouveau noeud lié à l'onglet "+tabName+" ajouté au centre de l'écran");

}

/*!
 *  \fn
 *  \brief
 *
 * \param
 * \return
 *
 */
void CustomGraphics::setTitleOnTop(bool _toggled)
{
    TITLE_ON_TOP = _toggled;
    updateScene();
}

/*!
 *  \fn
 *  \brief
 *
 * \param
 * \return
 *
 */
void CustomGraphics::branchSelection(bool _toggled)
{
    SELECT_BRANCH = _toggled;
    if(!SELECT_MODE)
    {
        SELECTABLE_STATE = _toggled;
        QList<QGraphicsItem*> items = myScene->items();
        for(int i=0; i<items.size(); i++)
        {
            items[i]->setFlag(QGraphicsItem::ItemIsSelectable, _toggled);
        }
    }
}


/*!
 *  \fn
 *  \brief
 *
 * \param
 * \return
 *
 */
void CustomGraphics::undo()
{
    //qDebug()<<"void CustomGraphics::undo()";

    if(!tmpSave.size()) return;
    main->setActivatedRedo(true);

    TemporarySave currentTmpSave = tmpSave.pop();

    WriteXML xmlDoc(this, QFileInfo(EXE_PATH, "tmp.ddc").absoluteFilePath(),true);
    redoSave.push(TemporarySave(xmlDoc.write(), "Annulation de la dernière action"));

    cleanScene();
    LoadXML load(currentTmpSave.element, this);
    if(!tmpSave.size()) main->setActivatedUndo(false);
    load.readTmp();
    setModified(true);

    // Options d'affichage en fonction de l'état des menus
    if (ANCHORS_DISPLAYED) displayAnchors(true);
    displayDDName(DISPLAY_DDNAME);
    hideClass(!DISPLAY_DCLFCLASS);
    hideStepName(!DISPLAY_STEPNAME);
    hideSysins(SYSIN_HIDDEN);
    hideConditions(!CONDITIONS_DISPLAYED);
    hideTitle(HIDE_TITLE);
    setTitleOnTop(TITLE_ON_TOP);
    // Options de sélection en fonction de l'état des menus
    setSelectMode(SELECT_MODE);
    branchSelection(SELECT_BRANCH);

    QString actionMessage = currentTmpSave.lastAction;
    main->setStatus("Dernière action annulée : " + actionMessage);

    //qDebug()<<"void CustomGraphics::undo() end";
}

void CustomGraphics::redo()
{
    //qDebug()<<"void CustomGraphics::redo()"<<redoSave.size();

    if(!redoSave.size()) return;

    TemporarySave currentRedoSave = redoSave.pop();
    if(!redoSave.size()) main->setActivatedRedo(false);
    WriteXML xmlDoc(this, QFileInfo(EXE_PATH, "tmp.ddc").absoluteFilePath(), true);
    tmpSave.push(TemporarySave(xmlDoc.write(), "Rétablir la dernière action"));
    main->setActivatedUndo(true);
    cleanScene();
    LoadXML load(currentRedoSave.element, this);
    load.readTmp();

    setModified(true);

    // Options d'affichage en fonction de l'état des menus
    if (ANCHORS_DISPLAYED) displayAnchors(true);
    displayDDName(DISPLAY_DDNAME);
    hideClass(!DISPLAY_DCLFCLASS);
    hideStepName(!DISPLAY_STEPNAME);
    hideSysins(SYSIN_HIDDEN);
    hideConditions(!CONDITIONS_DISPLAYED);
    hideTitle(HIDE_TITLE);
    setTitleOnTop(TITLE_ON_TOP);
    // Options de sélection en fonction de l'état des menus
    setSelectMode(SELECT_MODE);
    branchSelection(SELECT_BRANCH);

    QString actionMessage = currentRedoSave.lastAction;
    main->setStatus("Dernière action rétablie : " + actionMessage);

    //qDebug()<<"void CustomGraphics::redo() end";
}

/*!
 *  \fn
 *  \brief
 *
 * \param
 * \return
 *
 */
void CustomGraphics::displayDDName(bool _toggled)
{
    DISPLAY_DDNAME = _toggled;
    update(rect());
}

/*!
 *  \fn
 *  \brief
 *
 * \param
 * \return
 *
 */
void CustomGraphics::createStep()
{
    saveTemporaryFile("Création d'un noeud Etape");
    StepGraphic* newStep = new StepGraphic("", "STEP", "", "", this, false);
    addNode(newStep);
    QPointF center = mapToScene(this->rect().center());
    newStep->setPos(center.x()-newStep->getWidth()/2, center.y()-newStep->getHeight()/2);

    resizeScene();
    setModified(true);

    main->setStatus("Nouvelle étape ajoutée au centre de l'écran");
    if(!newStep->edit()) undo();
}

/*!
 *  \fn
 *  \brief
 *
 * \param
 * \return
 *
 */
void CustomGraphics::createFunction()
{
    saveTemporaryFile("Création d'un noeud Fonction");
    FunctionGraphic* newFunction = new FunctionGraphic("","","","","",this, false);
    addNode(newFunction);
    QPointF center = mapToScene(this->rect().center());
    newFunction->setPos(center.x()-newFunction->getWidth()/2, center.y()-newFunction->getHeight()/2);

    resizeScene();
    setModified(true);

    main->setStatus("Nouvelle fonction ajoutée au centre de l'écran");
    if(!newFunction->edit()) undo();
}

void CustomGraphics::createCondition()
{
    saveTemporaryFile("Création d'un noeud Condition");
    ConditionNodeGraphic* newCondition = new ConditionNodeGraphic("", "", this, false);
    addNode(newCondition);
    QPointF center = mapToScene(this->rect().center());
    newCondition->setPos(center.x()-newCondition->getWidth()/2, center.y()-newCondition->getHeight()/2);

    resizeScene();
    setModified(true);

    main->setStatus("Nouvelle condition ajoutée au centre de l'écran");
    if(!newCondition->edit()) undo();
}

/*!
 *  \fn
 *  \brief
 *
 * \param
 * \return
 *
 */
void CustomGraphics::createCustomNode()
{
    saveTemporaryFile("Création d'un noeud personnalisable");

    CustomNodeGraphic* newNode = new CustomNodeGraphic("", "", MyShapes::RoundedRectangle, this, false);
    addNode(newNode);

    QPointF center = mapToScene(this->rect().center());
    qreal xPos = center.x()-newNode->getWidth()/2,
          yPos = center.y()-newNode->getHeight()/2;

    newNode->setPos(xPos, yPos);

    resizeScene();
    setModified(true);

    main->setStatus("Nouveau noeud personnalisable ajouté au centre de l'écran");
    if(!newNode->edit()) undo();
}

/*!
 *  \fn
 *  \brief
 *
 * \param
 * \return
 *
 */
void CustomGraphics::createDCLF()
{
    saveTemporaryFile("Création d'un noeud DCLF");
    DCLFGraphic* newDCLF = new DCLFGraphic("","","","","",0,this, false);
    addNode(newDCLF);
    QPointF center = mapToScene(this->rect().center());
    newDCLF->setPos(center.x()-newDCLF->getWidth()/2, center.y()-newDCLF->getHeight()/2);

    resizeScene();
    setModified(true);

    main->setStatus("Nouvelle DCLF ajoutée au centre de l'écran");

    if(!newDCLF->edit()) undo();
}

/*!
 *  \fn
 *  \brief
 *
 * \param
 * \return
 *
 */
void CustomGraphics::displayAnchors(bool _display, bool _temporary)
{
    if(!_temporary) ANCHORS_DISPLAYED = _display;
    for(int i=0; i<nodeGraphic.size(); i++)
    {
        if(_display)
        {
            nodeGraphic[i]->getInputAnchor()->show();
            nodeGraphic[i]->getOutputAnchor()->show();
        }
        else
        {
            setNbClickedAnchors(0);
            if(firstClickedAnchor)
            {
                firstClickedAnchor->setCreatingLink(false);
                firstClickedAnchor = 0;
            }

            nodeGraphic[i]->getInputAnchor()->hide();
            nodeGraphic[i]->getInputAnchor()->setColor(Qt::black);
            nodeGraphic[i]->getOutputAnchor()->hide();
            nodeGraphic[i]->getOutputAnchor()->setColor(Qt::black);
        }
    }
}


/*!
 *  \fn
 *  \brief
 *
 * \param
 * \return
 *
 */
void CustomGraphics::setSelectMode(bool _toggled)
{
    SELECT_MODE = _toggled;
    SELECTABLE_STATE = _toggled;

    for(int i=0;i<nodeGraphic.size(); i++) { nodeGraphic[i]->setFlag(QGraphicsItem::ItemIsSelectable, _toggled); }
    setDragMode(_toggled ? QGraphicsView::RubberBandDrag : QGraphicsView::ScrollHandDrag);
}

/*!
 *  \fn
 *  \brief
 *
 * \param
 * \return
 *
 */
void CustomGraphics::saveImage()
{
    QString _title = getTabName();

    QString myfile  = QFileDialog::getSaveFileName(this, tr("Save image File"), WRITE_IMAGE_PATH + "/" + _title + ".png", tr("Image files (*.png *.jpg)"));
    if(!myfile.isEmpty())
    {
        WRITE_IMAGE_PATH = QFileInfo(myfile).dir().path();
        myScene->clearSelection();                                                  // Selections would also render to the file
        myScene->setSceneRect(myScene->itemsBoundingRect());                          // Re-shrink the scene to it's bounding contents
        QImage image(myScene->sceneRect().size().toSize(), QImage::Format_ARGB32);  // Create the image with the exact size of the shrunk scene
        image.fill(Qt::transparent);                                              // Start all pixels transparent

        QPainter painter(&image);
        myScene->render(&painter);
        image.save(myfile);

        main->setStatus("Image enregistrée sous " + myfile);
    }
}

/*!
 *  \fn
 *  \brief
 *
 * \param
 * \return
 *
 */
void CustomGraphics::copyImageToClipboard()
{
    myScene->clearSelection();                                    // Selections would also render to the file
    myScene->setSceneRect(myScene->itemsBoundingRect());          // Re-shrink the scene to it's bounding contents
    QImage image(myScene->sceneRect().size().toSize(), QImage::Format_ARGB32);  // Create the image with the exact size of the shrunk scene

    image.fill(Qt::transparent);
    QPainter painter(&image);
    myScene->render(&painter);
    image.save("tmp.png");

    QImage imageLoaded("tmp.png");
    QMimeData * mimeData = new QMimeData();
    QByteArray data;
    QBuffer buffer(&data);
    buffer.open(QIODevice::WriteOnly);
    imageLoaded.save(&buffer,"PNG");
    buffer.close();
    mimeData->setData("PNG",data);

    QApplication::clipboard()->setMimeData(mimeData,QClipboard::Clipboard);

    main->setStatus("Image copiée dans le presse papier");
}

void CustomGraphics::modifyColorOfSelection()
{
    saveTemporaryFile("Modification de la couleur des noeuds sélectionnés");

    QColor newColor = QColorDialog::getColor(Qt::white, this);

    if(! newColor.isValid()) return;
    foreach(NodeGraphic* node, getSelectedNodes())
    {
        node->setColor(newColor);
        node->setColorSaved(newColor);
        updateDCLFColor(node->getName(),newColor);
        updateFunctions(node->getName(), newColor);
        update();
    }
    getMain()->setStatus("Couleur des noeuds sélectionnés modifiée" );
}

void CustomGraphics::resetColorOfSelection()
{
    saveTemporaryFile("Réinitialisation de la couleur des noeuds sélectionnés");

    foreach(NodeGraphic* node, getSelectedNodes())
    {
        QColor color = Qt::white;

        if (node->getType()=="STEP")
        {
            color = node->getIsTechnical() ? TECHNICAL_STEP_COLOR : NORMAL_STEP_COLOR;
            if(node->getSubtype()=="PATTERN") color = PATTERN_STEP_COLOR;
        }

        node->setColor(color);
        node->setColorSaved(color);

        if(node->getType()=="DCLF")
        {
            updateDCLF(node->getName(), DCLFGraphic::getColorFromRules(node->getClass()), node->getClass(), node->getFormat(), node->getLength());
        }
    }

    getMain()->setStatus("Couleur des noeuds sélectionnés réinitialisée" );
}

/*!
 *  \fn
 *  \brief
 *
 * \param
 * \return
 *
 */
/// Ouvre une fenêtre permettant de séparer certaines DCLF en plusieurs noeuds pour organiser au mieux le dessin de chaine
void CustomGraphics::splitDCLFIfNeeded()
{
    SplitDCLFDialog splitDCLFWindow(this);
    connect(&splitDCLFWindow, SIGNAL(resortSignal()), this->getMain(), SLOT(reSort()));
    bool exec = false;

    for(int i=0; i<nodeGraphic.size(); i++)
    {
        if(nodeGraphic[i]->getType()!="DCLF") continue;

        if(getOutputLinksNb(nodeGraphic[i]) > 1 || getInputLinksNb(nodeGraphic[i]) > 1 ||
                (nodeGraphic[i]->getClass()!="XP" && nodeGraphic[i]->getClass()!="XP2"
                 && (getOutputLinksNb(nodeGraphic[i])+getInputLinksNb(nodeGraphic[i]) > 1)))
        {
            exec = true;
            SplitDCLFWidget * line = new SplitDCLFWidget(nodeGraphic[i], getInputLinksNb(nodeGraphic[i]), getOutputLinksNb(nodeGraphic[i]), &splitDCLFWindow);
            splitDCLFWindow.addLine(line);
        }
    }

    if(exec) splitDCLFWindow.exec();
}


/*!
 *  \fn
 *  \brief
 *
 * \param
 * \return
 *
 */
bool CustomGraphics::load(const QString &_path, bool _toBeDisplayed)
{
    currentFilename = _path;

    if(!_path.isEmpty())
    {
        LoadXML loadXML(_path, this);
        if(loadXML.getError()) return false;
        // Si xml en entrée loadXML.read() fait le job, sinon on utilise un traducteur
        if(!loadXML.read())
        {
            if(_toBeDisplayed)
            {
                main->getProgressBar()->setVisible(true);
                main->getProgressBar()->setValue(0);
            }
            int translatorIndex = chooseTranslator(_path);

            /// Traduction d'un PCL
            if(translatorIndex == 0)
            {
                /// >Traduire le PCL en objets C++ ...
                main->setStatus("Traduction du jobset en objets");
                // Détecter les variables dans le fichier et faire un vecteur d'association variable->valeur
                QVector<VariableValue*> variables = detectPatternVariables(_path);
                InitDialog* dialog = new InitDialog(variables, this);
                if(dialog->exec()==QDialog::Rejected) return false;

                PCLTranslator translator(variables);

                Jobset jobset = translator.translate(_path);
                /// >... puis en éléments graphiques
                if(_toBeDisplayed) main->getProgressBar()->setValue(05);
                main->setStatus("Représentation graphique des objets");
                translateToGraphics(jobset);
                if(_toBeDisplayed) main->getProgressBar()->setValue(15);

                /// >Demander si on veut séparer certains noeuds, et repositionner si c'est le cas
                if(_toBeDisplayed)
                {
                    /// >Positionner les noeuds
                    main->setStatus("Tri des noeuds");

                    splitDCLFIfNeeded();
                    reSort();
                }
            }

            /// Traduction d'un programme Pacbase
            else if (translatorIndex == 1)
            {
                PacbaseTranslator translator;

                InitDialog* dialog = new InitDialog(QVector<VariableValue*>(), this);
                if(dialog->exec()==QDialog::Rejected) return false;

                main->setStatus("Traduction du jobset en objets");
                PacbaseProgram program = translator.translate(_path);
                if(_toBeDisplayed) main->getProgressBar()->setValue(05);
                main->setStatus("Représentation graphique des objets");
                translateToGraphics(program);
                if(_toBeDisplayed) main->getProgressBar()->setValue(15);
                reSort();
            }

            /// Traduction d'un JCL
            else if (translatorIndex == 2)
            {
                JCLTranslator translator;
                InitDialog* dialog = new InitDialog(QVector<VariableValue*>(), this);
                if(dialog->exec()==QDialog::Rejected) return false;

                main->setStatus("Traduction du jobset en objets");
                Jobset jobset = translator.translate(_path);
                if(_toBeDisplayed) main->getProgressBar()->setValue(05);
                main->setStatus("Représentation graphique des objets");
                translateToGraphics(jobset);
                if(_toBeDisplayed)
                {
                    main->getProgressBar()->setValue(15);
                    reSort();
                }
            }

            /// Traduction d'un fichier ORTI
            else if (translatorIndex == 3)
            {
                OrtiTranslator translator;

                main->setStatus("Traduction du fichier ORTI en objets");
                OrtiFile ortifile = translator.translate(_path);


                InitDialog* dialog = new InitDialog(ortifile.getResources(), this);
                if(dialog->exec()==QDialog::Rejected) return false;

                if(_toBeDisplayed) main->getProgressBar()->setValue(05);
                main->setStatus("Représentation graphique des objets");
                translateToGraphics(ortifile);

            }
        }
        else xmlLoaded = true;

        resizeScene();
        main->setWindowTitle(renameWindow());
        main->getSearchWidget()->search();
    }

    QList<QGraphicsItem*> items = myScene->items();
    for(int i=0; i<items.size(); i++) items[i]->setFlag(QGraphicsItem::ItemIsSelectable, SELECTABLE_STATE);

    if(!_path.isEmpty()) setModified(false);

    return true;
}

bool CustomGraphics::loadFromPlugin(const QString &_path, FilePlugin* _plugin)
{
    currentFilename = _path;

    if(!_path.isEmpty())
    {
        LoadXML loadXML(_path, this);
        if(loadXML.getError()) return false;
        // Si xml en entrée loadXML.read() fait le job, sinon on utilise un traducteur
        if(!loadXML.read())
        {

            main->getProgressBar()->setVisible(true);
            main->getProgressBar()->setValue(0);

            InitDialog* dialog = new InitDialog(QVector<VariableValue*>(), this);
            if(dialog->exec()==QDialog::Rejected) return false;

            main->setStatus("Traduction du jobset en objets");
            _plugin->translate(_path);
            setTabName(_plugin->getTabName());
            main->getProgressBar()->setValue(05);
            main->setStatus("Représentation graphique des objets");
            QDomElement config = _plugin->translateToGraphics();
            title->setTitle(_plugin->getTitle());

            LoadXML loadXML(QFileInfo(EXE_PATH, "tmp.ddc").absoluteFilePath(), this);
            if(loadXML.getError()) return false;
            // Si xml en entrée loadXML.read() fait le job, sinon on utilise un traducteur
            loadXML.read();
            main->getProgressBar()->setValue(15);
            reSort();


        }
        else xmlLoaded = true;

        resizeScene();
        main->setWindowTitle(renameWindow());
        main->getSearchWidget()->search();
    }

    QList<QGraphicsItem*> items = myScene->items();
    for(int i=0; i<items.size(); i++) items[i]->setFlag(QGraphicsItem::ItemIsSelectable, SELECTABLE_STATE);

    if(!_path.isEmpty()) setModified(false);

    return true;
}

QVector<VariableValue*> CustomGraphics::detectPatternVariables(const QString &_path)
{
    QVector<VariableValue*> variables;
    QVector<QString> variableNames;
    QFile inputFile(_path);
    if(!inputFile.open(QIODevice::ReadOnly|QIODevice::Text)) { qFatal("ERROR when opening the input PCL File !"); exit(1); }
    else
    {
        QTextStream stream(&inputFile);
        while(!stream.atEnd())
        {
            QString line = stream.readLine();
            QRegExp commentedLine("^\\* ?(.*)[\\- \\!=]*"); /// line beginning with *, keeping all the characters but the final " -!=" and the " " in front
            if(!line.contains(commentedLine))
            {
                //Regarder si une variable est dans la ligne => si c'est le cas on l'ajoute au vecteur
                QRegExp variablePresent("^(.*)&(\\w+)(\\.?)(.*)");
                if(line.contains(variablePresent))
                {
                    QStringList subStrings = variablePresent.capturedTexts();
                    QString subLine = subStrings[1];

                    if(!variableNames.contains(subStrings[2]))
                    {
                        VariableValue* variable = new VariableValue();
                        variable->setName(subStrings[2]);
                        variable->setLine(line);
                        variableNames.push_back(subStrings[2]);
                        variables.push_back(variable);
                    }

                    while(subLine.contains(variablePresent))
                    {
                        subStrings = variablePresent.capturedTexts();
                        subLine = subStrings[1];

                        if(!variableNames.contains(subStrings[2]))
                        {
                            VariableValue* variable = new VariableValue();
                            variable->setName(subStrings[2]);
                            variable->setLine(line);
                            variableNames.push_back(subStrings[2]);
                            variables.push_back(variable);
                        }
                    }
                }
            }
        }
    }

    inputFile.close();
    return variables;
}


void CustomGraphics::reSort()
{
    main->getProgressBar()->setVisible(true);
    clearPositions();
    positionNodes();
    main->zoomWindow();
    main->getProgressBar()->setValue(100);
    main->setStatus("Noeuds triés");
    main->getProgressBar()->setVisible(false);
}

/*!
 *  \fn
 *  \brief
 *
 * \param
 * \return
 *
 */
int CustomGraphics::chooseTranslator(const QString &_path)
{
    QFile inputPCLFile(_path);
    if(!inputPCLFile.open(QIODevice::ReadOnly|QIODevice::Text)) { qFatal("ERROR when opening the input File !"); exit(1); }
    else
    {
        QTextStream stream(&inputPCLFile);
        while(!stream.atEnd())
        {
            QString line = stream.readLine();
            QRegExp pacbaseLine("1.*[*]{120}.*");
            QRegExp ortiLine(" [*]{2} .*");
            if(line.contains(pacbaseLine)) return 1;
            if(line.contains(ortiLine)) return 3;
            /// JCL
            if(line.startsWith("//")) return 2;
            break;
        }
    }
    return 0;
}

/*!
 *  \fn void CustomGraphics::saveAs()
 *  \brief Sauvegarde du dessin de chaine sous un nouveau nom
 */
void CustomGraphics::saveAs()
{
    //qDebug()<<"saveAs()";
    main->setSavingInProgress(true);

    /// Ouverture de la fenêtre de choix du chemin du nouveau fichier
    QString _title = getTabName() ;
    QString myfile  = QFileDialog::getSaveFileName(this, tr("Sauvegarde au format DDC"), WRITE_PATH + "/" +_title + ".ddc", tr("Dessins de chaine (*.ddc);;Fichiers XML (*.xml);;Fichiers Text (*.txt"));

    /// Enregistrement si la demande n'a pas été annulée
    if(!myfile.isEmpty())
    {
        WRITE_PATH = QFileInfo(myfile).dir().path();
        ///>Forcer la valeur de xmlLoaded pour éviter de repasser dans saveAs() au prochain passage dans save()
        xmlLoaded = true;

        WriteXML xmlDoc(this, myfile);
        xmlDoc.write();

        currentFilename = myfile;

        setModified(false);
        main->setWindowTitle(renameWindow());

        main->setStatus("Dessin de chaine enregistré sous " + myfile);
    }

    /// Déselectionner les noeuds séléctionnés avec Ctrl+Clic ou Maj+Clic
    if(!SELECT_MODE)
    {
        QList<QGraphicsItem*> items = myScene->items();
        SELECTABLE_STATE = false;
        for(int i=0; i<items.size(); i++)
        {
            items[i]->setFlag(QGraphicsItem::ItemIsSelectable, SELECTABLE_STATE);
        }
    }
    main->setSavingInProgress(false);
    main->getSearchWidget()->search();
}

/*!
 *  \fn
 *  \brief
 *
 * \param
 * \return
 *
 */
void CustomGraphics::save()
{
    main->setSavingInProgress(true);
    if(currentFilename=="" || !xmlLoaded) saveAs();
    else
    {
        WriteXML xmlDoc(this, currentFilename);
        if(xmlDoc.getAccepted())
        {
            xmlDoc.write();
            setModified(false);
            main->setStatus("Dessin de chaine enregistré (" + currentFilename + ")");
        }
    }
    main->setSavingInProgress(false);
    main->getSearchWidget()->search();
}

/*!
 *  \fn
 *  \brief
 *
 * \param
 * \return
 *
 */
void CustomGraphics::exportCSV()
{
    /// Ouverture de la fenêtre de choix du chemin du nouveau fichier
    QString _title = getTabName() ;
    QString myfile  = QFileDialog::getSaveFileName(this, tr("Export au format CSV"), WRITE_PATH + "/" +_title + ".csv", tr("Fichiers CSV (*.csv);;Fichiers Texte (*.txt"));

    if(!myfile.isEmpty())
    {
        QFile csvFile(myfile);
        if(csvFile.open(QFile::WriteOnly |QFile::Truncate))
        {
            QString separator = ";";
            QTextStream output(&csvFile);

            int minx = 0, miny = 0;
            for(int i=0; i<nodeGraphic.size(); i++)
            {
                NodeGraphic* currentNode = nodeGraphic[i];
                if((int)currentNode->pos().x() < minx) minx = (int)currentNode->pos().x();
                if((int)currentNode->pos().y() < miny) miny = (int)currentNode->pos().y();
            }
            for(int i=0; i<nodeGraphic.size(); i++)
            {
                NodeGraphic* currentNode = nodeGraphic[i];

                output << currentNode->getType() << separator;
                output << currentNode->getId() << separator;
                output << (int)currentNode->pos().x() - minx << separator;
                output << (int)currentNode->pos().y() - miny << separator;
                output << currentNode->getWidth() << separator;
                output << currentNode->getHeight() << separator;
                output << currentNode->getColorSaved().red() << separator;
                output << currentNode->getColorSaved().green() << separator;
                output << currentNode->getColorSaved().blue() << separator;

                if(currentNode->getType()=="DCLF")
                {
                    output << currentNode->getClass() << separator;
                    output << currentNode->getName() << separator;
                    output << currentNode->getSegments();
                }
                if(currentNode->getType()=="STEP")
                {
                    output << currentNode->getName() << separator;
                    output << currentNode->getProgram() << separator;
                    output << currentNode->getPrecision();
                }
                if(currentNode->getType()=="CUSTOM")
                {
                    output << currentNode->getProgram();
                }

                output << endl;

            }

            for(int i=0; i<linkGraphic.size(); i++)
            {
                LinkGraphic* currentLink = linkGraphic[i];
                output << "LINK" << separator;
                output << currentLink->getInput()->getId() << separator;
                output << currentLink->getOutput()->getId() << separator;
                output << currentLink->getDDName();
                output << endl;
            }
        }
        csvFile.close();
        main->setStatus("Dessin de chaine exporté au format CSV (" + myfile + ")");
    }

}

void CustomGraphics::updateLinks()
{
    foreach (NodeGraphic* node, nodeGraphic)
    {
        if(node->getType()!="CUSTOM" || !node->getDetailPath().isEmpty()) continue;
        QFileInfo file(WORKSPACE + "\\" + node->getText().simplified()+".ddc");
        if (file.exists() && file.isFile()) node->setDetailPath(node->getText().simplified()+".ddc");
    }

   updateScene();
}

/*!
 *  \fn
 *  \brief
 *
 * \param
 * \return
 *
 */
void CustomGraphics::newFile()
{
    //saveTemporaryFile("Création d'un nouveau dessin de chaine");
    currentFilename = "";
    main->setWindowTitle(renameWindow());
    cleanScene();
    setModified(true);
}

/*!
 *  \fn QVector<float> CustomGraphics::getBoundaries(bool _exceptSelected)
 *  \brief  renvoie un vecteur contenant les paramètres du rectangle englobant les noeuds de la scène
 *
 * \param bool _exceptSelected : Permet de renvoyer les limites de l'ensemble des noeuds non séléctionnés seulement (faux par défaut)
 * \return : renvoie un vecteur contenant : xmin, ymin, width et height
 * où xmin et ymin sont les coordonnées du noeuds origine,
 * et W et H la largeur et la hauteur de la scène
 */
QVector<float> CustomGraphics::getBoundaries(bool _exceptSelected)
{
    QVector<float> boundaries;
    float xmin=0, xmax=0, ymin=0, ymax=0;
    bool initialized = false;

    /// Initialisation des valeurs avec un noeud (pas forcément le premier si _exceptSelected est vrai
    foreach (NodeGraphic* node, nodeGraphic)
    {
        if(initialized) break;
        if(_exceptSelected && node->isSelected()) continue;
        xmin = node->pos().x(),
        ymin = node->pos().y(),
        xmax = node->pos().x() + nodeGraphic[0]->getWidth(),
        ymax = node->pos().y() + nodeGraphic[0]->getHeight();
        initialized = true;
    }

    /// Si aucun noeud dans la scène : renvoi d'un vecteur de zéros
    if(!nodeGraphic.size() || !initialized)
    {
        boundaries.push_back(0);
        boundaries.push_back(0);
        boundaries.push_back(0);
        boundaries.push_back(0);
        return boundaries;
    }

    /// Recherche des coordonnées extrèmes
    foreach(NodeGraphic* node, nodeGraphic)
    {
        if(_exceptSelected && node->isSelected()) continue;
        xmin = qMin(xmin,(float) node->pos().x());
        ymin = qMin(ymin,(float) node->pos().y());
        xmax = qMax(xmax,(float) (node->pos().x() + node->getWidth()));
        ymax = qMax(ymax,(float) (node->pos().y() + node->getHeight()));
    }
    foreach(LinkGraphic* link, linkGraphic)
    {
        if(_exceptSelected && (link->getInput()->isSelected()||link->getOutput()->isSelected())) continue;
        xmin = qMin(xmin, (float) link->boundingRect().x());
        xmax = qMax(xmax, (float) (link->boundingRect().x()+link->boundingRect().width()));
        ymin = qMin(ymin, (float) link->boundingRect().y());
        ymax = qMax(ymax, (float) (link->boundingRect().y()+link->boundingRect().height()));
    }

    /// Renvoi des valeurs
    float width = xmax-xmin, height = ymax-ymin;
    boundaries.push_back(xmin);
    boundaries.push_back(ymin);
    boundaries.push_back(width);
    boundaries.push_back(height);

    return boundaries;
}

void CustomGraphics::translateNodes(float deltaX, float deltaY)
{
    deltaX /= ALIGN_GRID_X;
    deltaY /= ALIGN_GRID_Y;

    foreach(NodeGraphic* node, nodeGraphic)
    {
        node->setXPos(node->getXPos()+deltaX);
        node->setYPos(node->getYPos()+deltaY);
    }

    title->setPosition();
}

void CustomGraphics::translateSelectedNodes(float deltaX, float deltaY)
{
    foreach(NodeGraphic* node, nodeGraphic)
    {
        if(!node->isSelected()) continue;
        node->setX(node->pos().x()+deltaX);
        node->setY(node->pos().y()+deltaY);
    }

    title->setPosition();
}

void CustomGraphics::versionUpdate()
{
    QString openingLocation = LOAD_PATH;
    QString myfile = QFileDialog::getOpenFileName(this, tr("Ouvrir un fichier .txt ou .xml"), openingLocation, tr("Fichiers reconnus(*.txt *.ddc);;Tous les fichiers(*);;Fichiers Texte(*.txt);;Dessins de chaine(*.ddc)"));

    if(!myfile.isEmpty())
    {
        CustomGraphics* temporaryGraphics = new CustomGraphics(main);
        if(temporaryGraphics->load(myfile, false))
        {
            QVector<NodeGraphic*> newNodes = temporaryGraphics->getNodeGraphics();
            QVector<NodeGraphic*> oldNodes = getNodeGraphics();

            VersionUpdateDialog* dialog = new VersionUpdateDialog(oldNodes, newNodes, this, temporaryGraphics, this);
            if(dialog->exec()==QDialog::Accepted)
            {
                getMain()->zoomWindow();
                getTitle()->setVersion(temporaryGraphics->getTitle()->getVersion());
            }
        }

        temporaryGraphics->cleanScene(false);
        delete temporaryGraphics;
    }

}

void CustomGraphics::focusInEvent(QFocusEvent *e)
{
    QGraphicsView::focusInEvent(e);

    if(!main->getFocusInAlertSent())
    {
        main->setFocusInAlertSent(true);
        main->warnAboutConflict(this);
    }
    else
    {
        main->setFocusInAlertSent(false);
    }
}
