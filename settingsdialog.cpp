#include "settingsdialog.h"
#include "ui_settingsdialog.h"
#include "utils.h"
#include "mainwindow.h"
#include <QDebug>
#include <QColorDialog>
#include "colorrulewidget.h"
#include "QFileDialog"

SettingsDialog::SettingsDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SettingsDialog),
    workspace("")
{
    ui->setupUi(this);
    resetModel();
    ui->listView->setSelectionMode(QAbstractItemView::ExtendedSelection);
    ui->buttonBox->addButton("&Annuler", QDialogButtonBox::RejectRole);

    ui->tabTechnical->setToolTip("Modifier les programmes considérés comme des programmes techniques");

    ui->horizontalSliderFontSize->setSliderPosition(FONT_SIZE);
    ui->horizontalSliderFontSize->setMouseTracking(false);
    ui->labelFontSize->setNum(FONT_SIZE);
    connect(ui->horizontalSliderFontSize, SIGNAL(valueChanged(int)), ui->labelFontSize, SLOT(setNum(int)));

    ui->horizontalSliderDDNameSize->setSliderPosition(DDNAME_FONT_SIZE);
    ui->labelDDNameSize->setNum(DDNAME_FONT_SIZE);
    connect(ui->horizontalSliderDDNameSize, SIGNAL(valueChanged(int)), ui->labelDDNameSize, SLOT(setNum(int)));

    ui->horizontalSliderTechnicalFontSize->setSliderPosition(TECHNICAL_FONT_SIZE);
    ui->labelTechnicalFontSize->setNum(TECHNICAL_FONT_SIZE);
    connect(ui->horizontalSliderTechnicalFontSize, SIGNAL(valueChanged(int)), ui->labelTechnicalFontSize, SLOT(setNum(int)));

    ui->horizontalSliderTitleSize->setSliderPosition(TITLE_SIZE);
    ui->labelTitleSize->setNum(TITLE_SIZE);
    connect(ui->horizontalSliderTitleSize, SIGNAL(valueChanged(int)), ui->labelTitleSize, SLOT(setNum(int)));

    ui->horizontalSliderTitleMargin->setSliderPosition(TITLE_MARGIN);
    ui->labelTitleMargin->setNum(TITLE_MARGIN);
    connect(ui->horizontalSliderTitleMargin, SIGNAL(valueChanged(int)), ui->labelTitleMargin, SLOT(setNum(int)));

    ui->horizontalSliderWeight->setSliderPosition(DETAILED_WEIGHT);
    ui->labelWeight->setNum(DETAILED_WEIGHT);
    connect(ui->horizontalSliderWeight, SIGNAL(valueChanged(int)), ui->labelWeight, SLOT(setNum(int)));

    ui->horizontalSliderWidth->setSliderPosition(DEFAULT_WIDTH);
    ui->labelWidth->setNum(DEFAULT_WIDTH);
    connect(ui->horizontalSliderWidth, SIGNAL(valueChanged(int)), ui->labelWidth, SLOT(setNum(int)));

    ui->horizontalSliderHeight->setSliderPosition(DEFAULT_HEIGHT);
    ui->labelHeight->setNum(DEFAULT_HEIGHT);
    connect(ui->horizontalSliderHeight, SIGNAL(valueChanged(int)), ui->labelHeight, SLOT(setNum(int)));

    ui->horizontalSliderCondition->setSliderPosition(CONDITION_SIZE);
    ui->labelCondition->setNum(CONDITION_SIZE);
    connect(ui->horizontalSliderCondition, SIGNAL(valueChanged(int)), ui->labelCondition, SLOT(setNum(int)));

    ui->horizontalSliderAnchors->setSliderPosition(ANCHOR_SIZE);
    ui->labelAnchors->setNum(ANCHOR_SIZE);
    connect(ui->horizontalSliderAnchors, SIGNAL(valueChanged(int)), ui->labelAnchors, SLOT(setNum(int)));

    ui->horizontalSliderSysin->setSliderPosition(SYSIN_SIZE);
    ui->labelSysin->setNum(SYSIN_SIZE);
    connect(ui->horizontalSliderSysin, SIGNAL(valueChanged(int)), ui->labelSysin, SLOT(setNum(int)));

    ui->horizontalSliderShadow->setSliderPosition(SHADOW_OFFSET);
    ui->labelShadow->setNum(SHADOW_OFFSET);
    connect(ui->horizontalSliderShadow, SIGNAL(valueChanged(int)), ui->labelShadow, SLOT(setNum(int)));

    ui->horizontalSliderGridWidth->setSliderPosition(ALIGN_GRID_X);
    ui->labelGridWidth->setNum(ALIGN_GRID_X);
    connect(ui->horizontalSliderGridWidth, SIGNAL(valueChanged(int)), ui->labelGridWidth, SLOT(setNum(int)));

    ui->horizontalSliderGridHeight->setSliderPosition(ALIGN_GRID_Y);
    ui->labelGridHeight->setNum(ALIGN_GRID_Y);
    connect(ui->horizontalSliderGridHeight, SIGNAL(valueChanged(int)), ui->labelGridHeight, SLOT(setNum(int)));

    ui->horizontalSliderGridSubWidth->setSliderPosition(ALIGN_GRID_X_DIVISION);
    ui->labelGridSubWidth->setNum(ALIGN_GRID_X_DIVISION);
    connect(ui->horizontalSliderGridSubWidth, SIGNAL(valueChanged(int)), ui->labelGridSubWidth, SLOT(setNum(int)));

    ui->horizontalSliderGridSubHeight->setSliderPosition(ALIGN_GRID_Y_DIVISION);
    ui->labelGridSubHeight->setNum(ALIGN_GRID_Y_DIVISION);
    connect(ui->horizontalSliderGridSubHeight, SIGNAL(valueChanged(int)), ui->labelGridSubHeight, SLOT(setNum(int)));

    ui->horizontalSliderInsideMargin->setSliderPosition(INSIDE_MARGIN);
    ui->labelInsideMargins->setNum(INSIDE_MARGIN);
    connect(ui->horizontalSliderInsideMargin, SIGNAL(valueChanged(int)), ui->labelInsideMargins, SLOT(setNum(int)));

    ui->horizontalSliderRedimStep->setSliderPosition(RESIZE_STEP);
    ui->labelRedimStep->setNum(RESIZE_STEP);
    connect(ui->horizontalSliderRedimStep, SIGNAL(valueChanged(int)), ui->labelRedimStep, SLOT(setNum(int)));

    for(int i=0; i<COLOR_RULES.size(); i++)
    {
        ColorRuleWidget* newRule = new ColorRuleWidget(COLOR_RULES[i].classe,COLOR_RULES[i].color);
        connect(newRule,SIGNAL(deleteColorRule(QString)),this,SLOT(removeColorRule(QString)));
        ui->verticalLayoutColorRules->addWidget(newRule);
        colorRules.push_back(newRule);
    }
    ui->lineEditSeparator->setText(CLASS_SEPARATOR);

    for(int i=0; i<PCL_RULES.size(); i++)
    {
        PCLRuleWidget* newRule = new PCLRuleWidget(PCL_RULES[i].program, PCL_RULES[i].param, PCL_RULES[i].subparam, PCL_RULES[i].classe, PCL_RULES[i].outputFile);
        connect(newRule,SIGNAL(deletePCLRule(QString,QString,QString)),this,SLOT(removePCLRule(QString,QString,QString)));
        ui->verticalLayoutPCLRules->addWidget(newRule);
        PCLRules.push_back(newRule);
    }

    ui->pushButtonAddColorRule->setDisabled(true);
    ui->pushButtonAdd->setDisabled(true);
    connect(ui->lineEdit,SIGNAL(textChanged(QString)),this,SLOT(allowAddNewTechnical(QString)));
    connect(ui->lineEditAddColorRule,SIGNAL(textChanged(QString)),this,SLOT(allowAddNewColorRule(QString)));
    selectedNodeColor = SELECTED_COLOR;
    programColor = NORMAL_STEP_COLOR;
    technicalColor = TECHNICAL_STEP_COLOR;
    defaultFileColor = NORMAL_FILE_COLOR;
    patternStepColor = PATTERN_STEP_COLOR;
    fileHoverColor = FILE_HOVER_COLOR;
    initializeColorButtonsColor();

    ui->pushButtonWorkspace->setText(WORKSPACE);

    ui->lineEditProgramPCLRule->setPlaceholderText("Programme");
    ui->lineEditParamPCLRule->setPlaceholderText("Paramètre");
    ui->lineEditSubParamPCLRule->setPlaceholderText("Sous-paramètre (facultatif)");
    ui->lineEditClassePCLRule->setPlaceholderText("Classe (facultatif)");

    ui->tabWidget->setCurrentIndex(2);
}

