#ifndef CUSTOMGRAPHICS_H
#define CUSTOMGRAPHICS_H

#include <QGraphicsView>
#include <QDebug>
#include <QtGui>
#include <QtXml>
#include <QStack>

#include "jobset.h"
#include "stepgraphic.h"
#include "dclfgraphic.h"
#include "functiongraphic.h"
#include "linkgraphic.h"
#include "nodegraphic.h"
#include "conditionnodegraphic.h"
#include "customnodegraphic.h"
#include "pacbaseprogram.h"
#include "ortifile.h"
#include "sysingraphic.h"
#include "anchorgraphic.h"
#include "titlegraphic.h"
#include "customgraphicscene.h"
#include "fileplugininterface.h"

class LinkGraphic;
class AnchorGraphic;
class TitleGraphic;
/*! \class TemporarySave
 * \brief Sauvegarde temporaire sous forme d'un QDomElement (arbre XML)
 * Un libellé permet d'afficher la dernière action annulée en cas de retour arrière
 *
 */
class TemporarySave
{
public:
    QDomElement element; /*! Sauvegarde de l'arbre XML*/
    QString lastAction; /*! Libellé de l'action effectuée après la sauvegarde*/
    TemporarySave(const QDomElement &_element, const QString &_lastAction):element(_element),lastAction(_lastAction){}
    TemporarySave(){}
};

/*! \class VariableValue
 * \brief Variable de pattern
 *
 * Cette classe permet de remplacer les variables dans les pattern
 *
 */
class VariableValue
{
private:
    QString name; /*! Nom de la variable */
    QString line; /*! Exemple d'utilisation de la variable */
    QString value; /*! Valeur de la variable */

public:
    QString getName() const {return name;}
    void setName(const QString &value){name = value;}

    QString getValue() const {return value;}
    void setValue(const QString &_value){value = _value;}

    QString getLine() const {return line;}
    void setLine(const QString &value){line = value;}

};

/*! \class CustomGraphics
 * \brief Vue graphique dans laquelle est représenté le dessin de chaine
 *
 *
 *
 */
class CustomGraphics : public QGraphicsView
{
private:
    bool xmlLoaded; /*! xmlLoaded (Cet attribut permet de savoir si on a ouvert le fichier depuis un fichier XML ou non)*/
    bool shiftPressedFirst;
    bool ctrlPressedFirst;
    bool savedSelectedState;

    MainWindow* main;
    QVector<LinkGraphic*> linkGraphic;
    QVector<NodeGraphic*> nodeGraphic;
    CustomGraphicScene *myScene;
    QString currentFilename;
    QString tabName;
    QDate modificationDate;
    QTime modificationTime;
    QString modificationUser;
    TitleGraphic* title;
    QStack<TemporarySave> tmpSave;
    QStack<TemporarySave> redoSave;
    bool modified;

    AnchorGraphic* firstClickedAnchor;
    int nbClickedAnchors;
    double scaleFactor;

    int idCpt;
    int nbOfGroups; /*! Nombre de groupes de noeuds disjoints*/

    bool notDoneEtaging();
    int getMaxYLevel(int _group = 0);
    int getMaxXLevel(int _group);
    int getMaxInputLevel(NodeGraphic *_item);
    int getMinOutputLevel(NodeGraphic *_item, int _maxLevel);

    void recursiveSetGroup(NodeGraphic *_node, int _group);
    int initRank();

    void tightenTree();
    void setNodesScanned(bool _value);
    void setLinksScanned(bool _value);
    void splitDCLFIfNeeded();
    void clearPositions();
    void initXPosition();

public:
    CustomGraphics(QWidget* _parent);
    ~CustomGraphics();

    // Getters & Setters
    int getTmpSize(){ return tmpSave.size(); }
    int getRedoSize(){ return redoSave.size(); }

    DCLFGraphic* getDclfGraphic(const QString &_name) const;
    QVector<DCLFGraphic*> getDclfGraphics() const;

    QVector<NodeGraphic*> getNodeGraphics() const {return nodeGraphic;}
    NodeGraphic* getNodeGraphic(int _id) const;
    void addNode(NodeGraphic* _node);
    void addLink(LinkGraphic* _link);

