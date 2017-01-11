#include <QToolBar>
#include <QCheckBox>
#include <QLabel>
#include <QFileDialog>
#include <QFileInfo>
#include <QtPrintSupport/QPrintDialog>
#include <QtPrintSupport/QPrintPreviewDialog>

#include "utils.h"
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "writexml.h"
#include "loadxml.h"
#include "savedialog.h"
#include "savebeforequitdialog.h"
#include "customtabwidget.h"
#include "settingsdialog.h"
#include "opennewdialog.h"
#include "actionrecentfile.h"
#include "searchdockwidget.h"
#include "fileplugininterface.h"

CustomTabWidget *MainWindow::getTabWidget() const { return tabWidget; }

void MainWindow::setTabWidget(CustomTabWidget *value) { tabWidget = value; }

bool MainWindow::getFocusInAlertSent() const { return focusInAlertSent; }

void MainWindow::setFocusInAlertSent(bool value) { focusInAlertSent = value; }

bool MainWindow::getSavingInProgress() const { return savingInProgress; }

void MainWindow::setSavingInProgress(bool value) { savingInProgress = value; }

MainWindow::MainWindow(const QString &_path, QWidget *parent) :
    QMainWindow(parent),
    openingInProgress(false),
    savingInProgress(false),
    focusInAlertSent(false),
    ui(new Ui::MainWindow)
{
    //qDebug()<<"MainWindow : constructeur";
    ui->setupUi(this);
    searchWidget = new SearchDockWidget(this);
    addDockWidget(Qt::LeftDockWidgetArea, searchWidget);
    searchWidget->setVisible(false);

    tabWidget = new CustomTabWidget(this);
    connect(tabWidget, SIGNAL(tabCloseRequested(int)), this, SLOT(closeTab(int)));
    connect(tabWidget,SIGNAL(currentChanged(int)), this, SLOT(tabChanged(int)));

    ui->menuFichiers_ouverts_r_cemment->setEnabled(false);

    QIcon icon("../PCLTranslator/Icons/bricks.ico");
    setWindowIcon(icon);
    setWindowTitle(BASE_NAME);

    ui->verticalLayout->addWidget(tabWidget);
    createMenus();

    showMaximized();

    toggleButtons(false);
    setPasteEnabled(false);
    setActivatedUndo(false);
    setActivatedRedo(false);

    loadSettings();

    progressBar = new QProgressBar(this);
    ui->statusBar->addPermanentWidget(progressBar);
    progressBar->setVisible(false);

    if(!_path.isEmpty()) load(_path);
    writeSettings(); //pour sauvegarde fichiers récents

    setAcceptDrops(true);

//    QList<FilePlugin*> plugins = loadPluginByDir<FilePlugin>(qApp->applicationDirPath() + "/plugins/");
//    foreach(FilePlugin* plugin, plugins)
//    {
//        QAction* newPluginAction = ui->menuTester_le_plugin->addAction(plugin->getExtension());
//        connect(newPluginAction, SIGNAL(triggered()), this, SLOT(createDCLF()));

//    }
}

MainWindow::~MainWindow() { delete ui; }

int MainWindow::getTabCount() { return tabWidget->count(); }

void MainWindow::dragEnterEvent(QDragEnterEvent *e){ if (e->mimeData()->hasUrls()) e->acceptProposedAction(); }

void MainWindow::dropEvent(QDropEvent *e)
{
    foreach (const QUrl &url, e->mimeData()->urls()) load(url.toLocalFile());
    writeSettings(); // pour sauvegarde fichiers récents / courants
}

void MainWindow::resetPolygons()
{
    for(int i=0; i<tabWidget->count(); i++)
    {
        foreach (NodeGraphic* node, getGraphics(i)->getNodeGraphics())
        {
            if(node->getSysinGraphic()) node->getSysinGraphic()->resetPolygon();
            if(node->getCondition()) node->getCondition()->resetPolygon();
        }
    }
}

void MainWindow::resetPositions()
{
    for(int i=0; i<tabWidget->count(); i++)
    {
        foreach (NodeGraphic* node, getGraphics(i)->getNodeGraphics())
        {
            if(node->getXPos()!=-1) node->setXPos(node->getXPos());
            if(node->getYPos()!=-1) node->setYPos(node->getYPos());
        }
    }
}

void MainWindow::resetTechnicalState()
{
    for(int i=0; i<tabWidget->count(); i++)
    {
        foreach (NodeGraphic* node, getGraphics(i)->getNodeGraphics())
        {
            if(node->getType()=="STEP")
            {
                bool defaultColorBefore = (node->getDefaultColor() == node->getColorSaved());
                ((StepGraphic*)node)->setIsTechnical(TECHNICAL_LIST.contains(node->getProgram()));
                if(defaultColorBefore)
                {
                    node->setColorSaved(node->getDefaultColor());
                }
            }
        }
    }
}

void MainWindow::resetColors(QVector<ColorRuleWidget*> _newRules, const QColor &_newDefaultFileColor, const QColor &_newTechnicalColor, const QColor &_newProgramColor, const QColor &_patternStepColor)
{
    //qDebug()<<"resetColors";
    for(int i=0; i<tabWidget->count(); i++)
    {
        foreach (NodeGraphic* node, getGraphics(i)->getNodeGraphics())
        {
            if(node->getType()=="DCLF")
            {
                QString classBeforeSeparator = ((DCLFGraphic*)node)->getClass();
                if(!CLASS_SEPARATOR.isEmpty()) classBeforeSeparator = classBeforeSeparator.split(CLASS_SEPARATOR)[0];
                QColor oldRuleColor = DCLFGraphic::getColorFromRules(classBeforeSeparator);
                QColor newRuleColor = _newDefaultFileColor;
                for(int i=0; i<_newRules.size(); i++)
                {
                    if(_newRules[i]->getClasse()==classBeforeSeparator) newRuleColor = _newRules[i]->getColor();
                }

                if(node->getColorSaved() == oldRuleColor)
                {
                    node->setColorSaved(newRuleColor);
                    node->setColor(newRuleColor);
                }
            }
            else if(node->getType()=="STEP")
            {
                if(node->getIsTechnical() && node->getColorSaved() == TECHNICAL_STEP_COLOR)
                {
                    node->setColorSaved(_newTechnicalColor);
                    node->setColor(_newTechnicalColor);
                }
                else if(node->getSubtype()=="PATTERN" && node->getColorSaved() == PATTERN_STEP_COLOR)
                {
                    node->setColorSaved(_patternStepColor);
                    node->setColor(_patternStepColor);
                }
                else if(!node->getIsTechnical() && node->getColorSaved() == NORMAL_STEP_COLOR)
                {
                    node->setColorSaved(_newProgramColor);
                    node->setColor(_newProgramColor);
                }
            }
        }
    }
}

