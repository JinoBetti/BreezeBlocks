#include "searchdockwidget.h"
#include "ui_searchdockwidget.h"
#include "propertiesdialog.h"

SearchDockWidget::SearchDockWidget(QWidget *parent) :
    QDockWidget(parent),
    ui(new Ui::SearchDockWidget),
    main(dynamic_cast<MainWindow*>(parent))
{
    ui->setupUi(this);
    ui->lineEdit->setFocus();
    ui->groupBox->setChecked(false);
    ui->labelResult->setMaximumHeight(50);

    connect(ui->checkBoxCase,SIGNAL(clicked()),this,SLOT(search()));
    //connect(ui->checkBoxSearchAll,SIGNAL(clicked()),this,SLOT(search()));
    connect(ui->checkBoxCustomCom,SIGNAL(clicked()),this,SLOT(search()));
    connect(ui->checkBoxCustomText,SIGNAL(clicked()),this,SLOT(search()));
    connect(ui->checkBoxExact,SIGNAL(clicked()),this,SLOT(search()));
    connect(ui->checkBoxFileClass,SIGNAL(clicked()),this,SLOT(search()));
    connect(ui->checkBoxFileCom,SIGNAL(clicked()),this,SLOT(search()));
    connect(ui->checkBoxFileName,SIGNAL(clicked()),this,SLOT(search()));
    connect(ui->checkBoxFilePrecision,SIGNAL(clicked()),this,SLOT(search()));
    connect(ui->checkBoxFunctionCall,SIGNAL(clicked()),this,SLOT(search()));
    connect(ui->checkBoxFunctionLevel,SIGNAL(clicked()),this,SLOT(search()));
    connect(ui->checkBoxFunctionCom,SIGNAL(clicked()),this,SLOT(search()));
    connect(ui->checkBoxFunctionName,SIGNAL(clicked()),this,SLOT(search()));
    connect(ui->checkBoxFunctionPrecision,SIGNAL(clicked()),this,SLOT(search()));
    connect(ui->checkBoxStepCom,SIGNAL(clicked()),this,SLOT(search()));
    connect(ui->checkBoxStepCond,SIGNAL(clicked()),this,SLOT(search()));
    connect(ui->checkBoxStepData,SIGNAL(clicked()),this,SLOT(search()));
    connect(ui->checkBoxStepName,SIGNAL(clicked()),this,SLOT(search()));
    connect(ui->checkBoxStepPrecision,SIGNAL(clicked()),this,SLOT(search()));
    connect(ui->checkBoxStepProg,SIGNAL(clicked()),this,SLOT(search()));
    connect(ui->groupBoxCustom,SIGNAL(clicked()),this,SLOT(search()));
    connect(ui->groupBoxFile,SIGNAL(clicked()),this,SLOT(search()));
    connect(ui->groupBoxFunction,SIGNAL(clicked()),this,SLOT(search()));
    connect(ui->groupBoxGeneral,SIGNAL(clicked()),this,SLOT(search()));
    connect(ui->groupBoxLink,SIGNAL(clicked()),this,SLOT(search()));
    connect(ui->groupBoxStep,SIGNAL(clicked()),this,SLOT(search()));
}

SearchDockWidget::~SearchDockWidget(){ delete ui; }

void SearchDockWidget::setLabel(const QString &_newLabel){ ui->labelResult->setText(_newLabel);}

void SearchDockWidget::clearResults()
{
    results.clear();
    ui->listWidget->clear();
}

void SearchDockWidget::searchInNode(const QString &_searched, const QString &_in, NodeGraphic *_node, int _place, const QString &_type, bool _dontskip)
{
    if(!_dontskip) return;

    Qt::CaseSensitivity sensitivity = Qt::CaseSensitive;
    bool strict = ui->checkBoxExact->isChecked();
    if(!ui->checkBoxCase->isChecked()) sensitivity = Qt::CaseInsensitive;

    if(_in.contains(_searched,sensitivity) && ui->checkBoxCustomText->isChecked())
    {
        if(strict && !(_in.compare(_searched,sensitivity)==0 && ui->checkBoxCustomText->isChecked())) return;

        SearchResult* result = new SearchResult(_node, _place, _type);
        result->setCompleteString(_in);
        results.push_back(result);
    }
}

