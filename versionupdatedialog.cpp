#include "versionupdatedialog.h"
#include "ui_versionupdatedialog.h"
#include "customgraphics.h"
#include <QFileDialog>

VersionUpdateDialog::VersionUpdateDialog(QVector<NodeGraphic*> oldNodes, QVector<NodeGraphic*> newNodes, CustomGraphics* _oldGraphics, CustomGraphics* _newGraphics, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::VersionUpdateDialog),
    oldGraphics(_oldGraphics),
    newGraphics(_newGraphics)
{
    ui->setupUi(this);

    //Trier oldSteps et newSteps
    qSort(oldNodes.begin(),oldNodes.end(), &VersionUpdateDialog::ebcdicLessThan);
    qSort(newNodes.begin(),newNodes.end(), &VersionUpdateDialog::ebcdicLessThan);

    // Remplissage de la liste des anciens noeuds
    foreach(NodeGraphic* node, oldNodes)
    {
        if(node->getType()=="STEP")
        {
            oldSteps.push_back(node);
            QListWidgetItem* item = new QListWidgetItem(node->getName()+" ("+node->getProgram() + ")");
            ui->listWidgetOld->addItem(item);
        }
    } 

    // Remplissage de la liste des nouveaux noeuds
    foreach(NodeGraphic* node, newNodes)
    {
        if(node->getType()=="STEP")
        {
            newSteps.push_back(node);
            QListWidgetItem* item = new QListWidgetItem(node->getName() + " (" + node->getProgram() + ")");
            ui->listWidgetNew->addItem(item);
        }
    }

    // Détection des correspondances entre les noeuds
    detectAssociations();

    // Positionnement sur le premier item de chaque liste
    if(ui->listWidgetOld->count()) ui->listWidgetOld->setCurrentRow(0);
    if(ui->listWidgetNew->count()) ui->listWidgetNew->setCurrentRow(0);

    // Paramètrage de la fenêtre
    Qt::WindowFlags flags = Qt::Window | Qt::WindowCloseButtonHint | Qt::WindowMaximizeButtonHint | Qt::WindowMinimizeButtonHint;
    setWindowFlags(flags);

    showMaximized();

}

bool VersionUpdateDialog::ebcdicLessThan(NodeGraphic* &n1, NodeGraphic* &n2)
{
    /// \todo : changer l'ordre pour coller sur l'ordre EBCDIC
    return (n1->getName().toLower() < n2->getName().toLower());
}

VersionUpdateDialog::~VersionUpdateDialog(){ delete ui; }

void VersionUpdateDialog::detectAssociations()
{
    //Première boucle : associer les noeuds strictements identiques
    for(int i=0; i<oldSteps.size(); i++)
    {
        for(int j=0; j<newSteps.size(); j++)
        {
            if(compareNodes(oldSteps[i], newSteps[j])==0)
            {
                if(!findAssociation(newSteps[j]))
                {
                    NodeAssociation *association =  new NodeAssociation(oldSteps[i],newSteps[j]);
                    association->initFiles(oldGraphics,newGraphics);
                    addAssociation(association);
                    break;
                }
            }
        }
    }

    //Deuxième boucle : associer les noeuds identiques au nom de step près
    for(int i=0; i<oldSteps.size(); i++)
    {
        for(int j=0; j<newSteps.size(); j++)
        {
            if(compareNodes(oldSteps[i], newSteps[j])==1)
            {
                if(!findAssociation(newSteps[j]) && !findAssociation(oldSteps[i]))
                {
                    NodeAssociation *association =  new NodeAssociation(oldSteps[i],newSteps[j]);
                    association->initFiles(oldGraphics,newGraphics);
                    addAssociation(association);
                    break;
                }
            }
        }
    }

    //Troisième boucle : associer les noeuds ressemblants
    for(int i=0; i<oldSteps.size(); i++)
    {
        for(int j=0; j<newSteps.size(); j++)
        {
            if(compareNodes(oldSteps[i], newSteps[j])==2)
            {
                if(!findAssociation(newSteps[j]) && !findAssociation(oldSteps[i]))
                {
                    NodeAssociation *association =  new NodeAssociation(oldSteps[i],newSteps[j]);
                    association->initFiles(oldGraphics,newGraphics);
                    addAssociation(association);
                    break;
                }
            }
        }
    }

    setStepsBackgroundColor();

}