void MainWindow::createMenus()
{
    //Menu fichier
    connect(ui->actionNouveau, SIGNAL(triggered()), this, SLOT(newFile()));
    ui->actionNouveau->setShortcut(QKeySequence("Ctrl+N"));
    ui->actionNouveau->setIcon(QIcon(QFileInfo(EXE_PATH,"Icons/new.png").absoluteFilePath()));

    connect(ui->actionOuvrir_fichier_PCL_ou_XML, SIGNAL(triggered()), this, SLOT(load()));
    ui->actionOuvrir_fichier_PCL_ou_XML->setShortcut(QKeySequence("Ctrl+O"));
    ui->actionOuvrir_fichier_PCL_ou_XML->setIcon(QIcon(QFileInfo(EXE_PATH,"Icons/open.png").absoluteFilePath()));

    connect(ui->actionSauvegarder_XML, SIGNAL(triggered()), this, SLOT(save()));
    ui->actionSauvegarder_XML->setIcon(QIcon(QFileInfo(EXE_PATH,"Icons/save.png").absoluteFilePath()));
    ui->actionSauvegarder_XML->setShortcut(QKeySequence("Ctrl+S"));

    connect(ui->actionSauvegarder_XML_sous, SIGNAL(triggered()), this, SLOT(saveAs()));
    ui->actionSauvegarder_XML_sous->setIcon(QIcon(QFileInfo(EXE_PATH,"Icons/saveas.png").absoluteFilePath()));
    ui->actionSauvegarder_XML_sous->setShortcut(QKeySequence("Ctrl+Shift+S"));

    connect(ui->actionSauvegarder_tous_les_DDC_ouverts, SIGNAL(triggered()), this, SLOT(saveAll()));
    ui->actionSauvegarder_tous_les_DDC_ouverts->setShortcut(QKeySequence("Ctrl+Alt+S"));

    connect(ui->actionSauvegarder_Image, SIGNAL(triggered()), this, SLOT(saveImage()));
    ui->actionSauvegarder_Image->setIcon(QIcon(QFileInfo(EXE_PATH,"Icons/image.png").absoluteFilePath()));

    connect(ui->actionCopier_l_image_dans_le_presse_papier, SIGNAL(triggered()), this, SLOT(copyImageToClipboard()));

    connect(ui->actionExport_en_CSV, SIGNAL(triggered()), this, SLOT(exportCSV()));
    ui->actionExport_en_CSV->setShortcut(QKeySequence("Ctrl+E"));

    connect(ui->actionRestaurer_le_dernier_fichier_modifi, SIGNAL(triggered()), this, SLOT(restore()));
    connect(ui->actionQuitter, SIGNAL(triggered()), qApp, SLOT(quit()));

    connect(ui->actionMont_e_de_version, SIGNAL(triggered()), this, SLOT(versionUpdate()));

    connect(ui->actionImprimer,SIGNAL(triggered()),this, SLOT(print()));
    ui->actionImprimer->setIcon(QIcon(QFileInfo(EXE_PATH,"Icons/print.png").absoluteFilePath()));
    ui->actionImprimer->setShortcut(QKeySequence("Ctrl+I"));

    connect(ui->actionImprimer_la_s_lection,SIGNAL(triggered()),this, SLOT(printSelection()));

    connect(ui->actionExporter_au_format_PDF,SIGNAL(triggered()),this, SLOT(exportPDF()));

    //Menu édition
    connect(ui->actionRechercher, SIGNAL(triggered()), this, SLOT(search()));
    ui->actionRechercher->setShortcut(QKeySequence("Ctrl+F"));

    connect(ui->actionAnnuler, SIGNAL(triggered()), this, SLOT(undo()));
    ui->actionAnnuler->setIcon(QIcon(QFileInfo(EXE_PATH,"Icons/undo.png").absoluteFilePath()));
    ui->actionAnnuler->setShortcut(QKeySequence("Ctrl+Z"));

    connect(ui->actionR_tablir, SIGNAL(triggered()), this, SLOT(redo()));
    ui->actionR_tablir->setIcon(QIcon(QFileInfo(EXE_PATH,"Icons/redo.png").absoluteFilePath()));
    ui->actionR_tablir->setShortcut(QKeySequence("Ctrl+Shift+Z"));

    connect(ui->actionAligner, SIGNAL(triggered()), this, SLOT(align()));
    ui->actionAligner->setIcon(QIcon(QFileInfo(EXE_PATH,"Icons/align.png").absoluteFilePath()));
    ui->actionAligner->setShortcut(QKeySequence("Ctrl+A"));

    connect(ui->actionCopier_dans_le_presse_papier, SIGNAL(triggered()), this, SLOT(copy()));
    ui->actionCopier_dans_le_presse_papier->setShortcut(QKeySequence("Ctrl+C"));

    connect(ui->actionCouper_dans_le_presse_papier, SIGNAL(triggered()), this, SLOT(cut()));
    ui->actionCouper_dans_le_presse_papier->setShortcut(QKeySequence("Ctrl+X"));

    connect(ui->actionDupliquer_la_s_lection, SIGNAL(triggered()), this, SLOT(duplicate()));
    ui->actionDupliquer_la_s_lection->setShortcut(QKeySequence("Ctrl+D"));

    connect(ui->actionColler_le_contenu_du_presse_papier, SIGNAL(triggered()), this, SLOT(paste()));
    ui->actionColler_le_contenu_du_presse_papier->setShortcut(QKeySequence("Ctrl+V"));

    connect(ui->actionAjouter_un_Step, SIGNAL(triggered()), this, SLOT(createStep()));
    ui->actionAjouter_un_Step->setIcon(QIcon(QFileInfo(EXE_PATH,"Icons/step.png").absoluteFilePath()));

    connect(ui->actionAjouter_une_fonction_pacbase, SIGNAL(triggered()), this, SLOT(createFunction()));

    connect(ui->actionAjouter_un_noeud_condition, SIGNAL(triggered()), this, SLOT(createCondition()));

    connect(ui->actionAjouter_un_fichier, SIGNAL(triggered()), this, SLOT(createDCLF()));
    ui->actionAjouter_un_fichier->setIcon(QIcon(QFileInfo(EXE_PATH,"Icons/DCLF.png").absoluteFilePath()));

    connect(ui->actionAjouter_un_noeud_personnalis, SIGNAL(triggered()), this, SLOT(createCustomNode()));
    ui->actionAjouter_un_noeud_personnalis->setIcon(QIcon(QFileInfo(EXE_PATH,"Icons/custom.png").absoluteFilePath()));

    connect(ui->actionPr_f_rences, SIGNAL(triggered()), this, SLOT(openSettingsDialog()));
    ui->actionPr_f_rences->setIcon(QIcon(QFileInfo(EXE_PATH,"Icons/settings.png").absoluteFilePath()));
    ui->actionPr_f_rences->setShortcut(QKeySequence("P"));

    connect(ui->actionTrier, SIGNAL(triggered()), this, SLOT(reSort()));
    connect(ui->actionTrier_la_s_l_ction, SIGNAL(triggered()), this, SLOT(reSortSelection()));

    connect(ui->actionMise_jour_des_liens, SIGNAL(triggered()),this, SLOT(updateLinks()));
    ui->actionMise_jour_des_liens->setShortcut(QKeySequence("Ctrl+M"));

    //Menu Affichage
    ui->actionZoom_100->setIcon(QIcon(QFileInfo(EXE_PATH,"Icons/zoom1.png").absoluteFilePath()));
    connect(ui->actionZoom_100, SIGNAL(triggered()), this, SLOT(zoom100pc()));

    ui->actionZoomIn->setShortcut(QKeySequence("+"));
    connect(ui->actionZoomIn, SIGNAL(triggered()), this, SLOT(zoomIn()));

    ui->actionZoomOut->setShortcut(QKeySequence("-"));
    connect(ui->actionZoomOut, SIGNAL(triggered()), this, SLOT(zoomOut()));

    ui->actionZoom_fen_tre->setIcon(QIcon(QFileInfo(EXE_PATH,"Icons/zoomwin.png").absoluteFilePath()));
    connect(ui->actionZoom_fen_tre, SIGNAL(triggered()), this, SLOT(zoomWindow()));

    ui->actionAfficher_les_ancres->setCheckable(true);
    ui->actionAfficher_les_ancres->setShortcut(QKeySequence("L"));
    ui->actionAfficher_les_ancres->setIcon(QIcon(QFileInfo(EXE_PATH,"Icons/anchors.png").absoluteFilePath()));
    connect(ui->actionAfficher_les_ancres, SIGNAL(toggled(bool)), this, SLOT(displayAnchors(bool)));

    ui->actionAfficher_les_DDName_sur_les_liens->setCheckable(true);
    ui->actionAfficher_les_DDName_sur_les_liens->setShortcut(QKeySequence("D"));
    connect(ui->actionAfficher_les_DDName_sur_les_liens, SIGNAL(toggled(bool)), this, SLOT(displayDDName(bool)));

    ui->actionCacher_la_classe_des_DCLF->setCheckable(true);
    connect(ui->actionCacher_la_classe_des_DCLF, SIGNAL(toggled(bool)), this, SLOT(hideClass(bool)));

    ui->actionCacher_le_nom_des_steps->setCheckable(true);
    connect(ui->actionCacher_le_nom_des_steps, SIGNAL(toggled(bool)), this, SLOT(hideStepName(bool)));

    ui->actionCacher_les_SYSIN->setCheckable(true);
    connect(ui->actionCacher_les_SYSIN, SIGNAL(toggled(bool)), this, SLOT(hideSysins(bool)));

    ui->actionCacher_les_conditions->setCheckable(true);
    connect(ui->actionCacher_les_conditions, SIGNAL(toggled(bool)), this, SLOT(hideConditions(bool)));

    ui->actionCacher_le_titre->setCheckable(true);
    connect(ui->actionCacher_le_titre, SIGNAL(toggled(bool)), this, SLOT(hideTitle(bool)));

    ui->actionCacher_les_liens_entre_noeuds->setCheckable(true);
    connect(ui->actionCacher_les_liens_entre_noeuds, SIGNAL(toggled(bool)), this, SLOT(hideLinkedSchematics(bool)));

    ui->actionMettre_le_titre_en_haut->setCheckable(true);
    connect(ui->actionMettre_le_titre_en_haut, SIGNAL(toggled(bool)), this, SLOT(setTitleOnTop(bool)));

    ui->actionDirects->setCheckable(true);
    ui->actionCourbes->setCheckable(true);

    QActionGroup* typesDeLiens = new QActionGroup(this);
    typesDeLiens->addAction(ui->actionCourbes);
    ui->actionCourbes->setCheckable(true);
    typesDeLiens->addAction(ui->actionDirects);
    ui->actionDirects->setCheckable(true);
    ui->actionDirects->setChecked(true);
    typesDeLiens->addAction(ui->actionLignes_droites);
    ui->actionLignes_droites->setCheckable(true);
    connect(typesDeLiens, SIGNAL(triggered(QAction*)), this, SLOT(modifyLinkType(QAction*)));

    ui->actionAfficher_la_grille->setCheckable(true);
    connect(ui->actionAfficher_la_grille,SIGNAL(toggled(bool)),this, SLOT(displayGrid(bool)));
    ui->actionAfficher_la_grille->setShortcut(QKeySequence("G"));

    //Menu Sélection
    ui->actionMode_s_lection->setCheckable(true);
    ui->actionMode_s_lection->setIcon(QIcon(QFileInfo(EXE_PATH,"Icons/selection.png").absoluteFilePath()));
    ui->actionMode_s_lection->setShortcut(QKeySequence("S"));
    connect(ui->actionMode_s_lection, SIGNAL(toggled(bool)), this, SLOT(setSelectMode(bool)));

    ui->actionActiver_la_s_lection_par_branche->setCheckable(true);
    connect(ui->actionActiver_la_s_lection_par_branche, SIGNAL(toggled(bool)), this, SLOT(branchSelection(bool)));

    ui->actionSupprimer_la_s_lection->setShortcut(QKeySequence(QKeySequence::Delete));
    connect(ui->actionSupprimer_la_s_lection, SIGNAL(triggered()), this, SLOT(deleteSelection()));

    connect(ui->actionTrier_la_s_lection, SIGNAL(triggered()), this, SLOT(reSortSelection()));
    connect(ui->actionModifier_la_couleur_des_noeuds_s_lectionn_s, SIGNAL(triggered()), this, SLOT(modifyColorOfSelection()));
    connect(ui->actionR_initialiser_la_couleur_des_noeuds_s_lectionn_s, SIGNAL(triggered()), this, SLOT(resetColorOfSelection()));

    // Menu Aide
    connect(ui->actionOuvrir_la_documentation, SIGNAL(triggered()), this, SLOT(openDocumentation()));


    //Barres d'outils
    QToolBar *fileToolBar = addToolBar("File");
    fileToolBar->addAction(ui->actionNouveau);
    fileToolBar->addAction(ui->actionOuvrir_fichier_PCL_ou_XML);
    fileToolBar->addAction(ui->actionSauvegarder_XML);
    fileToolBar->addAction(ui->actionSauvegarder_XML_sous);
    fileToolBar->addAction(ui->actionSauvegarder_Image);
    fileToolBar->addAction(ui->actionImprimer);

    QToolBar *editToolBar = addToolBar("Edit");
    editToolBar->addAction(ui->actionAnnuler);
    editToolBar->addAction(ui->actionR_tablir);
    editToolBar->addAction(ui->actionAligner);
    editToolBar->addAction(ui->actionAjouter_un_Step);
    editToolBar->addAction(ui->actionAjouter_un_fichier);
    editToolBar->addAction(ui->actionAjouter_un_noeud_personnalis);
    editToolBar->addAction(ui->actionMode_s_lection);
    editToolBar->addAction(ui->actionAfficher_les_ancres);
    editToolBar->addAction(ui->actionZoom_100);
    editToolBar->addAction(ui->actionZoom_fen_tre);
    editToolBar->addAction(ui->actionPr_f_rences);
}

