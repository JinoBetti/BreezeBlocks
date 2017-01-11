#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#include <QDebug>
#include <QMainWindow>
#include <QGraphicsScene>
#include <QMessageBox>
#include <QtPrintSupport/QPrinter>

#include "jobset.h"
#include "pcltranslator.h"
#include "jcltranslator.h"
#include "stepgraphic.h"
#include "dclfgraphic.h"
#include "linkgraphic.h"
#include "nodegraphic.h"
#include "sysingraphic.h"
#include "anchorgraphic.h"
#include "titlegraphic.h"
#include "customtabwidget.h"
#include "colorrulewidget.h"
#include "loadlastversiondialog.h"

#include <QProgressBar>

namespace Ui {
class MainWindow;
}

class LinkGraphic;
class AnchorGraphic;
class TitleGraphic;
class SearchDockWidget;

class MainWindow : public QMainWindow
{
    Q_OBJECT
private:
    Ui::MainWindow *ui;
    CustomTabWidget* tabWidget;
    QVector<NodeGraphic*> clipboardNodes;
    QVector<LinkGraphic*> clipboardLinks;
    SearchDockWidget* searchWidget;
    QProgressBar* progressBar;
    CustomGraphics* temporaryGraphicsForPrint;
    bool openingInProgress;
    bool savingInProgress;
    bool focusInAlertSent;

public:
    MainWindow(const QString &_path, QWidget *parent = 0);
    ~MainWindow();
    template<typename T> QList<T*> loadPluginByDir(const QString& );
    template<typename T> T *pluginByName(const QString &);

    int getTabCount();
    void createMenus();

    void setActivatedUndo(bool _value);
    void setActivatedRedo(bool _value);

    void writeSettings();
    void loadSettings();

    void closeEvent(QCloseEvent *event);
    void dragEnterEvent(QDragEnterEvent *e);
    void dropEvent(QDropEvent *e);

    int fileAlreadyOpened(QString _path);

    void resetTechnicalPrograms();
    void resetPolygons();
    void resetColors(QVector<ColorRuleWidget*> _newRules, const QColor &_newDefaultFileColor, const QColor &_newTechnicalColor, const QColor &_newProgramColor, const QColor &_patternStepColor);
    void resetColorRules();
    void resetPCLRules();

    QVector<NodeGraphic *> getClipboardNodes() const {return clipboardNodes;}
    void setClipboardNodes(const QVector<NodeGraphic *> &value){ clipboardNodes = value; }

    QVector<LinkGraphic *> getClipboardLinks() const { return clipboardLinks; }
    void setClipboardLinks(const QVector<LinkGraphic *> &value) { clipboardLinks = value; }

    QAction* getActionSelectMode();

    CustomGraphics* getGraphics(int _tabID);
    CustomGraphics* getCurrentGraphics();

    void clearClipboard();
    void addToClipboard(NodeGraphic *_node);
    void addToClipboard(LinkGraphic *_link);
    NodeGraphic *getClipboardNode(int _id);

    void resetPositions();
    void resetTechnicalState();

    void setPasteEnabled(bool _value);
    void saveInRecentFiles(const QString &_relativePath);
    ActionRecentFile *getActionRecentFile(const QString &_relativePath);

    SearchDockWidget *getSearchWidget() const { return searchWidget; }
    void setSearchWidget(SearchDockWidget *value) { searchWidget = value; }

    QProgressBar *getProgressBar() const { return progressBar; }
    void setProgressBar(QProgressBar *value) { progressBar = value; }

    CustomTabWidget *getTabWidget() const;
    void setTabWidget(CustomTabWidget *value);

    bool getFocusInAlertSent() const;
    void setFocusInAlertSent(bool value);

    bool getSavingInProgress() const;
    void setSavingInProgress(bool value);



public slots:
    void setStatus(const QString &_status);

    void setSelectMode(bool);
    void restore();
    void align();
    void undo();
    void redo();
    void saveAs();
    void save();
    void save(int _index);
    void saveImage();
    void saveAll();
    void copyImageToClipboard();

    void exportCSV();
    void load();
    void load(const QString &_path);
    bool newFile();
    void toggleButtons(bool _value);

    void branchSelection(bool _toggled);

    void createStep();
    void createCustomNode();
    void createDCLF();
    void createFunction();
    void createCondition();

    void displayAnchors(bool);
    void hideSysins(bool);
    void setTitleOnTop(bool _toggled);
    void hideTitle(bool);

    void displayDDName(bool _toggled);
    void hideConditions(bool _hide);
    void hideClass(bool _toggled);
    void hideStepName(bool _toggled);
    void closeTab(int index);
    void tabChanged(int _index);
    void zoom100pc();
    void zoomWindow();
    void deleteSelection();
    void modifyLinkType(QAction *_action);
    void hideLinkedSchematics(bool _toggled);
    void openSettingsDialog();
    void zoomIn();
    void zoomOut();
    void paste();
    void copy();
    void cut();
    void duplicate();
    void search();
    void print();
    void updateLinks();
    void printPreview(QPrinter * printer);
    void printSelection();
    void printSelectionPreview(QPrinter * printer);

    void reSort();    
    void reSortSelection();
    void modifyColorOfSelection();
    void resetColorOfSelection();
    void versionUpdate();

    void exportPDF();
    void displayGrid(bool _toggled);
    void warnAboutConflict(CustomGraphics *_graphics);

    void openDocumentation();
protected:
    //void changeEvent(QEvent *e);
    void focusInEvent(QFocusEvent *e);

};

#endif // MAINWINDOW_H