/// valeurs de retour :
/// 0 : strictement identiques => vert
/// 1 : identiques au nom de step près => jaune
/// 2 : Pas de différence majeure : entrée sorties identiques et nom de programme identique => orange
/// -1 : autres cas
int VersionUpdateDialog::compareNodes(NodeGraphic* _old, NodeGraphic* _new)
{

    /// CAS 0 ou 1 : identiques au renommage/précision près
    if(_old->getProgram()==_new->getProgram() &&
       _old->getConditionValue()==_new->getConditionValue() &&
       _old->getSysinValue()==_new->getSysinValue() &&
       _old->getComment()==_new->getComment())
    {
        // On vérifie que les entrées sorties utilisées sont également identiques
        QVector<LinkGraphic*> oldLinks = oldGraphics->getLinks(_old);
        QVector<LinkGraphic*> newLinks = newGraphics->getLinks(_new);

        bool filesIdentical = true;
        bool warningDotLink = false;
        int skippedNodes = 0;
        for(int i=0; i<oldLinks.size(); i++)
        {
            if(!((oldLinks[i]->getInput()->getType() == "DCLF" && oldLinks[i]->getOutput()->getType() == "STEP")
               ||(oldLinks[i]->getInput()->getType() == "STEP" && oldLinks[i]->getOutput()->getType() == "DCLF")))
            {
                skippedNodes++;
                continue;
            }

            bool found = false;
            for(int j=0; j<newLinks.size(); j++)
            {
                if(oldLinks[i]->getDDName()==newLinks[j]->getDDName())
                {
                    QString oldDCLF = oldLinks[i]->getInput()==_old ? oldLinks[i]->getOutput()->getName() : oldLinks[i]->getInput()->getName();
                    QString newDCLF = newLinks[j]->getInput()==_new ? newLinks[j]->getOutput()->getName() : newLinks[j]->getInput()->getName();

                    if(oldDCLF == newDCLF) found = true;
                }
            }
            // Si un DDNAME de l'ancienne liste n'est pas trouvé
            if(!found)
            {
                if(oldLinks[i]->getPenStyle()==Qt::SolidLine) filesIdentical = false;
                else
                {
                    skippedNodes++;
                    warningDotLink = true;
                }
            }

        }

        // gestion de la différence de nombre de liens (sans compter les liens forcément ajoutés à la main)
        if(oldLinks.size()-skippedNodes!=newLinks.size()) filesIdentical=false;

        if(filesIdentical)
        {
            if(warningDotLink) return 1;
            if( _old->getPrecision()==_new->getPrecision() &&
                _old->getName()==_new->getName()) return 0;
            else return 1;
        }

    }

    /// CAS 2 : ressemblant au moins le nom de pgm identiques et toutes les entrées sorties (au moins une)
    else if(_old->getProgram()==_new->getProgram())
    {
        // On vérifie que les entrées sorties utilisées sont également identiques
        QVector<LinkGraphic*> oldLinks = oldGraphics->getLinks(_old);
        QVector<LinkGraphic*> newLinks = newGraphics->getLinks(_new);

        bool filesIdentical = true;

        int skippedNodes = 0;
        for(int i=0; i<oldLinks.size(); i++)
        {
            // Lien qui ne peut pas être créé automatiquement : on le passe
            if(!((oldLinks[i]->getInput()->getType() == "DCLF" && oldLinks[i]->getOutput()->getType() == "STEP")
               ||(oldLinks[i]->getInput()->getType() == "STEP" && oldLinks[i]->getOutput()->getType() == "DCLF")))
            {
                skippedNodes++;
                continue;
            }

            bool found = false;
            for(int j=0; j<newLinks.size(); j++)
            {
                if(oldLinks[i]->getDDName()==newLinks[j]->getDDName())
                {
                    QString oldDCLF = oldLinks[i]->getInput()==_old ? oldLinks[i]->getOutput()->getName() : oldLinks[i]->getInput()->getName();
                    QString newDCLF = newLinks[j]->getInput()==_new ? newLinks[j]->getOutput()->getName() : newLinks[j]->getInput()->getName();

                    if(oldDCLF == newDCLF) found = true;
                }
            }
            // Si un DDNAME de l'ancienne liste n'est pas trouvé
            if(!found)
            {
                if(oldLinks[i]->getPenStyle()==Qt::SolidLine) filesIdentical = false;
                else skippedNodes++;
            }
        }

        // gestion de la différence de nombre de liens (sans compter les liens forcément ajoutés à la main)
        if(oldLinks.size()-skippedNodes!=newLinks.size()) filesIdentical=false;

        if(filesIdentical)
        {
            if(_old->getName()==_new->getName() || newLinks.size()>0) return 2;
        }
    }
    /// Autres cas
    return -1;
}

void VersionUpdateDialog::displayProperty(QLabel* _value, QLabel* _label, const QString &_stringValue)
{
    if(_stringValue.simplified().isEmpty())
    {
        _value->setText("");
        _value->hide();
        _label->hide();
    }
    else
    {
        _value->show();
        _label->show();
        _value->setText(_stringValue);
    }
}

void VersionUpdateDialog::colorProperty(QLabel* _old, QLabel* _new, bool _color)
{
    if(!_color)
    {
        _new->setStyleSheet("QLabel { background-color : transparent; }");
        return;
    }
    _old->setStyleSheet("QLabel { background-color : transparent; }");
    if(_old->text()==_new->text()) _new->setStyleSheet("QLabel { background-color : lightgreen; }");
    else
    {
        if(!_new->text().isEmpty()) _new->setStyleSheet("QLabel { background-color : red; }");
        else _old->setStyleSheet("QLabel { background-color : red; }");
    }
}