void MainWindow::setActivatedUndo(bool _value){ ui->actionAnnuler->setDisabled(!_value); }
void MainWindow::setActivatedRedo(bool _value){ ui->actionR_tablir->setDisabled(!_value); }

void MainWindow::openSettingsDialog()
{
    SettingsDialog* settingsDialog =  new SettingsDialog(this);
    settingsDialog->exec();
}

void MainWindow::hideSysins(bool _hide)
{
    for(int i=0; i<tabWidget->count(); i++) getGraphics(i)->hideSysins(_hide);
}

void MainWindow::deleteSelection()
{
    if(tabWidget->currentIndex() == -1) return;
    getCurrentGraphics()->deleteSelectedNodes();
    searchWidget->search();
}

void MainWindow::hideConditions(bool _hide)
{
    for(int i=0; i<tabWidget->count(); i++) getGraphics(i)->hideConditions(_hide);
}

void MainWindow::hideLinkedSchematics(bool _toggled)
{
    for(int i=0; i<tabWidget->count(); i++) getGraphics(i)->hideLinkedSchematics(_toggled);
}

void MainWindow::hideTitle(bool _toggled)
{    
    for(int i=0; i<tabWidget->count(); i++) getGraphics(i)->hideTitle(_toggled);
}

void MainWindow::hideClass(bool _toggled)
{
    if(tabWidget->currentIndex() == -1) return;
    getCurrentGraphics()->hideClass(_toggled);
}

void MainWindow::displayGrid(bool _toggled)
{
    DISPLAY_GRID = _toggled;
    update();
}

void MainWindow::hideStepName(bool _toggled)
{
    if(tabWidget->currentIndex() == -1) return;
    getCurrentGraphics()->hideStepName(_toggled);
}

void MainWindow::setTitleOnTop(bool _toggled)
{
    if(tabWidget->currentIndex() == -1) return;
    getCurrentGraphics()->setTitleOnTop(_toggled);
}

void MainWindow::branchSelection(bool _toggled)
{
    if(tabWidget->currentIndex() == -1) return;
    getCurrentGraphics()->branchSelection(_toggled);
}

void MainWindow::displayDDName(bool _toggled)
{
    if(tabWidget->currentIndex() == -1) return;
    getCurrentGraphics()->displayDDName(_toggled);
}

void MainWindow::createStep()
{
    if((tabWidget->currentIndex() == -1 && newFile()) || tabWidget->currentIndex() != -1) getCurrentGraphics()->createStep();
}

void MainWindow::createFunction()
{
    if((tabWidget->currentIndex() == -1 && newFile()) || tabWidget->currentIndex() != -1) getCurrentGraphics()->createFunction();
}

void MainWindow::createCondition()
{
    if((tabWidget->currentIndex() == -1 && newFile()) || tabWidget->currentIndex() != -1) getCurrentGraphics()->createCondition();
}

void MainWindow::createCustomNode()
{
    if((tabWidget->currentIndex() == -1 && newFile()) || tabWidget->currentIndex() != -1) getCurrentGraphics()->createCustomNode();
}

void MainWindow::createDCLF()
{
    if((tabWidget->currentIndex() == -1 && newFile()) || tabWidget->currentIndex() != -1) getCurrentGraphics()->createDCLF();
}

void MainWindow::displayAnchors(bool _display)
{
    for(int i=0; i<tabWidget->count(); i++)
    {
        getGraphics(i)->displayAnchors(_display);
    }
}

void MainWindow::reSort()
{
    if(tabWidget->currentIndex() == -1) return;
    getCurrentGraphics()->saveTemporaryFile("Application de l'algorithme de tri");
    getCurrentGraphics()->reSort();
}

void MainWindow::reSortSelection()
{
    qDebug()<<"resortSelection";

    ///Pour trier la sélection, on copie la sélection dans une autre scene et on les y trie, une fois fait on redonne les places
    if(tabWidget->currentIndex() == -1) return;
    CustomGraphics* currentGraphics = getCurrentGraphics();
    currentGraphics->saveTemporaryFile("Application de l'algorithme de tri sur la sélection");

    ///Création de la scène temporaire
    CustomGraphics* temporaryGraphics =  new CustomGraphics(this);

    ///Récupérer les noeuds et lien séléctionnés
    QVector<NodeGraphic*> selectedNodes = currentGraphics->getSelectedNodes();
    QVector<LinkGraphic*> selectedLinks = currentGraphics->getSelectedLinks();

    ///Les ajouter à la scène temporaire
    foreach(NodeGraphic* node, selectedNodes) temporaryGraphics->addNode(node);
    foreach(LinkGraphic* link, selectedLinks) temporaryGraphics->addLink(link);

    ///On les trie dans la scène temporaire (pointeurs => la nouvelle position reste effective en changeant de scène)
    temporaryGraphics->reSort();

    ///On les translate pour les placer à droite
    QVector<float> boundaries = currentGraphics->getBoundaries(true);
    temporaryGraphics->translateNodes(boundaries[0]+boundaries[2]+ALIGN_GRID_X, boundaries[1]+DEFAULT_HEIGHT/2);

    ///On remet tout dans la scène courante
    foreach(NodeGraphic* node, selectedNodes) currentGraphics->addToScene(node);
    foreach(LinkGraphic* link, selectedLinks) currentGraphics->addToScene(link);

    //nettoyage de la scene sans suppression des noeuds
    temporaryGraphics->cleanScene(false);
    delete temporaryGraphics;
    qDebug()<<"resortSelection fin";

    zoomWindow();
}

void MainWindow::openDocumentation()
{
    QFileInfo fileInfo(EXE_PATH, "Documentation.pdf");
    if(fileInfo.exists() && fileInfo.isFile())
    {
        QDesktopServices::openUrl(QUrl(fileInfo.absoluteFilePath()));
    }
    else
    {
        QMessageBox::warning(0,"Erreur à l'ouverture de la documentation",fileInfo.absoluteFilePath()+ " non trouvé");
    }
}

