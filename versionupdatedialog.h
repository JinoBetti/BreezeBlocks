#ifndef VERSIONUPDATEDIALOG_H
#define VERSIONUPDATEDIALOG_H

#include <QDialog>
#include <QLabel>
#include "nodegraphic.h"

namespace Ui {
class VersionUpdateDialog;
}
class TargetFile
{
public:
    TargetFile(const QString &_ddname, const QString &_dclfName, bool _isOutput, bool _report):
        ddName(_ddname),dclfName(_dclfName),isOutput(_isOutput),report(_report),warning(false){}

    bool getReport() const { return report; }
    void setReport(bool value) { report = value; }

    bool getIsOutput() const { return isOutput; }
    void setIsOutput(bool value) { isOutput = value; }

    QString getDclfName() const { return dclfName; }
    void setDclfName(const QString &value) { dclfName = value; }

    QString getDdName() const { return ddName; }
    void setDdName(const QString &value) { ddName = value; }

    bool getWarning() const;
    void setWarning(bool value);

private:
    QString ddName;
    QString dclfName;

    bool isOutput;
    bool report;
    bool warning;
};

class NodeAssociation
{
public:
    NodeAssociation(NodeGraphic *_oldNode, NodeGraphic *_newNode);
    void initFiles(CustomGraphics* _oldGraphics, CustomGraphics* _newGraphics);

    NodeGraphic *getOldNode() const { return oldNode; }
    void setOldNode(NodeGraphic *value){oldNode = value;}

    NodeGraphic *getNewNode() const {return newNode;}
    void setNewNode(NodeGraphic *value){ newNode = value; }

    QString getTargetComment() const { return targetComment; }
    void setTargetComment(const QString &value) { targetComment = value; }

    QString getTargetStepName() const { return targetStepName; }
    void setTargetStepName(const QString &value) { targetStepName = value; }

    QString getTargetProgram() const { return targetProgram; }
    void setTargetProgram(const QString &value) { targetProgram = value; }

    QString getTargetPrecision() const { return targetPrecision; }
    void setTargetPrecision(const QString &value) { targetPrecision = value; }

    QString getTargetData() const { return targetData; }
    void setTargetData(const QString &value) { targetData = value; }

    QString getTargetCondition() const { return targetCondition; }
    void setTargetCondition(const QString &value) { targetCondition = value; }

    QVector<TargetFile *> getTargetFiles() const { return targetFiles; }
    void setTargetFiles(const QVector<TargetFile*> &value) { targetFiles = value; }

    TargetFile *getTargetFile(const QString &_ddname, const QString &_dclfName, bool _isOutput);

    TargetFile* findLink(const QString &_inputName, const QString &_outputName);

    bool getForced() const { return forced; }
    void setForced(bool value) { forced = value; }

private:
    NodeGraphic *oldNode;
    NodeGraphic *newNode;
    QString targetComment;
    QString targetStepName;
    QString targetProgram;
    QString targetPrecision;
    QString targetData;
    QString targetCondition;
    QVector<TargetFile*> targetFiles;
    bool forced;
};

class VersionUpdateDialog : public QDialog
{
    Q_OBJECT

public:
    explicit VersionUpdateDialog(QVector<NodeGraphic*> oldNodes, QVector<NodeGraphic*> newNodes, CustomGraphics *oldGraphics, CustomGraphics *newGraphics, QWidget *parent = 0);
    ~VersionUpdateDialog();

    void displayProperty(QLabel *_value, QLabel *_label, const QString &_stringValue);
    int findRow(NodeGraphic *_step)
    {
        for(int i=0; i<newSteps.size(); i++) { if(_step==newSteps[i]) return i; }
        for(int i=0; i<oldSteps.size(); i++) { if(_step==oldSteps[i]) return i; }
        return -1;
    }

    void detectAssociations();
    int compareNodes(NodeGraphic *_old, NodeGraphic *_new);
    void colorProperty(QLabel *_old, QLabel *_new, bool _color = true);

    void displayAssociation(NodeAssociation* _association);
    void noDisplay();

    NodeAssociation* findAssociation(NodeGraphic *_step)
    { foreach(NodeAssociation* association, associations) { if(association->getOldNode()==_step || association->getNewNode()==_step) return association; } return 0;}

    NodeAssociation* currentAssociation();

    void setStepsBackgroundColor();

    void addAssociation(NodeAssociation *_association);

    static bool ebcdicLessThan(NodeGraphic *&n1, NodeGraphic *&n2);
private slots:
    void on_pushButtonAssociate_clicked();
    void on_listWidgetOld_currentRowChanged(int currentRow);
    void on_listWidgetNew_currentRowChanged(int currentRow);
    void on_switchCondition_clicked();
    void on_switchData_clicked();
    void on_switchCom_clicked();
    void on_switchPrecision_clicked();
    void on_switchProgram_clicked();
    void on_listWidgetFileTarget_doubleClicked(const QModelIndex &index);
    void on_buttonBox_accepted();
    void on_listWidgetOld_doubleClicked(const QModelIndex &index);
    void on_listWidgetNew_doubleClicked(const QModelIndex &index);
    void on_listWidgetOld_itemSelectionChanged();
    void on_listWidgetNew_itemSelectionChanged();

private:
    Ui::VersionUpdateDialog *ui;
    CustomGraphics* oldGraphics;
    CustomGraphics* newGraphics;
    QVector<NodeGraphic*> oldSteps;
    QVector<NodeGraphic*> newSteps;
    QVector<NodeAssociation*> associations;
    QVector<NodeGraphic*> newNodesSkipped;
    QVector<NodeGraphic*> oldNodesKept;

};

#endif // VERSIONUPDATEDIALOG_H