void VersionUpdateDialog::on_pushButtonAssociate_clicked()
{
    if(findAssociation(oldSteps[ui->listWidgetOld->currentRow()]))
    {
        for(int i=0; i<associations.size();i++)
        {
            if(associations[i]->getOldNode() == oldSteps[ui->listWidgetOld->currentRow()])
            {
                associations.erase(associations.begin()+i, associations.begin()+i+1);
            }
        }
        ui->pushButtonAssociate->setText("Associer");
    }
    else
    {
        NodeAssociation *association =  new NodeAssociation(oldSteps[ui->listWidgetOld->currentRow()],newSteps[ui->listWidgetNew->currentRow()]);
        association->initFiles(oldGraphics,newGraphics);
        association->setForced(true);
        addAssociation(association);
        displayAssociation(association);

        ui->pushButtonAssociate->setText("Dissocier");
    }

    setStepsBackgroundColor();

}

void VersionUpdateDialog::setStepsBackgroundColor()
{
    for(int i=0; i<ui->listWidgetNew->count(); i++)
    {
        if(findAssociation(newSteps[i]))
        {
            if(findAssociation(newSteps[i])->getForced()) ui->listWidgetNew->item(i)->setBackgroundColor(Qt::cyan); // vert
            else
            {
                int comparison = compareNodes(findAssociation(newSteps[i])->getOldNode(), newSteps[i]);
                if(comparison == 0) ui->listWidgetNew->item(i)->setBackgroundColor(Qt::green); // vert
                else if(comparison == 1) ui->listWidgetNew->item(i)->setBackgroundColor(Qt::yellow);
                else if(comparison == 2) ui->listWidgetNew->item(i)->setBackgroundColor(QColor(255,160,0)); // orange
                else ui->listWidgetNew->item(i)->setBackgroundColor(Qt::lightGray);
            }
        }
        else if(newNodesSkipped.contains(newSteps[i])) ui->listWidgetNew->item(i)->setBackgroundColor(Qt::red);//rouge
        else ui->listWidgetNew->item(i)->setBackgroundColor(Qt::blue);//bleu
    }

    for(int i=0; i<ui->listWidgetOld->count(); i++)
    {
        if(findAssociation(oldSteps[i]))
        {
            if(findAssociation(oldSteps[i])->getForced()) ui->listWidgetOld->item(i)->setBackgroundColor(Qt::cyan); // vert
            else
            {
                int comparison = compareNodes(oldSteps[i], findAssociation(oldSteps[i])->getNewNode());
                if(comparison == 0) ui->listWidgetOld->item(i)->setBackgroundColor(Qt::green);// vert
                else if(comparison == 1) ui->listWidgetOld->item(i)->setBackgroundColor(Qt::yellow);// jaune
                else if(comparison == 2) ui->listWidgetOld->item(i)->setBackgroundColor(QColor(255,160,0));// orange
                else  ui->listWidgetOld->item(i)->setBackgroundColor(Qt::lightGray);
            }
        }
        else if(oldNodesKept.contains(oldSteps[i])) ui->listWidgetOld->item(i)->setBackgroundColor(Qt::blue);// bleu
        else ui->listWidgetOld->item(i)->setBackgroundColor(Qt::red);// rouge
    }
}

void VersionUpdateDialog::displayAssociation(NodeAssociation *_association)
{
    ui->labelComTarget->setText(_association->getTargetComment());
    ui->labelPrecisionTarget->setText(_association->getTargetPrecision());
    ui->labelProgTarget->setText(_association->getTargetProgram());
    ui->labelCondTarget->setText(_association->getTargetCondition());
    ui->labelDataTarget->setText(_association->getTargetData());
    ui->labelNameTarget->setText(_association->getTargetStepName());

    ui->switchCom->setVisible(_association->getNewNode()->getComment()!=_association->getOldNode()->getComment());
    ui->switchCondition->setVisible(_association->getNewNode()->getConditionValue()!=_association->getOldNode()->getConditionValue());
    ui->switchPrecision->setVisible(_association->getNewNode()->getPrecision()!=_association->getOldNode()->getPrecision());
    ui->switchProgram->setVisible(_association->getNewNode()->getProgram()!=_association->getOldNode()->getProgram());
    ui->switchData->setVisible(_association->getNewNode()->getSysinValue()!=_association->getOldNode()->getSysinValue());

    ui->listWidgetFileTarget->clear();
    foreach(TargetFile* file, _association->getTargetFiles())
    {
        QListWidgetItem* item = new QListWidgetItem(file->getDdName()+" : "+file->getDclfName()+" ("+(file->getIsOutput() ? "Sortie" : "Entrée")+")");
        if(file->getReport())
        {
            if(file->getWarning()) item->setBackgroundColor(Qt::yellow);
            else item->setBackgroundColor(Qt::green);
        }
        else item->setBackgroundColor(Qt::red);
        ui->listWidgetFileTarget->addItem(item);
    }

    /// todo display files
}