void MainWindow::writeSettings()
{
    //qDebug()<<"writesettings()";
    QSettings settings(QFileInfo(EXE_PATH,"settings.ini").absoluteFilePath(),QSettings::IniFormat, this);

    settings.beginGroup("Int");
    settings.setValue("TITLE_SIZE", TITLE_SIZE);
    settings.setValue("TITLE_MARGIN", TITLE_MARGIN);
    settings.setValue("COPY_OFFSET", COPY_OFFSET);
    settings.setValue("ANCHOR_SIZE", ANCHOR_SIZE);
    settings.setValue("SYSIN_SIZE", SYSIN_SIZE);
    settings.setValue("CONDITION_SIZE", CONDITION_SIZE);
    settings.setValue("SCENE_MARGIN", SCENE_MARGIN);
    settings.setValue("ALIGN_GRID_X", ALIGN_GRID_X);
    settings.setValue("ALIGN_GRID_X_DIVISION", ALIGN_GRID_X_DIVISION);
    settings.setValue("ALIGN_GRID_Y_DIVISION", ALIGN_GRID_Y_DIVISION);
    settings.setValue("ALIGN_GRID_Y", ALIGN_GRID_Y);
    settings.setValue("SHADOW_OFFSET", SHADOW_OFFSET);
    settings.setValue("FONT_SIZE", FONT_SIZE);
    settings.setValue("DDNAME_FONT_SIZE", DDNAME_FONT_SIZE);
    settings.setValue("TECHNICAL_FONT_SIZE", TECHNICAL_FONT_SIZE);
    settings.setValue("DEFAULT_HEIGHT", DEFAULT_HEIGHT);
    settings.setValue("DEFAULT_WIDTH", DEFAULT_WIDTH);
    settings.setValue("DETAILED_WEIGHT", DETAILED_WEIGHT);
    settings.setValue("LINK_TYPE", LINK_TYPE);
    settings.setValue("INSIDE_MARGIN", INSIDE_MARGIN);
    settings.setValue("RESIZE_STEP", RESIZE_STEP);
    settings.endGroup();

    settings.beginGroup("Double");
    settings.setValue("SCALE_CHANGE", SCALE_CHANGE);
    settings.endGroup();

    settings.beginGroup("Color");
    settings.setValue("TECHNICAL_STEP_COLOR", TECHNICAL_STEP_COLOR.name());
    settings.setValue("NORMAL_STEP_COLOR", NORMAL_STEP_COLOR.name());
    settings.setValue("PATTERN_STEP_COLOR", PATTERN_STEP_COLOR.name());
    settings.setValue("NORMAL_FILE_COLOR", NORMAL_FILE_COLOR.name());
    settings.setValue("SELECTED_COLOR", SELECTED_COLOR.name());
    settings.setValue("FILE_HOVER_COLOR", FILE_HOVER_COLOR.name());

    settings.remove("COLOR_RULES");
    settings.beginWriteArray("COLOR_RULES");
    for(int i = 0; i < COLOR_RULES.size(); i++)
    {
        settings.setArrayIndex(i);
        settings.setValue("CLASS", COLOR_RULES.at(i).classe);
        settings.setValue("COLOR", COLOR_RULES.at(i).color.name());
    }
    settings.endArray();
    settings.endGroup();

    settings.beginGroup("Rules");
    settings.remove("PCL_RULES");
    settings.beginWriteArray("PCL_RULES");
    for(int i = 0; i < PCL_RULES.size(); i++)
    {
        settings.setArrayIndex(i);
        settings.setValue("PROGRAM", PCL_RULES.at(i).program);
        settings.setValue("PARAMETER", PCL_RULES.at(i).param);
        settings.setValue("SUBPARAMETER", PCL_RULES.at(i).subparam);
        settings.setValue("CLASSE", PCL_RULES.at(i).classe);
        settings.setValue("INOUT", PCL_RULES.at(i).outputFile);
    }
    settings.endArray();
    settings.endGroup();

    settings.beginGroup("RecentFiles");
    settings.remove("RECENT_FILES");
    settings.beginWriteArray("RECENT_FILES");
    for(int i = 0; i <RECENT_FILES.size(); i++)
    {
        settings.setArrayIndex(i);
        settings.setValue("PATH", RECENT_FILES.at(RECENT_FILES.size()-i-1)->getRelativePath());
    }
    settings.endArray();
    settings.endGroup();

    settings.beginGroup("CurrentFiles");
    settings.remove("CURRENT_FILES");
    settings.setValue("CURRENT_INDEX", tabWidget->currentIndex());
    settings.beginWriteArray("CURRENT_FILES");
    if(tabWidget->currentIndex() != -1)
    {
        for(int i = 0; i <tabWidget->count(); i++)
        {
            settings.setArrayIndex(i);
            settings.setValue("PATH", getGraphics(i)->getCurrentFilename());
        }
    }
    settings.endArray();
    settings.endGroup();

    settings.beginGroup("Bool");
    settings.setValue("DISPLAY_STEPNAME", DISPLAY_STEPNAME);
    settings.setValue("DISPLAY_DCLFCLASS", DISPLAY_DCLFCLASS);
    settings.setValue("CONDITIONS_DISPLAYED", CONDITIONS_DISPLAYED);
    settings.setValue("ANCHORS_DISPLAYED", ANCHORS_DISPLAYED);
    settings.setValue("DISPLAY_DDNAME", DISPLAY_DDNAME);
    settings.setValue("SYSIN_HIDDEN", SYSIN_HIDDEN);
    settings.setValue("SELECT_MODE", SELECT_MODE);
    settings.setValue("SELECT_BRANCH", SELECT_BRANCH);
    settings.setValue("HIDE_TITLE", HIDE_TITLE);
    settings.setValue("DISPLAY_SCHEMATICS", DISPLAY_SCHEMATICS);
    settings.setValue("TITLE_ON_TOP", TITLE_ON_TOP);
    settings.endGroup();

    settings.beginGroup("String");
    settings.setValue("LOAD_PATH", LOAD_PATH);
    settings.setValue("WRITE_PATH", WRITE_PATH);
    settings.setValue("WRITE_IMAGE_PATH", WRITE_IMAGE_PATH);
    settings.setValue("WORKSPACE", WORKSPACE);
    settings.setValue("CLASS_SEPARATOR", CLASS_SEPARATOR);
    settings.remove("TECHNICAL_LIST");
    settings.beginWriteArray("TECHNICAL_LIST");
    for(int i = 0; i < TECHNICAL_LIST.size(); i++)
    {
        settings.setArrayIndex(i);
        settings.setValue("TECHNICAL_PROGRAM", TECHNICAL_LIST.at(i));
    }
    settings.endArray();
    settings.endGroup();
}