    QVector<LinkGraphic*> getLinkGraphics() const {return linkGraphic;}
    int getInputLinksNb(QGraphicsItem* _item);
    int getOutputLinksNb(QGraphicsItem* _item);

    bool getModified() const {return modified;}
    void setModified(bool _modified);

    MainWindow *getMain() const { return main; }
    void setMain(MainWindow *value) { main = value; }

    bool getXmlLoaded() const { return xmlLoaded; }
    void setXmlLoaded(bool value) { xmlLoaded = value; }

    double getScaleFactor() const { return scaleFactor; }
    void setScaleFactor(double value) { scaleFactor = value; }

    QString getCurrentFilename() const { return currentFilename; }
    void setCurrentFilename(const QString &value) { currentFilename = value; }

    int getIdCpt() const { return idCpt; }

    int getMaxId();
    void incrementIdCpt() { idCpt++; }

    TitleGraphic *getTitle() const { return title; }
    void setTitle(const QString &_title);

    QString getTabName() const { return tabName; }
    void setTabName(const QString &value) { tabName = value; }

    QGraphicsScene * getScene() const {return myScene;}
    void addToScene(QGraphicsItem* _item) { myScene->addItem(_item); }
    void removeFromScene(QGraphicsItem* _item) { myScene->removeItem(_item); }

    QString renameWindow();

    void cleanScene(bool _delete = true);

    // Traduction
    int chooseTranslator(const QString &_path);
    void translateToGraphics(const Jobset &_jobset);
    void translateToGraphics(const PacbaseProgram &_program);
    void translateToGraphics(const OrtiFile &_ortiFile);

    QVector<NodeGraphic *> createInstructionNodes(SubFunction *_subfunction, const PacbaseProgram &_program);

    void positionNodes();

    void createLinksFromRules(const QString &_program, const Step &_step, StepGraphic* _stepGraphic);

    AnchorGraphic* getFirstClickedAnchor() const { return firstClickedAnchor; }
    int getNbClickedAnchors() const { return nbClickedAnchors; }
    void setFirstClickedAnchor(AnchorGraphic* _anchor){firstClickedAnchor = _anchor;}
    void setNbClickedAnchors(int _nb) { nbClickedAnchors = _nb; }

    LinkGraphic* linkExists(NodeGraphic*, NodeGraphic*) const;
    LinkGraphic *linkExists(const QString &_dclfName, NodeGraphic *_step, bool _outputFile) const;

    void deleteLink(NodeGraphic* _input,  NodeGraphic* _output);
    void deleteNode(NodeGraphic *_node);
    void deleteAssociatedLinks(NodeGraphic *_node);

    void setColorToDCLF(const QString&, const QColor&);
    void updateDCLF(const QString&, const QColor&, const QString&, const QString&, int);
    void updateDCLFColor(const QString &_name, const QColor &_color);

    void setColorToFunction(const QString &_name, const QColor &_color);
    void setColorToFunction(const QString _name);

    void alignBranch(NodeGraphic*, bool _ctrlPressed);

    NodeGraphic* getPreviousNode(NodeGraphic*, bool _noNegativeSlack = false);
    NodeGraphic *getFollowingNode(NodeGraphic *_node, bool _noNegativeSlack = false);

    int outputCount(NodeGraphic*);
    int inputCount(NodeGraphic *_node);
    QVector<LinkGraphic*> getLinks(NodeGraphic* _node);
    QVector<LinkGraphic*> getLinks(int _rank);

    NodeGraphic* getNodeFromPos(float _x, float _y);
    void switchPos(NodeGraphic* _node1, NodeGraphic* _node2);

    void splitNode(NodeGraphic* _node);

    void deleteSelectedNodes();
    void copySelectedNodes();
    void align();
    void resizeScene();
    void saveTemporaryFile(const QString &_lastAction);
    void setSelectMode(bool);
    void saveImage();
    void undo();
    void redo();
    void saveAs();
    void save();
    void exportCSV();
    void updateLinks();