NodeAssociation *VersionUpdateDialog::currentAssociation()
{
    NodeGraphic* node = newSteps[ui->listWidgetNew->currentRow()];

    foreach(NodeAssociation *association, associations)
    { if(association->getNewNode()==node) return association; }

    return 0;
}

void VersionUpdateDialog::noDisplay()
{
    ui->labelComTarget->setText("");
    ui->labelPrecisionTarget->setText("");
    ui->labelProgTarget->setText("");
    ui->labelCondTarget->setText("");
    ui->labelDataTarget->setText("");
    ui->labelNameTarget->setText("");

    ui->switchCom->setVisible(false);
    ui->switchCondition->setVisible(false);
    ui->switchPrecision->setVisible(false);
    ui->switchProgram->setVisible(false);
    ui->switchData->setVisible(false);
    ui->listWidgetFileTarget->clear();
}

void VersionUpdateDialog::on_listWidgetOld_currentRowChanged(int currentRow)
{
    NodeGraphic* currentStep = oldSteps[currentRow];
    NodeAssociation *association = findAssociation(currentStep);
    bool dontSkip = true;
    if(association)
    {
        NodeGraphic* associatedStep = association->getNewNode();
        ui->listWidgetNew->setCurrentRow(findRow(associatedStep));
        displayAssociation(association);
        ui->pushButtonAssociate->setText("Dissocier");
    }
    else
    {
        ui->pushButtonAssociate->setText("Associer");
        //-->Si le curseur de l'autre liste est sur une étape liée, on positionne le curseur sur pas de séléction
        NodeGraphic* newStep = newSteps[ui->listWidgetNew->currentRow()];

        // Si on était sur une association précédemment
        if(findAssociation(newStep))
        {
            ui->listWidgetNew->clearSelection();
            ui->labelComNew->setText("");
            ui->labelPrecisionNew->setText("");
            ui->labelProgNew->setText("");
            ui->labelCondNew->setText("");
            ui->labelDataNew->setText("");
            ui->labelNameNew->setText("");
            ui->listWidgetFileNew->clear();
            dontSkip=false;
        }

        noDisplay();
    }

    displayProperty(ui->labelComOld, ui->labComOld, currentStep->getComment());
    colorProperty(ui->labelComOld, ui->labelComNew, dontSkip);

    displayProperty(ui->labelCondOld, ui->labCondOld, currentStep->getConditionValue());
    colorProperty(ui->labelCondOld, ui->labelCondNew, dontSkip);

    displayProperty(ui->labelPrecisionOld, ui->labPrecisionOld, currentStep->getPrecision());
    colorProperty(ui->labelPrecisionOld, ui->labelPrecisionNew, dontSkip);

    displayProperty(ui->labelProgOld, ui->labProgOld, currentStep->getProgram());
    colorProperty(ui->labelProgOld, ui->labelProgNew, dontSkip);

    displayProperty(ui->labelNameOld, ui->labNameOld, currentStep->getName());
    colorProperty(ui->labelNameOld, ui->labelNameNew, dontSkip);

    displayProperty(ui->labelDataOld, ui->labDataOld, currentStep->getSysinValue());
    colorProperty(ui->labelDataOld, ui->labelDataNew, dontSkip);

    ui->listWidgetFileOld->clear();

    QVector<LinkGraphic*> oldLinks = oldGraphics->getLinkGraphics();

    foreach (LinkGraphic* link, oldLinks)
    {
        if(link->getInput()==currentStep && link->getOutput()->getType()=="DCLF")
        {
            QListWidgetItem* item = new QListWidgetItem(link->getDDName()+" : "+link->getOutput()->getName()+" (Sortie)");
            ui->listWidgetFileOld->addItem(item);
        }
        if(link->getOutput()==currentStep && link->getInput()->getType()=="DCLF")
        {
            QListWidgetItem* item = new QListWidgetItem(link->getDDName()+" : "+link->getInput()->getName()+" (Entrée)");
            ui->listWidgetFileOld->addItem(item);
        }
    }

    //Chercher les fichiers NEW dans la liste des fichiers OLD pour mise en valeur des fichiers modifiés/identiques
    for(int i=0; i<ui->listWidgetFileNew->count(); i++)
    {
        bool found = false;

        QListWidgetItem* newItem = ui->listWidgetFileNew->item(i);
        for(int j=0; j<ui->listWidgetFileOld->count(); j++)
        {
            QListWidgetItem* oldItem = ui->listWidgetFileOld->item(j);
            if(newItem->text() == oldItem->text()) found = true;

        }
        if(dontSkip)
        {
            if(found) newItem->setBackgroundColor(Qt::green);
            else newItem->setBackgroundColor(Qt::red);
        }
    }

    //Chercher les fichiers OLD dans la liste des fichiers NEW pour mise en valeur des fichiers supprimés
    for(int i=0; i<ui->listWidgetFileOld->count(); i++)
    {
        bool found = false;

        QListWidgetItem* oldItem = ui->listWidgetFileOld->item(i);
        for(int j=0; j<ui->listWidgetFileNew->count(); j++)
        {
            QListWidgetItem* newItem = ui->listWidgetFileNew->item(j);
            if(oldItem->text() == newItem->text()) found = true;

        }
        if(dontSkip)
        {
            if(!found) oldItem->setBackgroundColor(Qt::red);
            else oldItem->setBackgroundColor(Qt::white);
        }

    }

}