void MainWindow::loadSettings()
{
    //qDebug()<<"loadSetting()";
    QSettings settings(QFileInfo(EXE_PATH,"settings.ini").absoluteFilePath(),QSettings::IniFormat, this);

    settings.beginGroup("Int");
    TITLE_SIZE = settings.value("TITLE_SIZE", 40).toInt();
    TITLE_MARGIN = settings.value("TITLE_MARGIN", 40).toInt();
    COPY_OFFSET = settings.value("COPY_OFFSET", 25).toInt();
    ANCHOR_SIZE = settings.value("ANCHOR_SIZE", 12).toInt();
    SYSIN_SIZE = settings.value("SYSIN_SIZE", 4).toInt();
    CONDITION_SIZE = settings.value("CONDITION_SIZE", 15).toInt();
    SCENE_MARGIN = settings.value("SCENE_MARGIN", 50).toInt();
    ALIGN_GRID_X = settings.value("ALIGN_GRID_X", 150).toInt();
    ALIGN_GRID_X_DIVISION = settings.value("ALIGN_GRID_X_DIVISION", 4).toInt();
    ALIGN_GRID_Y_DIVISION = settings.value("ALIGN_GRID_Y_DIVISION", 2).toInt();
    ALIGN_GRID_Y = settings.value("ALIGN_GRID_Y", 115).toInt();
    SHADOW_OFFSET = settings.value("SHADOW_OFFSET", 5).toInt();
    FONT_SIZE = settings.value("FONT_SIZE", 12).toInt();
    DDNAME_FONT_SIZE = settings.value("DDNAME_FONT_SIZE", 10).toInt();
    TECHNICAL_FONT_SIZE = settings.value("TECHNICAL_FONT_SIZE", 12).toInt();
    DEFAULT_HEIGHT = settings.value("DEFAULT_HEIGHT", 50).toInt();
    DEFAULT_WIDTH = settings.value("DEFAULT_WIDTH", 100).toInt();
    DETAILED_WEIGHT = settings.value("DETAILED_WEIGHT", 3).toInt();
    LINK_TYPE = settings.value("LINK_TYPE",0).toInt();
    INSIDE_MARGIN = settings.value("INSIDE_MARGIN",5).toInt();
    RESIZE_STEP = settings.value("RESIZE_STEP",5).toInt();

    ui->actionDirects->setChecked(true);
    if(LINK_TYPE==MyShapes::CurvedLine) ui->actionCourbes->setChecked(true);
    else if (LINK_TYPE==MyShapes::BrokenLine) ui->actionLignes_droites->setChecked(true);
    settings.endGroup();

    settings.beginGroup("Double");
    SCALE_CHANGE = settings.value("SCALE_CHANGE", 1.15).toDouble();
    settings.endGroup();

    settings.beginGroup("Color");
    TECHNICAL_STEP_COLOR = settings.value("TECHNICAL_STEP_COLOR", QColor(90,255,60).name()).value<QColor>();
    NORMAL_STEP_COLOR = settings.value("NORMAL_STEP_COLOR", QColor(120,255,255).name()).value<QColor>();
    NORMAL_FILE_COLOR = settings.value("NORMAL_FILE_COLOR", QColor(255,255,100).name()).value<QColor>();
    PATTERN_STEP_COLOR = settings.value("PATTERN_STEP_COLOR", QColor(255,220,100).name()).value<QColor>();
    SELECTED_COLOR = settings.value("SELECTED_COLOR", QColor(Qt::yellow).name()).value<QColor>();
    FILE_HOVER_COLOR = settings.value("FILE_HOVER_COLOR", QColor(Qt::red).name()).value<QColor>();

    int sizeColorRules = settings.beginReadArray("COLOR_RULES");
    if(!sizeColorRules) resetColorRules();
    else
    {
        for(int i = 0; i < sizeColorRules; ++i)
        {
            settings.setArrayIndex(i);
            COLOR_RULES.push_back(ColorRule(settings.value("CLASS").toString(),QColor(settings.value("COLOR").toString())));
        }
    }
    settings.endArray();
    settings.endGroup();

    settings.beginGroup("Rules");
    int sizePCLRules = settings.beginReadArray("PCL_RULES");
    if(!sizePCLRules) resetPCLRules();
    else
    {
        for(int i = 0; i < sizePCLRules; ++i)
        {
            settings.setArrayIndex(i);
            PCL_RULES.push_front(PCLRule(settings.value("PROGRAM").toString(),settings.value("PARAMETER").toString(),settings.value("SUBPARAMETER").toString(),settings.value("CLASSE").toString(),settings.value("INOUT").toBool()));
        }
    }
    settings.endArray();
    settings.endGroup();

    settings.beginGroup("Bool");
    DISPLAY_STEPNAME = settings.value("DISPLAY_STEPNAME", true).toBool();
    ui->actionCacher_le_nom_des_steps->setChecked(!DISPLAY_STEPNAME);
    DISPLAY_DCLFCLASS = settings.value("DISPLAY_DCLFCLASS", true).toBool();
    ui->actionCacher_la_classe_des_DCLF->setChecked(!DISPLAY_DCLFCLASS);
    CONDITIONS_DISPLAYED = settings.value("CONDITIONS_DISPLAYED", true).toBool();
    ui->actionCacher_les_conditions->setChecked(!CONDITIONS_DISPLAYED);
    ANCHORS_DISPLAYED = settings.value("ANCHORS_DISPLAYED", false).toBool();
    ui->actionAfficher_les_ancres->setChecked(ANCHORS_DISPLAYED);
    DISPLAY_DDNAME = settings.value("DISPLAY_DDNAME", false).toBool();
    ui->actionAfficher_les_DDName_sur_les_liens->setChecked(DISPLAY_DDNAME);
    SYSIN_HIDDEN = settings.value("SYSIN_HIDDEN", false).toBool();
    ui->actionCacher_les_SYSIN->setChecked(SYSIN_HIDDEN);
    SELECT_MODE = settings.value("SELECT_MODE", false).toBool();
    ui->actionMode_s_lection->setChecked(SELECT_MODE);
    SELECT_BRANCH = settings.value("SELECT_BRANCH", false).toBool();
    ui->actionActiver_la_s_lection_par_branche->setChecked(SELECT_BRANCH);
    HIDE_TITLE = settings.value("HIDE_TITLE", false).toBool();
    ui->actionCacher_le_titre->setChecked(HIDE_TITLE);
    DISPLAY_SCHEMATICS = settings.value("DISPLAY_SCHEMATICS", true).toBool();
    ui->actionCacher_les_liens_entre_noeuds->setChecked(!DISPLAY_SCHEMATICS);
    TITLE_ON_TOP = settings.value("TITLE_ON_TOP", false).toBool();
    ui->actionMettre_le_titre_en_haut->setChecked(TITLE_ON_TOP);
    settings.endGroup();

    settings.beginGroup("String");
    LOAD_PATH = settings.value("LOAD_PATH", "../").toString();
    if(!QDir(LOAD_PATH).exists())LOAD_PATH = "../";
    WRITE_PATH = settings.value("WRITE_PATH", "../").toString();
    if(!QDir(WRITE_PATH).exists())WRITE_PATH = "../";
    WRITE_IMAGE_PATH = settings.value("WRITE_IMAGE_PATH", "../").toString();
    if(!QDir(WRITE_IMAGE_PATH).exists())WRITE_IMAGE_PATH = "../";
    RESTORE_PATH = settings.value("RESTORE_PATH", "../").toString();
    if(!QFileInfo(RESTORE_PATH).exists())RESTORE_PATH = "../";
    WORKSPACE = settings.value("WORKSPACE", EXE_PATH.path()).toString();
    if((!QDir(WORKSPACE).exists()) || WORKSPACE.isEmpty()) WORKSPACE = EXE_PATH.path();    
    CLASS_SEPARATOR = settings.value("CLASS_SEPARATOR", "").toString();

    int size = settings.beginReadArray("TECHNICAL_LIST");
    if(!size) resetTechnicalPrograms();
    else
    {
        for(int i = 0; i < size; ++i)
        {
            settings.setArrayIndex(i);
            TECHNICAL_LIST.push_back(settings.value("TECHNICAL_PROGRAM").toString());
        }
    }

    settings.endArray();
    settings.endGroup();

    settings.beginGroup("RecentFiles");
    int sizeRecentFiles = settings.beginReadArray("RECENT_FILES");
    for(int i = 0; i < sizeRecentFiles; ++i)
    {
        settings.setArrayIndex(i);
        QString relativePath = settings.value("PATH").toString();
        saveInRecentFiles(relativePath);
    }
    settings.endArray();
    settings.endGroup();

    settings.beginGroup("CurrentFiles");
    int currentIndex = settings.value("CURRENT_INDEX",-1).toInt();

    int sizeCurrentFiles = settings.beginReadArray("CURRENT_FILES");
    for(int i = 0; i < sizeCurrentFiles; ++i)
    {
        settings.setArrayIndex(i);
        QString path = settings.value("PATH").toString();
        QFileInfo fileInfo(path);
        if(fileInfo.exists() && fileInfo.isFile())
        {
            CustomGraphics* temporaryGraphics = new CustomGraphics(this);
            LoadXML loadXML(path, temporaryGraphics);
            if(loadXML.getError()) continue;
            // Si xml en entrée load.read() fait le job, sinon on utilise un traducteur
            if(loadXML.read()) load(path);

        }
    }

    if(currentIndex !=-1) tabWidget->setCurrentIndex(currentIndex);

    settings.endArray();
    settings.endGroup();
}

void MainWindow::resetTechnicalPrograms()
{
    //!\\ modifier aussi dans SettingsDialog::on_pushButtonInitTechnical_clicked()
    TECHNICAL_LIST.push_back("SORT");
    TECHNICAL_LIST.push_back("MERGE");
    TECHNICAL_LIST.push_back("ADRDSSU");
    TECHNICAL_LIST.push_back("IEFBR14");
    TECHNICAL_LIST.push_back("IEBGENER");
    TECHNICAL_LIST.push_back("IDCAMS");
    TECHNICAL_LIST.push_back("GVEXPORT");
    TECHNICAL_LIST.push_back("CFTUTIL");
    TECHNICAL_LIST.push_back("DSNTIAUL");
}


void MainWindow::resetColorRules()
{
    //!\\ modifier aussi dans SettingsDialog::on_pushButtonInitColorRules_clicked()
    COLOR_RULES.clear();
    COLOR_RULES.push_back(ColorRule("ETAT",QColor(Qt::white)));
    COLOR_RULES.push_back(ColorRule("XP",QColor(Qt::lightGray)));
    COLOR_RULES.push_back(ColorRule("XP2",QColor(Qt::lightGray)));
}