void SearchDockWidget::searchInLink(const QString &_searched, const QString &_in, LinkGraphic *_link)
{
    Qt::CaseSensitivity sensitivity = Qt::CaseSensitive;
    bool strict = ui->checkBoxExact->isChecked();
    if(!ui->checkBoxCase->isChecked()) sensitivity = Qt::CaseInsensitive;

    if(_in.contains(_searched,sensitivity) && ui->checkBoxCustomText->isChecked())
    {
        if(strict && !(_in.compare(_searched,sensitivity)==0 && ui->checkBoxCustomText->isChecked())) return;

        SearchResult* result = new SearchResult(_link, ResultPlace::Link);
        result->setCompleteString(_in);
        results.push_back(result);
    }
}

void SearchDockWidget::setGraphics(CustomGraphics *value)
{
    graphics = value;
    animation = new QParallelAnimationGroup(graphics);
    connect(animation,SIGNAL(stateChanged(QAbstractAnimation::State,QAbstractAnimation::State)),
            this, SLOT(monitorAnimation(QAbstractAnimation::State,QAbstractAnimation::State)));
}

void SearchDockWidget::search()
{
    ui->pushButton->setEnabled(!graphics->getCurrentFilename().isEmpty());
    //caractéristiques du dernier enregistrement
    ui->labelUser->setText(graphics->getModificationUser());
    ui->labelDate->setText(graphics->getModificationDate().toString());
    ui->labelTime->setText(graphics->getModificationTime().toString());

    // Supprimer les résultats de la précédente recherche
    clearResults();

//    QVector<CustomGraphics*> graphicsToBeSearched;

//    if(ui->checkBoxSearchAll->isChecked())
//    {
//        for(int i=0; i<main->getTabCount(); i++) graphicsToBeSearched.push_back(main->getGraphics(i));
//    }
//    else graphicsToBeSearched.push_back(main->getCurrentGraphics());

    CustomGraphics* graphicsToBeSet = main->getCurrentGraphics();
    if(!graphicsToBeSet) return;
    setGraphics(graphicsToBeSet);

    QString searchedLine = ui->lineEdit->text();

    // Ne pas chercher une chaine vide qui serait trouvée partout
    if(searchedLine.simplified().isEmpty())
    {
        ui->labelResult->setText("Chaine cherchée vide");
        ui->lineEdit->setFocus();
        ui->lineEdit->selectAll();
        return;
    }


    // Recherche dans les noeuds
    foreach(NodeGraphic* node, graphics->getNodeGraphics())
    {
       if(node->getType() == "CUSTOM")
       {
          if(!ui->groupBoxCustom->isChecked()) continue;
          searchInNode(searchedLine, node->getName(), node, ResultPlace::Text, node->getType(), ui->checkBoxCustomText->isChecked());
          searchInNode(searchedLine, node->getComment(), node, ResultPlace::Comment, node->getType(), ui->checkBoxCustomCom->isChecked());
       }
       if(node->getType() == "STEP")
       {
           if(!ui->groupBoxStep->isChecked()) continue;
           searchInNode(searchedLine, node->getProgram(), node, ResultPlace::Program, node->getType(), ui->checkBoxStepProg->isChecked());
           searchInNode(searchedLine, node->getComment(), node, ResultPlace::Comment, node->getType(), ui->checkBoxStepCom->isChecked());
           searchInNode(searchedLine, node->getSysinValue(), node, ResultPlace::DataCards, node->getType(), ui->checkBoxStepData->isChecked());
           searchInNode(searchedLine, node->getConditionValue(), node, ResultPlace::Condition, node->getType(), ui->checkBoxStepCond->isChecked());
           searchInNode(searchedLine, node->getPrecision(), node, ResultPlace::Precision, node->getType(), ui->checkBoxStepPrecision->isChecked());
           searchInNode(searchedLine, node->getName(), node, ResultPlace::Name, node->getType(), ui->checkBoxStepName->isChecked());
       }
       if(node->getType() == "DCLF")
       {
            if(!ui->groupBoxFile->isChecked()) continue;
            searchInNode(searchedLine, node->getName(), node, ResultPlace::Name, node->getType(), ui->checkBoxFileName->isChecked());
            searchInNode(searchedLine, node->getClass(), node, ResultPlace::Class, node->getType(), ui->checkBoxFileClass->isChecked());
            searchInNode(searchedLine, node->getPrecision(), node, ResultPlace::Precision, node->getType(), ui->checkBoxFilePrecision->isChecked());
            searchInNode(searchedLine, node->getComment(), node, ResultPlace::Comment, node->getType(), ui->checkBoxFileCom->isChecked());
       }
       if(node->getType() == "FUNCTION")
       {
           if(!ui->groupBoxFunction->isChecked()) continue;
           searchInNode(searchedLine, node->getCallMethod(),node, ResultPlace::CallMethod, node->getType(), ui->checkBoxFunctionCall->isChecked());
           searchInNode(searchedLine, node->getLevel(),node, ResultPlace::Level, node->getType(), ui->checkBoxFunctionLevel->isChecked());
           searchInNode(searchedLine, node->getComment(),node, ResultPlace::Comment, node->getType(), ui->checkBoxFunctionCom->isChecked());
           searchInNode(searchedLine, node->getName(),node, ResultPlace::Name, node->getType(), ui->checkBoxFunctionName->isChecked());
           searchInNode(searchedLine, node->getPrecision(),node, ResultPlace::Precision, node->getType(), ui->checkBoxFunctionPrecision->isChecked());
       }
    }

    // Recherche dans les liens
    if(ui->groupBoxLink->isChecked()) { foreach(LinkGraphic* link, graphics->getLinkGraphics()) searchInLink(searchedLine, link->getDDName(),link); }

    // Affichage du nombre de résultats
    ui->labelResult->setText(QString::number(results.size()) + " résultats");

    // Création de la liste
    connect(ui->listWidget,SIGNAL(clicked(QModelIndex)),this, SLOT(animate(QModelIndex)));
    foreach(SearchResult* result, results)
    {
        QListWidgetItem* item = new QListWidgetItem(result->getResultString());
        item->setToolTip(result->getCompleteString());
        ui->listWidget->addItem(item);
    }
}