void VersionUpdateDialog::on_listWidgetNew_currentRowChanged(int currentRow)
{

    NodeGraphic* currentStep = newSteps[currentRow];

    NodeAssociation *association = findAssociation(currentStep);
    bool dontSkip = true;

    if(association)
    {
        NodeGraphic* associatedStep = association->getOldNode();
        ui->listWidgetOld->setCurrentRow(findRow(associatedStep));
        displayAssociation(association);
        ui->pushButtonAssociate->setText("Dissocier");
    }
    else
    {
        ui->pushButtonAssociate->setText("Associer");
        NodeGraphic* oldStep = oldSteps[ui->listWidgetOld->currentRow()];

        // Si on était sur une association précédemment
        if(findAssociation(oldStep))
        {
            ui->listWidgetOld->clearSelection();
            ui->labelComOld->setText("");
            ui->labelPrecisionOld->setText("");
            ui->labelProgOld->setText("");
            ui->labelCondOld->setText("");
            ui->labelDataOld->setText("");
            ui->labelNameOld->setText("");
            ui->listWidgetFileOld->clear();
            dontSkip=false;
        }

        noDisplay();
    }

    displayProperty(ui->labelComNew, ui->labComNew, currentStep->getComment());
    colorProperty(ui->labelComOld, ui->labelComNew, dontSkip);

    displayProperty(ui->labelCondNew, ui->labCondNew, currentStep->getConditionValue());
    colorProperty(ui->labelCondOld, ui->labelCondNew, dontSkip);

    displayProperty(ui->labelPrecisionNew, ui->labPrecisionNew, currentStep->getPrecision());
    colorProperty(ui->labelPrecisionOld, ui->labelPrecisionNew, dontSkip);

    displayProperty(ui->labelProgNew, ui->labProgNew, currentStep->getProgram());
    colorProperty(ui->labelProgOld, ui->labelProgNew, dontSkip);

    displayProperty(ui->labelNameNew, ui->labNameNew, currentStep->getName());
    colorProperty(ui->labelNameOld, ui->labelNameNew, dontSkip);

    displayProperty(ui->labelDataNew, ui->labDataNew, currentStep->getSysinValue());
    colorProperty(ui->labelDataOld, ui->labelDataNew, dontSkip);

    ui->listWidgetFileNew->clear();
    QVector<LinkGraphic*> newLinks = newGraphics->getLinkGraphics();
    foreach (LinkGraphic* link, newLinks)
    {
        if(link->getInput()==currentStep && link->getOutput()->getType()=="DCLF")
        {
            QListWidgetItem* item = new QListWidgetItem(link->getDDName()+" : "+link->getOutput()->getName()+" (Sortie)");
            ui->listWidgetFileNew->addItem(item);
        }
        if(link->getOutput()==currentStep && link->getInput()->getType()=="DCLF")
        {
            QListWidgetItem* item = new QListWidgetItem(link->getDDName()+" : "+link->getInput()->getName()+" (Entrée)");
            ui->listWidgetFileNew->addItem(item);
        }
    }

    //Chercher les fichiers NEW dans la liste des fichiers OLD pour mise en valeur des fichiers modifiés/identiques
    for(int i=0; i<ui->listWidgetFileNew->count(); i++)
    {
        bool found = false;

        QListWidgetItem* newItem = ui->listWidgetFileNew->item(i);
        for(int j=0; j<ui->listWidgetFileOld->count(); j++)
        {
            QListWidgetItem* oldItem = ui->listWidgetFileOld->item(j);
            if(newItem->text() == oldItem->text()) found = true;
        }
        if(dontSkip)
        {
            if(found) newItem->setBackgroundColor(Qt::green);
            else newItem->setBackgroundColor(Qt::red);
        }

    }

    //Chercher les fichiers OLD dans la liste des fichiers NEW pour mise en valeur des fichiers supprimés
    for(int i=0; i<ui->listWidgetFileOld->count(); i++)
    {
        bool found = false;

        QListWidgetItem* oldItem = ui->listWidgetFileOld->item(i);
        for(int j=0; j<ui->listWidgetFileNew->count(); j++)
        {
            QListWidgetItem* newItem = ui->listWidgetFileNew->item(j);
            if(oldItem->text() == newItem->text()) found = true;

        }
        if(dontSkip)
        {
            if(!found) oldItem->setBackgroundColor(Qt::red);
            else oldItem->setBackgroundColor(Qt::white);
        }

    }

}