void MainWindow::resetPCLRules()
{
    //!\\ modifier aussi dans SettingsDialog::on_pushButtonInitPCLRules_clicked()
    PCL_RULES.clear();
    /// \todo : supprimer les commentaires avant livraison
//    PCL_RULES.push_back(PCLRule("AT60110","PATKW","CODE","DM6",1));
//    PCL_RULES.push_back(PCLRule("AT60110","PATKW","IDFIN","XP",0));
//    PCL_RULES.push_back(PCLRule("AT60120","PATKW","CODE","DM6",0));
//    PCL_RULES.push_back(PCLRule("AT60120","PATKW","IDFOUT","XP",1));
//    PCL_RULES.push_back(PCLRule("AT60120","PATKW","IDFEXT","XP",1));
//    PCL_RULES.push_back(PCLRule("AT60125","PATKW","IDFEXT","XP",0));
//    PCL_RULES.push_back(PCLRule("RDJXPUTC","PATKW","RDJFIL","MQ",1));
//    PCL_RULES.push_back(PCLRule("RDJXPUTC","PATKW","RDJFIC","XP",0));
//    PCL_RULES.push_back(PCLRule("RDJXGETC","PATKW","RDJFIL","MQ",0));
//    PCL_RULES.push_back(PCLRule("RDJXGETC","PATKW","RDJFIC","XP",1));
//    PCL_RULES.push_back(PCLRule("CFTUTIL","PATKW","FICENV","XP",0));
}

QAction *MainWindow::getActionSelectMode() { return ui->actionMode_s_lection; }

CustomGraphics *MainWindow::getGraphics(int _tabID) { return static_cast<CustomGraphics*>(tabWidget->widget(_tabID)); }

CustomGraphics *MainWindow::getCurrentGraphics() { return static_cast<CustomGraphics*>(tabWidget->currentWidget()); }

void MainWindow::setStatus(const QString &_status)
{
    ui->statusBar->setStatusTip(_status);
    ui->statusBar->showMessage(_status, 5000);
}

void MainWindow::closeEvent(QCloseEvent* event)
{
    /// Avant de quitter, proposer l'enregistrement des ddc non sauvegardés
    SaveBeforeQuitDialog *saveDialog = new SaveBeforeQuitDialog(this);

    QVector<int> modifiedIndexes;
    /// Initialisation de l'interface
    for(int i=0; i<tabWidget->count(); i++)
    {
        if(getGraphics(i)->getModified())
        {
            modifiedIndexes.push_back(i);
            QHBoxLayout* row = new QHBoxLayout();
            row->addSpacing(100);
            QCheckBox* checkbox = new QCheckBox(saveDialog);
            QSizePolicy placeOnTheLeft(QSizePolicy::Preferred, QSizePolicy::Preferred);
            placeOnTheLeft.setHorizontalStretch(1);
            checkbox->setSizePolicy(placeOnTheLeft);
            checkbox->setChecked(true);
            row->addWidget(checkbox);

            QLabel * label = new QLabel(getGraphics(i)->getTabName(),saveDialog);
            QSizePolicy spRight(QSizePolicy::Preferred, QSizePolicy::Preferred);
            spRight.setHorizontalStretch(10);
            label->setSizePolicy(spRight);
            row->addWidget(label);
            saveDialog->addRow(row, i);
        }
    }

    /// Ouverture de l'interface seulement si nécessaire et fermeture si nécessaire
    int closingConfirmed = 1;
    if(modifiedIndexes.size()) closingConfirmed  = saveDialog->exec();

    if(closingConfirmed)
    {
        writeSettings();
        event->accept();
    }
    else event->ignore();
}

void MainWindow::undo()
{
    //qDebug()<<"undo asked";
    if(tabWidget->currentIndex() == -1) return;
    getCurrentGraphics()->undo();
    getCurrentGraphics()->updateScene();
}

void MainWindow::redo()
{
    //qDebug()<<"redo asked";
    if(tabWidget->currentIndex() == -1) return;
    getCurrentGraphics()->redo();
    getCurrentGraphics()->updateScene();
}

void MainWindow::align()
{
    if(tabWidget->currentIndex() == -1) return;
    getCurrentGraphics()->align();
}

void MainWindow::copy()
{
    if(tabWidget->currentIndex() == -1) return;
    getCurrentGraphics()->copy();
    getCurrentGraphics()->unselectNodes();
}

void MainWindow::cut()
{
    if(tabWidget->currentIndex() == -1) return;
    getCurrentGraphics()->cut();
}

void MainWindow::duplicate()
{
    if(tabWidget->currentIndex() == -1) return;
    getCurrentGraphics()->duplicate();
}

void MainWindow::modifyColorOfSelection()
{
    if(tabWidget->currentIndex() == -1) return;
    getCurrentGraphics()->modifyColorOfSelection();
}
void MainWindow::resetColorOfSelection()
{
    if(tabWidget->currentIndex() == -1) return;
    getCurrentGraphics()->resetColorOfSelection();
}

void MainWindow::search() { searchWidget->setVisible(true); }

void MainWindow::print()
{
    if(tabWidget->currentIndex() == -1) return;

    QPrinter printer(QPrinter::HighResolution);
    //printer.setPaperSize(QPrinter::A4);
    QPrintPreviewDialog preview(&printer, this);
    preview.setWindowTitle("Aperçu avant impression");

    connect(&preview, SIGNAL(paintRequested(QPrinter*)), SLOT(printPreview(QPrinter*)));
    preview.exec();
}

void MainWindow::updateLinks()
{
    if(tabWidget->currentIndex() == -1) return;
    getCurrentGraphics()->updateLinks();
}

void MainWindow::printPreview(QPrinter *printer)
{
    QGraphicsScene* scene = getCurrentGraphics()->getScene();
    scene->setSceneRect(scene->itemsBoundingRect());
    getCurrentGraphics()->unselectNodes();

    QPainter painter(printer);

    // Centrer : pour ça on récupère l'échelle utilisée (mise à l'échelle automatique)
    double xscale = printer->pageRect().width()/double(scene->width());
    double yscale = printer->pageRect().height()/double(scene->height());
    double scale = qMin(xscale, yscale);
    double sceneWidth = scene->width()*scale;
    double sceneHeight = scene->height()*scale;

    painter.translate((printer->pageRect().width()-sceneWidth)/2,
                      (printer->pageRect().height()-sceneHeight)/2);
    scene->render(&painter);
}


void MainWindow::printSelection()
{
    temporaryGraphicsForPrint = new CustomGraphics(this);
    if(tabWidget->currentIndex() == -1) return;

    QPrinter printer(QPrinter::HighResolution);
    printer.setPaperSize(QPrinter::A4);
    QPrintPreviewDialog preview(&printer, this);
    preview.setWindowTitle("Aperçu avant impression de la sélection");
    connect(&preview, SIGNAL(paintRequested(QPrinter*)), SLOT(printSelectionPreview(QPrinter*)));
    preview.exec();
}

void MainWindow::exportPDF()
{
    if(tabWidget->currentIndex() == -1) return;

    QPrinter printer(QPrinter::HighResolution);
    printer.setOutputFormat( QPrinter::PdfFormat );

    printer.setOutputFileName(WRITE_PATH + "/" + getCurrentGraphics()->getTabName() + ".pdf");
    QPrintPreviewDialog preview(&printer, this);
    preview.setWindowTitle("Aperçu avant export en PDF");

    connect(&preview, SIGNAL(paintRequested(QPrinter*)), SLOT(printPreview(QPrinter*)));
    preview.exec();
}



void MainWindow::printSelectionPreview(QPrinter *printer)
{
    copy();
    temporaryGraphicsForPrint->paste();

    QGraphicsScene* scene = temporaryGraphicsForPrint->getScene();
    scene->setSceneRect(scene->itemsBoundingRect());
    temporaryGraphicsForPrint->unselectNodes();

    QPainter painter(printer);

    // Centrer : pour ça on récupère l'échelle utilisée (mise à l'échelle automatique)
    double xscale = printer->pageRect().width()/double(scene->width());
    double yscale = printer->pageRect().height()/double(scene->height());
    double scale = qMin(xscale, yscale);
    double sceneWidth = scene->width()*scale;
    double sceneHeight = scene->height()*scale;
    painter.translate((printer->pageRect().width()-sceneWidth)/2,
                      (printer->pageRect().height()-sceneHeight)/2);
    scene->render(&painter);
}

void MainWindow::copyImageToClipboard()
{
    if(tabWidget->currentIndex() == -1) return;
    getCurrentGraphics()->copyImageToClipboard();
}

void MainWindow::paste()
{
    if(tabWidget->currentIndex() == -1) return;
    getCurrentGraphics()->paste();
}

void MainWindow::clearClipboard()
{
    for(int i=0; i<clipboardNodes.size(); i++) { delete clipboardNodes[i]; }
    clipboardLinks.clear();
    clipboardNodes.clear();
    setPasteEnabled(false);
}

void MainWindow::setPasteEnabled(bool _value) { ui->actionColler_le_contenu_du_presse_papier->setEnabled(_value); }