QSequentialAnimationGroup* SearchDockWidget::hilight(NodeGraphic* _node, int _duration)
{
    float factor = 1.5;
    int loops = 1;

    QParallelAnimationGroup* homotetieUp = new QParallelAnimationGroup(graphics);
    QPropertyAnimation *scaleUpW = new QPropertyAnimation(_node, "width");
    scaleUpW->setDuration(_duration);
    scaleUpW->setEndValue(_node->getWidth()*factor);
    scaleUpW->setLoopCount(loops);

    QPropertyAnimation *scaleUpH = new QPropertyAnimation(_node, "height");
    scaleUpH->setDuration(_duration);
    scaleUpH->setEndValue(_node->getHeight()*factor);
    scaleUpH->setLoopCount(loops);

    QPropertyAnimation *animX = new QPropertyAnimation(_node, "x");
    animX->setDuration(_duration);
    animX->setEndValue(_node->pos().x()-_node->getWidth()*(factor-1)/2);
    animX->setLoopCount(loops);

    QPropertyAnimation *animY = new QPropertyAnimation(_node, "y");
    animY->setDuration(_duration);
    animY->setEndValue(_node->pos().y()-_node->getHeight()*(factor-1)/2);
    animY->setLoopCount(loops);

    homotetieUp->addAnimation(scaleUpW);
    homotetieUp->addAnimation(scaleUpH);
    homotetieUp->addAnimation(animX);
    homotetieUp->addAnimation(animY);

    QParallelAnimationGroup* homotetieDown = new QParallelAnimationGroup(graphics);

    QPropertyAnimation *scaleDownW = new QPropertyAnimation(_node, "width");
    scaleDownW->setDuration(_duration);
    scaleDownW->setEndValue(_node->getWidth());
    scaleDownW->setLoopCount(loops);

    QPropertyAnimation *scaleDownH = new QPropertyAnimation(_node, "height");
    scaleDownH->setDuration(_duration);
    scaleDownH->setEndValue(_node->getHeight());
    scaleDownH->setLoopCount(loops);

    QPropertyAnimation *animXDown = new QPropertyAnimation(_node, "x");
    animXDown->setDuration(_duration);
    animXDown->setEndValue(_node->pos().x());
    animXDown->setLoopCount(loops);

    QPropertyAnimation *animYDown = new QPropertyAnimation(_node, "y");
    animYDown->setDuration(_duration);
    animYDown->setEndValue(_node->pos().y());
    animYDown->setLoopCount(loops);

    homotetieDown->addAnimation(scaleDownW);
    homotetieDown->addAnimation(scaleDownH);
    homotetieDown->addAnimation(animXDown);
    homotetieDown->addAnimation(animYDown);

    QSequentialAnimationGroup* completeAnimation = new QSequentialAnimationGroup(graphics);
    completeAnimation->addAnimation(homotetieUp);
    completeAnimation->addAnimation(homotetieDown);

    return completeAnimation;
}