NodeAssociation::NodeAssociation(NodeGraphic *_oldNode, NodeGraphic *_newNode)
{
    oldNode=_oldNode;
    newNode=_newNode;
    forced = false;

    targetComment = newNode->getComment();
    targetCondition = newNode->getConditionValue();
    targetStepName = newNode->getName();
    targetData = newNode->getSysinValue();
    targetProgram = newNode->getProgram();
    targetPrecision = oldNode->getPrecision();
}

void NodeAssociation::initFiles(CustomGraphics *_oldGraphics, CustomGraphics *_newGraphics)
{
    // Ajout des fichiers de la nouvelle version
    QVector<LinkGraphic*> newLinks = _newGraphics->getLinkGraphics();
    foreach (LinkGraphic* link, newLinks)
    {
        TargetFile *file = 0;
        if(link->getInput()==getNewNode() && link->getOutput()->getType()=="DCLF") file = new TargetFile(link->getDDName(),link->getOutput()->getName(), true, true);
        if(link->getOutput()==getNewNode() && link->getInput()->getType()=="DCLF") file = new TargetFile(link->getDDName(),link->getInput()->getName(), false, true);

        if(file) targetFiles.push_back(file);
    }

    // Ajout des fichiers de l'ancienne version si pas encore ajouté, mais non reportés par défaut
    // sauf si lien pointillé
    QVector<LinkGraphic*> oldLinks = _oldGraphics->getLinkGraphics();
    foreach (LinkGraphic* link, oldLinks)
    {
        TargetFile *file = 0;
        if(link->getInput()==getOldNode() && link->getOutput()->getType()=="DCLF"
          && !getTargetFile(link->getDDName(),link->getOutput()->getName(), true))       
        {
            file = new TargetFile(link->getDDName(),link->getOutput()->getName(), true, link->getPenStyle()!=Qt::SolidLine);
            file->setWarning(link->getPenStyle()!=Qt::SolidLine);
        }

        if(link->getOutput()==getOldNode() && link->getInput()->getType()=="DCLF"
          && !getTargetFile(link->getDDName(),link->getInput()->getName(), false))
        {
            file = new TargetFile(link->getDDName(),link->getInput()->getName(), false, link->getPenStyle()!=Qt::SolidLine);
            file->setWarning(link->getPenStyle()!=Qt::SolidLine);
        }

        if(file) targetFiles.push_back(file);
    }

}

TargetFile* NodeAssociation::getTargetFile(const QString &_ddname, const QString &_dclfName, bool _isOutput)
{
    foreach (TargetFile* file, targetFiles)
    {
        if(file->getDdName()==_ddname && _dclfName == file->getDclfName() && _isOutput==file->getIsOutput()) return file;
    }

    return 0;
}

TargetFile *NodeAssociation::findLink(const QString &_inputName, const QString &_outputName)
{
    foreach (TargetFile* file, targetFiles)
    {
        if(file->getIsOutput() && file->getDclfName()==_outputName) return file;
        if(!file->getIsOutput() && file->getDclfName()==_inputName) return file;
    }
    return 0;
}

void VersionUpdateDialog::on_switchCondition_clicked()
{
    NodeAssociation *association = currentAssociation();
    if(association)
    {
        QString condition = (ui->labelCondOld->text() == association->getTargetCondition()) ? ui->labelCondNew->text() : ui->labelCondOld->text();
        association->setTargetCondition(condition);
        displayAssociation(association);
    }
}
void VersionUpdateDialog::on_switchData_clicked()
{
    NodeAssociation *association = currentAssociation();
    if(association)
    {
        QString data = (ui->labelDataOld->text() == association->getTargetData()) ? ui->labelDataNew->text() : ui->labelDataOld->text();
        association->setTargetData(data);
        displayAssociation(association);
    }
}
void VersionUpdateDialog::on_switchCom_clicked()
{
    NodeAssociation *association = currentAssociation();
    if(association)
    {
        QString comment = (ui->labelComOld->text() == association->getTargetComment()) ? ui->labelComNew->text() : ui->labelComOld->text();
        association->setTargetComment(comment);
        displayAssociation(association);
    }
}
void VersionUpdateDialog::on_switchPrecision_clicked()
{
    NodeAssociation *association = currentAssociation();
    if(association)
    {
        QString precision = (ui->labelPrecisionOld->text() == association->getTargetPrecision()) ? ui->labelPrecisionNew->text() : ui->labelPrecisionOld->text();
        association->setTargetPrecision(precision);
        displayAssociation(association);
    }
}
void VersionUpdateDialog::on_switchProgram_clicked()
{
    NodeAssociation *association = currentAssociation();
    if(association)
    {
        QString program = (ui->labelProgOld->text() == association->getTargetProgram()) ? ui->labelProgNew->text() : ui->labelProgOld->text();
        association->setTargetProgram(program);
        displayAssociation(association);
    }
}