void SettingsDialog::removeColorRule(const QString &_class)
{
    for(int i=0; i<colorRules.size(); i++)
    {
        if(colorRules[i]->getClasse()==_class)
        {
            delete colorRules[i];
            colorRules.erase(colorRules.begin()+i);
            break;
        }
    }
}

void SettingsDialog::removePCLRule(const QString &_program, const QString &_parameter, const QString &_subparam)
{
    for(int i=0; i<PCLRules.size(); i++)
    {
        if(PCLRules[i]->getProgram()==_program && PCLRules[i]->getParameter()==_parameter && PCLRules[i]->getSubparam()==_subparam)
        {
            delete PCLRules[i];
            PCLRules.erase(PCLRules.begin()+i);
            break;
        }
    }
}

SettingsDialog::~SettingsDialog() { delete ui; }

void SettingsDialog::resetModel()
{
    model = new QStandardItemModel;
    for(int i=0; i< TECHNICAL_LIST.size(); i++)
    {
        QStandardItem* program = new QStandardItem(TECHNICAL_LIST[i]);
        model->appendRow(program);
    }

    ui->listView->setModel(model);
}

void SettingsDialog::on_pushButtonAdd_clicked()
{
    QString newEntry = ui->lineEdit->text().simplified().toUpper();
    if(!newEntry.isEmpty() && !model->findItems(newEntry).size())
    {
        QStandardItem* item = new QStandardItem(newEntry);
        model->appendRow(item);
    }
    else if(model->findItems(newEntry).size())
    {
        int i;
        for(i=0; i<model->rowCount(); i++)
        {
            if(model->item(i)->text()==newEntry) break;
        }
        ui->listView->setCurrentIndex(model->index(i,0));
    }
}