void SearchDockWidget::monitorAnimation(QAbstractAnimation::State _new, QAbstractAnimation::State _old)
{
    if(_new == QAbstractAnimation::Running) ui->listWidget->setEnabled(false);
    if(_new == QAbstractAnimation::Stopped &&
       _old == QAbstractAnimation::Running)
    {
        ui->listWidget->setEnabled(true);
        getGraphics()->update();
    }
}

void SearchDockWidget::setVisible(bool _visibility)
{
    QDockWidget::setVisible(_visibility);
    ui->lineEdit->setFocus();
    ui->lineEdit->selectAll();
}

SearchResult::SearchResult(NodeGraphic *_node, int _place, const QString &_type)
{
    node = _node;
    link = 0;
    place = _place;
    type = _type;
}

SearchResult::SearchResult(LinkGraphic *_link, int _place)
{
    node = 0;
    link = _link;
    place = _place;
    type = "LINK";
}

QString SearchResult::getResultString()
{
    QString nameAsString = type + " ";
    if(node) nameAsString += node->getName().left(15);
    else nameAsString += link->getInput()->getName().left(10)+" -> "+link->getOutput()->getName().left(10);

    QString placeAsString;
    switch(place)
    {
    case ResultPlace::CallMethod:
        placeAsString = "Méthode d'appel : ";
        break;
    case ResultPlace::Class:
        placeAsString = "Classe de fichier : ";
        break;
    case ResultPlace::Comment:
        placeAsString = "Commentaire : ";
        break;
    case ResultPlace::Precision:
        placeAsString = "Précisions : ";
        break;
    case ResultPlace::Condition:
        placeAsString = "Condition : ";
        break;
    case ResultPlace::DataCards:
        placeAsString = "Cartes Data : ";
        break;
    case ResultPlace::Text:
        placeAsString = "Texte : ";
        break;
    case ResultPlace::Name:
        placeAsString = "Nom : ";
        break;
    case ResultPlace::Program:
        placeAsString = "Programme : ";
        break;
    case ResultPlace::Link:
        placeAsString = "Lien : ";
        break;
    case ResultPlace::Level:
        placeAsString = "Niveau : ";
        break;
    default:
        placeAsString = "Inconnu : ";
    }

    QString beginning;

    beginning = completeString.left(25);
    if(completeString.size()>25) beginning += "...";

    return nameAsString + "\n   " + placeAsString +"\n ->"+ beginning;
}

void SearchDockWidget::on_groupBox_toggled(bool _checked)
{
    ui->groupBoxCustom->setVisible(_checked);
    ui->groupBoxLink->setVisible(_checked);
    ui->groupBoxStep->setVisible(_checked);
    ui->groupBoxFile->setVisible(_checked);
    ui->groupBoxFunction->setVisible(_checked);
    ui->groupBoxGeneral->setVisible(_checked);
}

void SearchDockWidget::animate(const QModelIndex &_index)
{
    getGraphics()->getMain()->zoom100pc();

    int i = _index.row();
    if(results[i]->getNode()) getGraphics()->centerOn(results[i]->getNode()->center());
    else if(results[i]->getLink()) getGraphics()->centerOn(results[i]->getLink()->getInput()->center());
    animation->clear();
    if(results[i]->getNode()) animation->addAnimation(hilight(results[i]->getNode()));
    else if(results[i]->getLink())
    {
        animation->addAnimation(hilight(results[i]->getLink()->getInput()));
        animation->addAnimation(hilight(results[i]->getLink()->getOutput()));
    }
    animation->start();
}

void SearchDockWidget::on_pushButton_clicked()
{
    if(main->getTabWidget()->currentIndex() == -1) return;
    PropertiesDialog dialog(graphics->getCurrentFilename(), graphics);
    dialog.exec();
}