void VersionUpdateDialog::addAssociation(NodeAssociation* _association)
{
    if(newNodesSkipped.contains(_association->getNewNode())) newNodesSkipped.erase(newNodesSkipped.begin() + newNodesSkipped.indexOf(_association->getNewNode()), newNodesSkipped.begin() + newNodesSkipped.indexOf(_association->getNewNode())+1);
    if(oldNodesKept.contains(_association->getOldNode())) oldNodesKept.erase(oldNodesKept.begin() + oldNodesKept.indexOf(_association->getOldNode()), oldNodesKept.begin() + oldNodesKept.indexOf(_association->getOldNode())+1);
    associations.push_back(_association);
}

void VersionUpdateDialog::on_listWidgetFileTarget_doubleClicked(const QModelIndex &index)
{
    if(!currentAssociation()) return;

    TargetFile *file = currentAssociation()->getTargetFiles()[index.row()];
    file->setWarning(false);
    file->setReport(!file->getReport());
    if(file->getReport()) ui->listWidgetFileTarget->item(index.row())->setBackgroundColor(Qt::green);
    else ui->listWidgetFileTarget->item(index.row())->setBackgroundColor(Qt::red);
}

void VersionUpdateDialog::on_buttonBox_accepted()
{
    qDebug()<<"Début de montée de version";
    oldGraphics->saveTemporaryFile("Montée de version");

    oldGraphics->getMain()->getActionSelectMode()->setChecked(true);
    oldGraphics->getMain()->setSelectMode(true);

    foreach (NodeGraphic* node, newSteps)
    {
        // Noeud associé : on modifie le noeud d'origine
        if(findAssociation(node))
        {
            // Modification de tous les champs
            NodeAssociation* currentAssociation = findAssociation(node);
            StepGraphic* modifiedNode = (StepGraphic*)currentAssociation->getOldNode();

            modifiedNode->setComment(currentAssociation->getTargetComment());
            modifiedNode->setProgram(currentAssociation->getTargetProgram());
            modifiedNode->setPrecision(currentAssociation->getTargetPrecision());
            modifiedNode->setName(currentAssociation->getTargetStepName());
            modifiedNode->setSysin(currentAssociation->getTargetData());
            modifiedNode->setCondition(currentAssociation->getTargetCondition());

            // Suppression des liens non reportés
            foreach(LinkGraphic* link, oldGraphics->getLinks(modifiedNode))
            {
                // On ne supprime que les liens automatiques non reportés
                if(!((link->getInput()->getType()=="DCLF" && link->getOutput()->getType()=="STEP")
                   || link->getOutput()->getType()=="DCLF" && link->getInput()->getType()=="STEP")) continue;
                TargetFile* targetFile = currentAssociation->findLink(link->getInput()->getName(),link->getOutput()->getName());
                if(!targetFile || !targetFile->getReport()) oldGraphics->deleteLink(link->getInput(), link->getOutput());
            }
            // Ajout des liens si inexistants, modification si ddName modifié;
            // DCLF rapportée si inexistante dans l'ancien dessin de chaine
            foreach(TargetFile* file, currentAssociation->getTargetFiles())
            {
                if(file->getReport())
                {
                    LinkGraphic* link = link = oldGraphics->linkExists(file->getDclfName(), modifiedNode, file->getIsOutput());
                    //Modif si lien existant
                    if(link) link->setDDName(file->getDdName());

                    //Création sinon
                    else
                    {
                        //Ajout de la DCLF si nécessaire
                        NodeGraphic* dclf = oldGraphics->getDclfGraphic(file->getDclfName());
                        if(!dclf)
                        {
                            dclf = newGraphics->getDclfGraphic(file->getDclfName());
                            oldGraphics->addNode(dclf);
                            dclf->setGraphics(oldGraphics);
                            dclf->setId(oldGraphics->getMaxId()+1);
                            dclf->setSelected(true);
                        }

                        NodeGraphic* input = file->getIsOutput() ? modifiedNode : dclf;
                        NodeGraphic* output = file->getIsOutput() ? dclf : modifiedNode;

                        LinkGraphic* newLink = new LinkGraphic(input, output, file->getDdName(), oldGraphics, false);
                        oldGraphics->addLink(newLink);
                    }
                }
            }
        }
        // Nouveau noeud non conservé
        else if (newNodesSkipped.contains(node)) continue;
        //Nouveau noeud conservé : on le passe dans l'ancienne scène
        else
        {
            oldGraphics->addNode(node);
            node->setGraphics(oldGraphics);
            node->setId(oldGraphics->getMaxId()+1);
            node->setSelected(true);

            //On recréé les liens
            foreach(LinkGraphic* link, newGraphics->getLinks(node))
            {
                NodeGraphic* dclf=0;
                bool isOutput = false;
                if(link->getInput()->getType()=="DCLF") dclf = link->getInput();
                else if(link->getOutput()->getType()=="DCLF")
                {
                    dclf = link->getOutput();
                    isOutput = true;
                }
                else continue;

                //On ramène aussi les DCLF liées si inexistantes dans l'ancienne

                if(! oldGraphics->getDclfGraphic(dclf->getName()))
                {
                    oldGraphics->addNode(dclf);
                    dclf->setGraphics(oldGraphics);

                    dclf->setId(oldGraphics->getMaxId()+1);
                    dclf->setSelected(true);
                }

                NodeGraphic* input = isOutput ? node : oldGraphics->getDclfGraphic(dclf->getName());
                NodeGraphic* output = isOutput ? oldGraphics->getDclfGraphic(dclf->getName()) : node;

                LinkGraphic* newLink = new LinkGraphic(input, output, link->getDDName(), oldGraphics, false);
                oldGraphics->addLink(newLink);
            }
        }
    }
    // Suppression des anciens noeuds en trop
    foreach (NodeGraphic* node, oldSteps)
    {
        if(findAssociation(node)) continue; // Déjà traitée plus haut
        else if(oldNodesKept.contains(node)) continue; // Noeud déjà présent conservé
        else
        {
            //Suppression du noeud
            oldGraphics->deleteNode(node);
        }
    }

    //Mise à jour des DCLF
    if(ui->checkBoxUpdateDCLF)
    {
        foreach(DCLFGraphic* dclf, oldGraphics->getDclfGraphics())
        {
            //on filtre sur la longueur : si longueur nulle c'est que les caractéristiques ne sont pas renseignées
            if(newGraphics->getDclfGraphic(dclf->getName()) && newGraphics->getDclfGraphic(dclf->getName())->getLength())
            {
                DCLFGraphic* modelDCLF = newGraphics->getDclfGraphic(dclf->getName());
                dclf->setLength(modelDCLF->getLength());
                dclf->setClass(modelDCLF->getClass());
                dclf->setType(modelDCLF->getType());
            }
        }
    }

    //Suppression des DCLF inutilisées
    if(ui->checkBoxDeleteUnused)
    {
        foreach(DCLFGraphic* dclf, oldGraphics->getDclfGraphics())
        {
            if(!oldGraphics->getLinks((NodeGraphic*)dclf).size()) oldGraphics->deleteNode((NodeGraphic*)dclf);
        }
    }

    //Tri des noeuds selectionnés (nouveaux noeuds seulement)
    if(ui->checkBoxSort) oldGraphics->getMain()->reSortSelection();
    else oldGraphics->getMain()->reSort();
    newGraphics->getMain()->setStatus("Montée de version effectuée");
    qDebug()<<"Fin de montée de version";

}