    bool load(const QString &_path, bool _toBeDisplayed = true);
    bool loadFromPlugin(const QString &_path, FilePlugin *_plugin);

    void newFile();
    void displayAnchors(bool, bool _temporary = false);
    void hideSysins(bool);
    void setTitleOnTop(bool _toggled);
    void hideTitle(bool);
    void branchSelection(bool _toggled);
    void createStep();
    void createCustomNode();
    void createDCLF();
    void createFunction();
    void createCondition();
    void copy();
    void cut();
    void paste();
    void displayDDName(bool _toggled);
    void hideConditions(bool _hide);
    void hideClass(bool _toggled);
    void hideStepName(bool _toggled);
    void makeCustom(NodeGraphic* _node);

    void addLinkedNode(const QString &tabName, QString path);
    void hideLinkedSchematics(bool _toggled);

    void setGroups();
    void findTightSubTree(NodeGraphic *_node);
    void zoomIn();
    void zoomOut();

    QVector<NodeGraphic *> getNodesInGroup(int _group);
    void initCutValues();

    void propagateTail(NodeGraphic *_node);

    void cutValueLoop();
    LinkGraphic *getLeaveEdge(int _group);
    void exchangeEdges(int _group, LinkGraphic *_leaveEdge);
    void addVirtualNodes();
    void deleteVirtualNodes();
    QVector<int> depthFirstSearch(NodeGraphic *_node, QVector<int> _xLevels, int _xOffset);
    void ordering();
    void wmedian(int _iteration);
    void transpose();
    QVector<NodeGraphic *> getNodesInRank(int _rank, int _group);
    static bool sortX(NodeGraphic *&_n1, NodeGraphic *&_n2);
    static bool sortMedian(NodeGraphic *&_n1, NodeGraphic *&_n2);

    float medianValue(NodeGraphic *_node, int _adjacentRank);
    bool linkedToEachOther(NodeGraphic *_n1, NodeGraphic *_n2);
    int crossings(NodeGraphic *_nLeft, NodeGraphic *_nRight);
    void xCoordinates();
    int giveBranchIds(QVector<NodeGraphic *> _nodes);
    QVector<NodeGraphic *> getNodesInBranch(int _branchId, int _group);
    void moveBranchTo(int _posX, QVector<NodeGraphic *> _nodesInBranch);
    void clearBranchIds();
    bool getNeedForSplit() const;
    void setNeedForSplit(bool value);

    void reSort();
    void updateFunctions(const QString &_name, const QColor &_color);

    void unselectNodes();
    void duplicate();
    void copyImageToClipboard();
    void modifyColorOfSelection();
    void resetColorOfSelection();

    QVector<NodeGraphic *> getSelectedNodes();
    QVector<LinkGraphic *> getSelectedLinks();
    QVector<float> getBoundaries(bool onlySelected = false);

    void translateNodes(float deltaX, float deltaY);
    void translateSelectedNodes(float deltaX, float deltaY);

    void versionUpdate();
    QVector<VariableValue *> detectPatternVariables(const QString &_path);

    QDate getModificationDate() const;
    void setModificationDate(const QDate &value);

    QTime getModificationTime() const;
    void setModificationTime(const QTime &value);

    QString getModificationUser() const;
    void setModificationUser(const QString &value);

    void updateScene();

    bool createLink(NodeGraphic *_input, NodeGraphic *_output, QString _ddname, QString _alternativeDdname = "", bool _automatic = true);
    void createLinks(QVector<NodeGraphic *> _inputs, NodeGraphic *_output, QString _ddname, QString _alternativeDdname = "", bool _automatic = true);
protected:
    void wheelEvent(QWheelEvent* event); /// Zoom in/out
    void keyPressEvent(QKeyEvent *event);
    void keyReleaseEvent(QKeyEvent *event);
    void dragEnterEvent(QDragEnterEvent *e);
    void dragMoveEvent(QDragEnterEvent *e);
    void dropEvent(QDropEvent *e);
    void focusInEvent(QFocusEvent *e);
};

#endif // CUSTOMGRAPHICS_H