NodeGraphic* MainWindow::getClipboardNode(int _id)
{
    foreach(NodeGraphic* node, clipboardNodes) { if(node->getId()==_id) return node; }
    return 0;
}

void MainWindow::addToClipboard(NodeGraphic* _node) { clipboardNodes.push_back(_node); }
void MainWindow::addToClipboard(LinkGraphic* _link) { clipboardLinks.push_back(_link); }

/// Ouverture d'une liste de fichier lorsqu'on appelle "Ouvrir" dans le menu
/// On demande la liste des fichiers à ouvrir à l'utilisateur et on appelle la fonction load(path) pour chaque
void MainWindow::load()
{
    QString openingLocation = LOAD_PATH;

    QStringList myfiles = QFileDialog::getOpenFileNames(this, tr("Ouvrir un fichier .txt ou .xml"), openingLocation, tr("Fichiers reconnus(*.txt *.ddc);;Tous les fichiers(*);;Fichiers Texte(*.txt);;Dessins de chaine(*.ddc)"));
    for(int i=0; i<myfiles.size();i++)
    {
        QString myfile = myfiles.at(i);
        load(myfile);
        LOAD_PATH = QFileInfo(myfile).path();
    }

    writeSettings(); // pour sauvegarde fichiers récents
}

/// ouverture d'un dessin de chaine connaissant son chemin
/// paramètre :
/// - path : chemin du fichier à ouvrir
void MainWindow::load(const QString &_path)
{

    QDir workspace(WORKSPACE);
    QString relativePath = workspace.relativeFilePath(_path);
    saveInRecentFiles(relativePath);
    // fichier dejà ouvert, on ne fait que changer la tab
    int index = fileAlreadyOpened(_path);
    if(index!=-1)
    {
        tabWidget->setCurrentIndex(index);
        setStatus("Fichier déjà ouvert !");
    }
    // sinon on en ouvre une nouvelle
    else
    {
        openingInProgress = true;
        CustomGraphics* newTab = new CustomGraphics(this);

        tabWidget->setCurrentIndex(tabWidget->addTab(newTab,""));
        if(tabWidget->currentIndex() == 0) toggleButtons(true);

        //Chargement
        getCurrentGraphics()->load(_path);

        tabWidget->setTabText(tabWidget->currentIndex(),getCurrentGraphics()->getTabName());

        // Options d'affichage en fonction de l'état des menus
        if(ui->actionAfficher_les_ancres->isChecked()) getCurrentGraphics()->displayAnchors(ui->actionAfficher_les_ancres->isChecked());
        displayDDName(ui->actionAfficher_les_DDName_sur_les_liens->isChecked());
        hideClass(ui->actionCacher_la_classe_des_DCLF->isChecked());
        hideStepName(ui->actionCacher_le_nom_des_steps->isChecked());
        if(ui->actionCacher_les_SYSIN->isChecked()) getCurrentGraphics()->hideSysins(ui->actionCacher_les_SYSIN->isChecked());
        if(ui->actionCacher_les_conditions->isChecked()) getCurrentGraphics()->hideConditions(ui->actionCacher_les_conditions->isChecked());
        if(ui->actionCacher_le_titre->isChecked()) getCurrentGraphics()->hideTitle(ui->actionCacher_le_titre->isChecked());
        setTitleOnTop(ui->actionMettre_le_titre_en_haut->isChecked());

        // Options de sélection en fonction de l'état des menus
        setSelectMode(ui->actionMode_s_lection->isChecked());
        branchSelection(ui->actionActiver_la_s_lection_par_branche->isChecked());
        setStatus("Fichier ouvert : " + _path);

        zoomWindow();
        openingInProgress = false;
    }
}

void MainWindow::saveInRecentFiles(const QString &_relativePath)
{
    //qDebug()<<"SaveInRecentFiles "<<_relativePath;
    // Suppression de la ligne si déjà existante pour la remettre au dessus
    if(getActionRecentFile(_relativePath))
    {
        ui->menuFichiers_ouverts_r_cemment->removeAction(getActionRecentFile(_relativePath)->getAction());
        RECENT_FILES.removeAll(getActionRecentFile(_relativePath));
    }

    // Si fichier inexistant => on n'ajoute pas la ligne au menu
    QDir workspace(WORKSPACE);
    QString finalPath = workspace.cleanPath(workspace.absoluteFilePath(_relativePath));
    QFileInfo fileInfo(finalPath);
    if(!fileInfo.exists()||!fileInfo.isFile())
    {
        qDebug()<<"Fichier "<<finalPath<<" inexistant : existant ?"<<fileInfo.exists()<<"; est un fichier ?"<<fileInfo.isFile();
        return;
    }

    // Si fichier existant
    if(!_relativePath.isEmpty())
    {
        // On ajoute la ligne en haut du menu
        QAction* newAction = new QAction(_relativePath, ui->menuFichiers_ouverts_r_cemment);
        ui->menuFichiers_ouverts_r_cemment->insertAction(ui->menuFichiers_ouverts_r_cemment->actions()[0], newAction);
        ActionRecentFile* newActionRecentFile = new ActionRecentFile(newAction, _relativePath,this);
        RECENT_FILES.push_front(newActionRecentFile);

        // Si on dépasse la limite du nombre de fichiers récents, on supprime le trop plein
        if(RECENT_FILES.size() > MAX_RECENT_FILES_SIZE)
        {
            for(int i=MAX_RECENT_FILES_SIZE; i<RECENT_FILES.size(); i++)
            {
                ui->menuFichiers_ouverts_r_cemment->removeAction(RECENT_FILES[i]->getAction());
                delete RECENT_FILES[i];
            }

            RECENT_FILES.erase(RECENT_FILES.begin()+ MAX_RECENT_FILES_SIZE, RECENT_FILES.end());
        }

        ui->menuFichiers_ouverts_r_cemment->setEnabled(true);
    }
}

ActionRecentFile* MainWindow::getActionRecentFile(const QString &_relativePath)
{
    foreach(ActionRecentFile* action, RECENT_FILES) { if(action->getRelativePath() == _relativePath) return action; }
    return 0;
}

/// regarde si le chemin donné en paramètre est déjà ouvert
/// paramètre : chemin à cherche parmi les chemins des dessins de chaine ouverts
/// retour : position de la tab contenant le dessin de chaine, ou -1 si aucune tab ne correspond
int MainWindow::fileAlreadyOpened(QString _path)
{
    if(_path.isEmpty()) return -1;
    for(int i=0; i<tabWidget->count(); i++){ if(getGraphics(i)->getCurrentFilename() == _path) return i;}
    return -1;
}

void MainWindow::saveAs()
{
    if(tabWidget->currentIndex() == -1) return;
    getCurrentGraphics()->saveAs();
}

void MainWindow::saveAll()
{
    if(tabWidget->currentIndex() == -1) return;
    for(int i=0; i<tabWidget->count(); i++) { getGraphics(i)->save(); }
}

void MainWindow::save()
{
    if(tabWidget->currentIndex() == -1) return;
    getCurrentGraphics()->save();
}

void MainWindow::save(int _index)
{
    if(tabWidget->currentIndex() == -1) return;
    getGraphics(_index)->save();
}

void MainWindow::exportCSV()
{
    if(tabWidget->currentIndex() == -1) return;
    getCurrentGraphics()->exportCSV();
}

bool MainWindow::newFile()
{
    OpenNewDialog* openNewDialog = new OpenNewDialog(this);
    if(openNewDialog->exec())
    {
        CustomGraphics* newTab = new CustomGraphics(this);

        tabWidget->setCurrentIndex(tabWidget->addTab(newTab,openNewDialog->getName()));
        getCurrentGraphics()->setTabName(openNewDialog->getName());
        getCurrentGraphics()->newFile();
        QString title = openNewDialog->getTitle();
        if(title.isEmpty()) title = "Titre à modifier";
        getCurrentGraphics()->setTitle(title);
        getCurrentGraphics()->getTitle()->setVersion(openNewDialog->getVersion());
        setStatus("Nouveau dessin de chaine vierge créé");

        toggleButtons(true);
        return true;
    }

    return false;
}

void MainWindow::toggleButtons(bool _value)
{
    ui->actionAligner->setEnabled(_value);
    ui->actionSauvegarder_Image->setEnabled(_value);
    ui->actionSauvegarder_XML->setEnabled(_value);
    ui->actionSauvegarder_XML_sous->setEnabled(_value);
    ui->actionSauvegarder_tous_les_DDC_ouverts->setEnabled(_value);
    ui->actionZoom_100->setEnabled(_value);
    ui->actionZoom_fen_tre->setEnabled(_value);
    ui->actionZoomIn->setEnabled(_value);
    ui->actionZoomOut->setEnabled(_value);
    ui->actionExport_en_CSV->setEnabled(_value);
    ui->actionExporter_au_format_PDF->setEnabled(_value);
    ui->actionCopier_dans_le_presse_papier->setEnabled(_value);
    ui->actionCopier_l_image_dans_le_presse_papier->setEnabled(_value);
    ui->actionSupprimer_la_s_lection->setEnabled(_value);
    ui->actionTrier_la_s_l_ction->setEnabled(_value);
    ui->actionTrier->setEnabled(_value);
    ui->actionMont_e_de_version->setEnabled(_value);
    ui->actionImprimer->setEnabled(_value);
    ui->actionImprimer_la_s_lection->setEnabled(_value);
}