void SettingsDialog::on_pushButtonDel_clicked()
{
    QModelIndexList listSelection = ui->listView->selectionModel()->selectedIndexes();
    for(int i=listSelection.size(); i>0; i--)
    {
        model->removeRows(listSelection[i-1].row(),1);
    }
}

void SettingsDialog::on_buttonBox_accepted()
{
    FONT_SIZE = ui->horizontalSliderFontSize->value();
    TITLE_MARGIN = ui->horizontalSliderTitleMargin->value();
    TITLE_SIZE = ui->horizontalSliderTitleSize->value();
    TECHNICAL_FONT_SIZE = ui->horizontalSliderTechnicalFontSize->value();
    DDNAME_FONT_SIZE = ui->horizontalSliderDDNameSize->value();
    DETAILED_WEIGHT = ui->horizontalSliderWeight->value();
    DEFAULT_HEIGHT = ui->horizontalSliderHeight->value();
    DEFAULT_WIDTH = ui->horizontalSliderWidth->value();
    SHADOW_OFFSET = ui->horizontalSliderShadow->value();
    ALIGN_GRID_X = ui->horizontalSliderGridWidth->value();
    ALIGN_GRID_Y = ui->horizontalSliderGridHeight->value();
    ALIGN_GRID_X_DIVISION = ui->horizontalSliderGridSubWidth->value();
    ALIGN_GRID_Y_DIVISION = ui->horizontalSliderGridSubHeight->value();
    ANCHOR_SIZE = ui->horizontalSliderAnchors->value();
    SYSIN_SIZE = ui->horizontalSliderSysin->value();
    CONDITION_SIZE = ui->horizontalSliderCondition->value();
    INSIDE_MARGIN = ui->horizontalSliderInsideMargin->value();
    RESIZE_STEP = ui->horizontalSliderRedimStep->value();

    //Si on change de workspace, on change les liens vers les fichiers récents
    if(!workspace.isEmpty() && WORKSPACE != workspace)
    {
        // On met le chemin absolu
        for(int i=0; i<RECENT_FILES.size(); i++)
        {
            QDir workspace(WORKSPACE);
            QString absolutePath = workspace.cleanPath(workspace.absoluteFilePath(RECENT_FILES[i]->getRelativePath()));
            RECENT_FILES[i]->setRelativePath(absolutePath);
        }

        // On change le Workspace
        WORKSPACE = workspace;

        // et on retrouve le chemin relatif
        for(int i=0; i<RECENT_FILES.size(); i++)
        {
            QDir workspace(WORKSPACE);
            QString relativePath = workspace.relativeFilePath(RECENT_FILES[i]->getRelativePath());
            RECENT_FILES[i]->setRelativePath(relativePath);
        }
    }

    QObject * parentWindow = parent();

    TECHNICAL_LIST.clear();
    for(int i=0; i<model->rowCount(); i++)
    {
        TECHNICAL_LIST.push_back(model->item(i)->text().toUpper());
    }
    //static_cast<MainWindow*>(parentWindow)->resetTechnicalState();

    // Redonner la couleur par défaut aux noeuds
    static_cast<MainWindow*>(parentWindow)->resetColors(colorRules, defaultFileColor, technicalColor, programColor, patternStepColor);

    SELECTED_COLOR = selectedNodeColor;
    FILE_HOVER_COLOR = fileHoverColor;
    NORMAL_FILE_COLOR = defaultFileColor;
    NORMAL_STEP_COLOR = programColor;
    PATTERN_STEP_COLOR = patternStepColor;
    TECHNICAL_STEP_COLOR = technicalColor;

    COLOR_RULES.clear();
    for(int i=0; i<colorRules.size(); i++)
    {
        COLOR_RULES.push_back(ColorRule(colorRules[i]->getClasse(),colorRules[i]->getColor()));
    }

    PCL_RULES.clear();
    for(int i=0; i<PCLRules.size(); i++)
    {
        PCL_RULES.push_back(PCLRule(PCLRules[i]->getProgram(),PCLRules[i]->getParameter(),PCLRules[i]->getSubparam(),PCLRules[i]->getClasse(),PCLRules[i]->getIsOut()));
    }

    CLASS_SEPARATOR = ui->lineEditSeparator->text();


    static_cast<MainWindow*>(parentWindow)->writeSettings();

    // Reconstruire les Sysin et les conditions au cas où leur taille ait été changée
    static_cast<MainWindow*>(parentWindow)->resetPolygons();

    // Redonner la couleur par défaut aux noeuds
    static_cast<MainWindow*>(parentWindow)->resetPositions();

}