void VersionUpdateDialog::on_listWidgetOld_doubleClicked(const QModelIndex &index)
{
    if(findAssociation(oldSteps[index.row()])) return;

    // Echange entre conservation et Suppression du noeud
    int position = oldNodesKept.indexOf(oldSteps[index.row()]);
    if(position!=-1)
    {
        oldNodesKept.erase(oldNodesKept.begin()+position, oldNodesKept.begin()+position+1);
    }
    else oldNodesKept.push_back(oldSteps[index.row()]);

    setStepsBackgroundColor();
}

void VersionUpdateDialog::on_listWidgetNew_doubleClicked(const QModelIndex &index)
{
    if(findAssociation(newSteps[index.row()])) return;

    // Echange entre conservation et Suppression du noeud
    int position = newNodesSkipped.indexOf(newSteps[index.row()]);
    if(position!=-1)
    {
        newNodesSkipped.erase(newNodesSkipped.begin()+position, newNodesSkipped.begin()+position+1);
    }
    else newNodesSkipped.push_back(newSteps[index.row()]);

    setStepsBackgroundColor();
}

void VersionUpdateDialog::on_listWidgetOld_itemSelectionChanged()
{
    // Activation / Désactivation du bouton Associer selon la sélection
    if(ui->listWidgetNew->selectedItems().size()==0 || ui->listWidgetOld->selectedItems().size()==0) ui->pushButtonAssociate->setEnabled(false);
    else ui->pushButtonAssociate->setEnabled(true);
}

void VersionUpdateDialog::on_listWidgetNew_itemSelectionChanged()
{
    // Activation / Désactivation du bouton Associer selon la sélection
    if(ui->listWidgetNew->selectedItems().size()==0 || ui->listWidgetOld->selectedItems().size()==0) ui->pushButtonAssociate->setEnabled(false);
    else ui->pushButtonAssociate->setEnabled(true);
}

bool TargetFile::getWarning() const
{
    return warning;
}

void TargetFile::setWarning(bool value)
{
    warning = value;
}