void MainWindow::setSelectMode(bool _toggled)
{
    if(tabWidget->currentIndex() == -1) return;
    getCurrentGraphics()->setSelectMode(_toggled);
}

void MainWindow::saveImage()
{
    if(tabWidget->currentIndex() == -1) return;
    ui->actionAfficher_les_ancres->setChecked(false);
    getCurrentGraphics()->saveImage();
}

void MainWindow::restore()
{
    QFileInfo tmpFile(EXE_PATH, "tmp.ddc");
    load(tmpFile.absoluteFilePath());
    getCurrentGraphics()->setCurrentFilename(RESTORE_PATH);
    getCurrentGraphics()->setModified(true);
    setStatus("Dernière sauvegarde temporaire restaurée");
}

void MainWindow::versionUpdate()
{
    if(tabWidget->currentIndex() == -1) return;
    getCurrentGraphics()->versionUpdate();
}

void MainWindow::closeTab(int index)
{
    if (index == -1) return;

    CustomGraphics* tabItem = getGraphics(index);
    int accepted = 1;
    if(tabItem->getModified())
    {
        SaveDialog* saveDialog = new SaveDialog(this, index);
        accepted = saveDialog->exec();
        delete saveDialog;
    }

    if(accepted || !tabItem->getModified())
    {
        setStatus("Onglet fermé");
        tabWidget->removeTab(index);
        delete tabItem;

        if(tabWidget->currentIndex() == -1) toggleButtons(false);
        writeSettings();
    }
}

void MainWindow::tabChanged(int _index)
{
    // Si plus d'onglet ouvert
    if(_index == -1)
    {
        setWindowTitle(BASE_NAME);
        tabWidget->setStyleSheet("background-color: lightgray");
        searchWidget->clearResults();
        searchWidget->setLabel("Ouvrez un dessin de chaine pour effectuer une recherche");
    }
    // Si il reste des onglets ouverts
    if(_index > -1)
    {
        CustomGraphics* graphics = getCurrentGraphics();
        setSelectMode(SELECT_MODE);
        setWindowTitle(graphics->renameWindow());
        setActivatedUndo(graphics->getTmpSize());
        setActivatedRedo(graphics->getRedoSize());
        tabWidget->setStyleSheet("background-color: white");
        searchWidget->setGraphics(graphics);
        searchWidget->search();

        //Chercher si il y a eu des modifs depuis l'ouverture du fichier
        if(!openingInProgress)
        {
            LoadXML loadXML(graphics->getCurrentFilename(), graphics, false);
            if(!loadXML.getError()) //Si fichier supprimé pas de pb
            {
                //Warning si le fichier a changé de version depuis l'ouverture
                if(loadXML.readModificationDate()> graphics->getModificationDate()||
                   (loadXML.readModificationDate()== graphics->getModificationDate()&&loadXML.readModificationTime()> graphics->getModificationTime()))
                {
                    LoadLastVersionDialog dialog;
                    dialog.setUser(loadXML.readUser());
                    dialog.setDateAndTime(loadXML.readModificationDate().toString()+" "+loadXML.readModificationTime().toString());
                    int accepted = dialog.exec();
                    if(accepted)
                    {
                        QString fileName = graphics->getCurrentFilename();
                        tabWidget->removeTab(tabWidget->currentIndex());
                        load(fileName);
                    }
                }
            }
        }
    }
}

void MainWindow::zoom100pc()
{
    if(tabWidget->currentIndex() == -1) return;
    CustomGraphics * graphics = getCurrentGraphics();
    if(graphics->getNodeGraphics().size() == 0) return;
    graphics->scale(1.0/graphics->getScaleFactor(), 1.0/ graphics->getScaleFactor());
    graphics->setScaleFactor(1);
}

void MainWindow::zoomIn()
{
    if(tabWidget->currentIndex() == -1) return;
    CustomGraphics * graphics = getCurrentGraphics();
    if(graphics->getNodeGraphics().size() == 0) return;
    graphics->zoomIn();
}

void MainWindow::zoomOut()
{
    if(tabWidget->currentIndex() == -1) return;
    CustomGraphics * graphics = getCurrentGraphics();
    if(graphics->getNodeGraphics().size() == 0) return;
    graphics->zoomOut();
}

void MainWindow::zoomWindow()
{
    /// On se replace à un facteur d'échelle de 1
    zoom100pc();
    /// Si pas de scene, on ne fait rien
    if(tabWidget->currentIndex() == -1) return;
    CustomGraphics * graphics = getCurrentGraphics();
    /// Si aucun noeud dans la scene, on ne fait rien
    if(graphics->getNodeGraphics().size() == 0) return;

    /// On prend le facteur d'échelle le plus petit entre celui en largeur et celui en hauteur
    double scaleFactor = graphics->width()/(graphics->getScene()->itemsBoundingRect().width()+2*SCENE_MARGIN);
    if(graphics->height()/(graphics->getScene()->itemsBoundingRect().height()+2*SCENE_MARGIN)<scaleFactor) scaleFactor = graphics->height()/(graphics->getScene()->itemsBoundingRect().height()+2*SCENE_MARGIN);

    /// Mise à l'échelle
//    graphics->getScene()->setSceneRect(graphics->getScene()->itemsBoundingRect());
    graphics->scale(scaleFactor, scaleFactor);
    graphics->setScaleFactor(scaleFactor);
}

void MainWindow::modifyLinkType(QAction* _action)
{
    if(_action==ui->actionCourbes) LINK_TYPE = MyShapes::CurvedLine;
    if(_action==ui->actionDirects) LINK_TYPE = MyShapes::StraightLine;
    if(_action==ui->actionLignes_droites) LINK_TYPE = MyShapes::BrokenLine;
}

//void MainWindow::changeEvent( QEvent* e )
//{
//    if( e->type() == QEvent::WindowStateChange )
//    {
//        QWindowStateChangeEvent* event = static_cast< QWindowStateChangeEvent* >( e );

//        if( event->oldState() & Qt::WindowMinimized )
//        {
//            CustomGraphics* graphics = getCurrentGraphics();

//            if(!graphics) return;

//            //warnAboutConflict(graphics);
//        }
//        else if( event->oldState() == Qt::WindowNoState && this->windowState() == Qt::WindowMaximized )
//        {
//            //qDebug() << "Window Maximized!";
//        }
//    }
//}

void MainWindow::focusInEvent(QFocusEvent *e)
{
    QMainWindow::focusInEvent(e);
    CustomGraphics* graphics = getCurrentGraphics();
    if(!graphics) return;

    warnAboutConflict(graphics);
}

void MainWindow::warnAboutConflict(CustomGraphics* _graphics)
{
    if(savingInProgress) return;
    if(_graphics->getCurrentFilename().isEmpty()) return;
    LoadXML loadXML(_graphics->getCurrentFilename(), _graphics);
    if(!loadXML.getError()) //Si fichier supprimé pas de pb
    {
        //Warning si le fichier a changé de version depuis l'ouverture
        if(loadXML.readModificationDate()> _graphics->getModificationDate()||
           (loadXML.readModificationDate()== _graphics->getModificationDate()&&loadXML.readModificationTime()> _graphics->getModificationTime()))
        {
            LoadLastVersionDialog loadLastVersionDialog;
            loadLastVersionDialog.setUser(loadXML.readUser());
            loadLastVersionDialog.setDateAndTime(loadXML.readModificationDate().toString()+" "+loadXML.readModificationTime().toString());
            int accepted = loadLastVersionDialog.exec();
            if(accepted > 0)
            {
                QString fileName = _graphics->getCurrentFilename();
                tabWidget->removeTab(tabWidget->currentIndex());
                load(fileName);
            }
        }
    }
}

template<typename T> T* MainWindow::pluginByName(const QString& fileName) {

    QPluginLoader loader(fileName); // On charge le plugin.

    QObject *plugin = loader.instance(); // On prend l'instance.

    return qobject_cast<T*>(plugin); // Et on retourne le plugin casté.

}


template<typename T> QList<T*> MainWindow::loadPluginByDir(const QString& dir)
{
    QList<T*> ls; // On crée la liste où seront mis tous les plugins valides.

    QDir plugDir = QDir(dir);
    foreach(QString file, plugDir.entryList(QDir::Files)) { // On prend la liste des fichiers.

        if(T* plugin = pluginByName<T>(plugDir.absoluteFilePath(file))) // On vérifie si le plugin existe.
            ls.push_back(plugin); // On l'ajoute à la liste si oui.

    }

    return ls;
}