void SettingsDialog::on_pushButtonInitGrid_clicked()
{
    ui->horizontalSliderGridWidth->setSliderPosition(150);
    ui->horizontalSliderGridHeight->setSliderPosition(75);
    ui->horizontalSliderGridSubWidth->setSliderPosition(4);
    ui->horizontalSliderGridSubHeight->setSliderPosition(2);
}

void SettingsDialog::on_pushButtonInitNodes_clicked()
{
    ui->horizontalSliderAnchors->setSliderPosition(8);
    ui->horizontalSliderSysin->setSliderPosition(4);
    ui->horizontalSliderCondition->setSliderPosition(15);
    ui->horizontalSliderShadow->setSliderPosition(5);
    ui->horizontalSliderWidth->setSliderPosition(100);
    ui->horizontalSliderHeight->setSliderPosition(50);
    ui->horizontalSliderWeight->setSliderPosition(3);
    ui->horizontalSliderRedimStep->setSliderPosition(5);
}

void SettingsDialog::on_pushButtonInitText_clicked()
{
    ui->horizontalSliderFontSize->setSliderPosition(14);
    ui->horizontalSliderTitleMargin->setSliderPosition(40);
    ui->horizontalSliderTitleSize->setSliderPosition(40);
    ui->horizontalSliderTechnicalFontSize->setSliderPosition(14);
    ui->horizontalSliderDDNameSize->setSliderPosition(12);
}

void SettingsDialog::on_pushButtonProgramColor_clicked()
{
    QColor color = QColorDialog::getColor(programColor, this, "Changer la couleur des noeuds programme");
    if(color.isValid())
    {
        programColor = color;
        QString qss = QString("background-color: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1, stop: 0 %1, stop: 1 #ffffff);").arg(programColor.name());
        ui->pushButtonProgramColor->setStyleSheet(qss);
        ui->pushButtonProgramColor->clearFocus();
    }
}

void SettingsDialog::on_pushButtonTechnicalColor_clicked()
{
    QColor color = QColorDialog::getColor(technicalColor, this, "Changer la couleur des noeuds programme technique");
    if(color.isValid())
    {
        technicalColor = color;
        QString qss = QString("background-color: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1, stop: 0 %1, stop: 1 #ffffff);").arg(technicalColor.name());
        ui->pushButtonTechnicalColor->setStyleSheet(qss);
        ui->pushButtonTechnicalColor->clearFocus();
    }
}

void SettingsDialog::on_pushButtonPatternStepColor_clicked()
{
    QColor color = QColorDialog::getColor(patternStepColor, this, "Changer la couleur des noeuds Pattern d'étape");
    if(color.isValid())
    {
        patternStepColor = color;
        QString qss = QString("background-color: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1, stop: 0 %1, stop: 1 #ffffff);").arg(patternStepColor.name());
        ui->pushButtonPatternStepColor->setStyleSheet(qss);
        ui->pushButtonPatternStepColor->clearFocus();
    }
}

void SettingsDialog::on_pushButtonDefaultFileColor_clicked()
{
    QColor color = QColorDialog::getColor(defaultFileColor, this, "Changer la couleur des noeuds fichier");
    if(color.isValid())
    {
        defaultFileColor = color;
        QString qss = QString("background-color: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1, stop: 0 %1, stop: 1 #ffffff);").arg(defaultFileColor.name());
        ui->pushButtonDefaultFileColor->setStyleSheet(qss);
        ui->pushButtonDefaultFileColor->clearFocus();
    }
}

void SettingsDialog::on_pushButtonSelectedColor_clicked()
{
    QColor color = QColorDialog::getColor(selectedNodeColor, this, "Changer la couleur des noeuds séléctionnés");
    if(color.isValid())
    {
        selectedNodeColor = color;
        QString qss = QString("background-color: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1, stop: 0 %1, stop: 1 #ffffff);").arg(selectedNodeColor.name());
        ui->pushButtonSelectedColor->setStyleSheet(qss);
        ui->pushButtonSelectedColor->clearFocus();
    }
}

void SettingsDialog::on_pushButtonFileHoverColor_clicked()
{
    QColor color = QColorDialog::getColor(fileHoverColor, this, "Changer la couleur de survol des noeuds fichier");
    if(color.isValid())
    {
        fileHoverColor = color;
        QString qss = QString("background-color: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1, stop: 0 %1, stop: 1 #ffffff);").arg(fileHoverColor.name());
        ui->pushButtonFileHoverColor->setStyleSheet(qss);
        ui->pushButtonFileHoverColor->clearFocus();
    }
}

void SettingsDialog::on_pushButtonAddColorRule_clicked()
{
    QString classe = ui->lineEditAddColorRule->text().simplified().toUpper();
    if(!classe.isEmpty() && !colorRuleExists(classe))
    {
        QColor color = QColorDialog::getColor();
        if(color.isValid())
        {
            ColorRuleWidget* newRule = new ColorRuleWidget(classe, color);
            ui->verticalLayoutColorRules->addWidget(newRule);
            connect(newRule,SIGNAL(deleteColorRule(QString)),this,SLOT(removeColorRule(QString)));
            colorRules.push_back(newRule);
            ui->lineEditAddColorRule->setText("");
        }
    }
}

void SettingsDialog::on_pushButtonAddPCLRule_clicked()
{
    QString program = ui->lineEditProgramPCLRule->text().simplified().toUpper();
    QString parameter = ui->lineEditParamPCLRule->text().simplified().toUpper();
    QString subparam = ui->lineEditSubParamPCLRule->text().simplified().toUpper();
    QString classe = ui->lineEditClassePCLRule->text().simplified().toUpper();
    bool isOut = ui->checkBoxOutputFile->isChecked();

    if(!program.isEmpty() && !parameter.isEmpty() && !PCLRuleExists(program,parameter,subparam))
    {
        PCLRuleWidget* newRule = new PCLRuleWidget(program, parameter, subparam, classe, isOut);
        ui->verticalLayoutPCLRules->addWidget(newRule);
        connect(newRule,SIGNAL(deletePCLRule(QString,QString,QString)),this,SLOT(removePCLRule(QString,QString,QString)));
        PCLRules.push_back(newRule);
        ui->lineEditProgramPCLRule->setText("");
        ui->lineEditParamPCLRule->setText("");
        ui->lineEditSubParamPCLRule->setText("");
        ui->lineEditClassePCLRule->setText("");
        ui->checkBoxOutputFile->setChecked(false);
    }
}

void SettingsDialog::allowAddNewColorRule(const QString &_text)
{
    QString classe = _text.simplified().toUpper();
    ui->pushButtonAddColorRule->setEnabled(!classe.isEmpty() && !colorRuleExists(classe));
}

void SettingsDialog::allowAddNewTechnical(const QString &_text)
{
    QString newEntry = _text.simplified().toUpper();
    ui->pushButtonAdd->setEnabled(!newEntry.isEmpty() && !model->findItems(newEntry).size());
}

bool SettingsDialog::colorRuleExists(const QString &_classe)
{
    for(int i=0; i<colorRules.size(); i++)
    {
        if(colorRules[i]->getClasse()==_classe) return true;
    }
    return false;
}

bool SettingsDialog::PCLRuleExists(const QString &_program, const QString &_parameter, const QString &_subparam)
{
    for(int i=0; i<PCLRules.size(); i++)
    {
        if(PCLRules[i]->getProgram()==_program && PCLRules[i]->getParameter()==_parameter && PCLRules[i]->getSubparam()==_subparam) return true;
    }
    return false;
}

void SettingsDialog::initializeColorButtonsColor()
{
    QString qssDefaultFile = QString("background-color: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1, stop: 0 %1, stop: 1 #ffffff);").arg(defaultFileColor.name());
    ui->pushButtonDefaultFileColor->setStyleSheet(qssDefaultFile);
    QString qssProgram = QString("background-color: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1, stop: 0 %1, stop: 1 #ffffff);").arg(programColor.name());
    ui->pushButtonProgramColor->setStyleSheet(qssProgram);
    QString qssTechnical = QString("background-color: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1, stop: 0 %1, stop: 1 #ffffff);").arg(technicalColor.name());
    ui->pushButtonTechnicalColor->setStyleSheet(qssTechnical);
    QString qssSelected = QString("background-color: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1, stop: 0 %1, stop: 1 #ffffff);").arg(selectedNodeColor.name());
    ui->pushButtonSelectedColor->setStyleSheet(qssSelected);
    QString qssFileHover = QString("background-color: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1, stop: 0 %1, stop: 1 #ffffff);").arg(fileHoverColor.name());
    ui->pushButtonFileHoverColor->setStyleSheet(qssFileHover);
    QString qssPatternHover = QString("background-color: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1, stop: 0 %1, stop: 1 #ffffff);").arg(patternStepColor.name());
    ui->pushButtonPatternStepColor->setStyleSheet(qssPatternHover);
}

void SettingsDialog::on_pushButtonInitColor_clicked()
{
    technicalColor = QColor(90,255,60);
    programColor = QColor(120,255,255);
    defaultFileColor = QColor(255,255,100);
    patternStepColor = QColor(255,220,100);
    selectedNodeColor = QColor(Qt::yellow);
    fileHoverColor = QColor(Qt::red);
    initializeColorButtonsColor();
}

void SettingsDialog::on_pushButtonInitTechnical_clicked()
{
    //!\\ modifier aussi dans MainWindow::resetTechnicalPrograms()
    QVector<QString> defaultList;
    defaultList.push_back("SORT");
    defaultList.push_back("MERGE");
    defaultList.push_back("ADRDSSU");
    defaultList.push_back("IEFBR14");
    defaultList.push_back("IEBGENER");
    defaultList.push_back("IDCAMS");
    defaultList.push_back("GVEXPORT");
    defaultList.push_back("CFTUTIL");
    defaultList.push_back("DSNTIAUL");

    model = new QStandardItemModel;
    for(int i=0; i<defaultList.size(); i++)
    {
        QStandardItem* program = new QStandardItem(defaultList[i]);
        model->appendRow(program);
    }

    ui->listView->setModel(model);

}

void SettingsDialog::on_pushButtonInitColorRules_clicked()
{
    for(int i=colorRules.size(); i>0; i--)
    {
        removeColorRule(colorRules[i-1]->getClasse());
    }
    //!\\ modifier aussi dans MainWindow::resetColorRules()
    ColorRuleWidget *report = new ColorRuleWidget("ETAT",QColor(Qt::white));
    connect(report,SIGNAL(deleteColorRule(QString)),this,SLOT(removeColorRule(QString)));
    ui->verticalLayoutColorRules->addWidget(report);
    colorRules.push_back(report);

    ColorRuleWidget *working = new ColorRuleWidget("XP",QColor(Qt::lightGray));
    connect(working,SIGNAL(deleteColorRule(QString)),this,SLOT(removeColorRule(QString)));
    ui->verticalLayoutColorRules->addWidget(working);
    colorRules.push_back(working);

    ColorRuleWidget *working2 = new ColorRuleWidget("XP2",QColor(Qt::lightGray));
    connect(working2,SIGNAL(deleteColorRule(QString)),this,SLOT(removeColorRule(QString)));
    ui->verticalLayoutColorRules->addWidget(working2);
    colorRules.push_back(working2);
}

void SettingsDialog::on_pushButtonInitPCLRules_clicked()
{
    for(int i=PCLRules.size(); i>0; i--)
    {
        removePCLRule(PCLRules[i-1]->getProgram(),PCLRules[i-1]->getParameter(),PCLRules[i-1]->getSubparam());
    }
    //!\\ modifier aussi dans MainWindow::resetPCLRules()

    PCLRuleWidget *CFTUTIL_Ficenv = new PCLRuleWidget("CFTUTIL","PATKW","FICENV","XP",0);
    connect(CFTUTIL_Ficenv,SIGNAL(deletePCLRule(QString,QString,QString)),this,SLOT(removePCLRule(QString,QString,QString)));
    ui->verticalLayoutPCLRules->addWidget(CFTUTIL_Ficenv);
    PCLRules.push_back(CFTUTIL_Ficenv);
}

void SettingsDialog::on_pushButtonWorkspace_clicked()
{
    QString mydir = QFileDialog::getExistingDirectory(this, tr("Choisir l'espace de travail"), EXE_PATH.path());
    if(!mydir.isEmpty())
    {
        workspace = mydir;
        ui->pushButtonWorkspace->setText(workspace);
    }
}
