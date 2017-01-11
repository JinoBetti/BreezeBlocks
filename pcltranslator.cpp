#include "pcltranslator.h"
#include<QRegExp>
#include<QFile>
#include<QDebug>
#include "customgraphics.h"

PCLTranslator::PCLTranslator(QVector<VariableValue *> _variables) :variables(_variables){}
PCLTranslator::~PCLTranslator(){foreach(VariableValue* variable, variables) delete variable;}

Jobset PCLTranslator::translate(const QString &_path)
{
    //qDebug()<<"PCLTranslator::translate("<<_path<<")";
    Jobset jobset;
    jobset.setSource("PCL");
    QFile inputPCLFile(_path);
    if(!inputPCLFile.open(QIODevice::ReadOnly|QIODevice::Text)) { qDebug()<<"ERROR when opening the input PCL File !"; exit(1); }
    else
    {
        QString comment;
        QString DDName;
        QString name = "";

        Job* currentJob = new Job;
        Step* currentStep = new Step;
        QVector<Condition> conditions;
        bool start = false;
        bool ended = false;

        QTextStream stream(&inputPCLFile);
        while(!stream.atEnd())
        {
            QString line = stream.readLine().left(72);
            // Changer les variables par les valeurs données à l'ouverture
            line = changeVariables(line);

            bool beginBySpace = false;
            if(line.startsWith(" ")) beginBySpace = true;
            line = line.simplified();
            if(beginBySpace) line = " " + line;

            // Extract commented block
            QRegExp commentedLine("^\\* ?(.*)[\\- \\!=]*"); /// line beginning with *, keeping all the characters but the final " -!=" and the " " in front
            if(commentedLine.indexIn(line)!=-1)
            {
                QStringList smatch = commentedLine.capturedTexts();
                QRegExp uselessComment("^\\*[\\-\\*_=\\! ]*$");
                if(line.contains(uselessComment)) continue;
                if(comment != "") comment+="\n";
                comment+=smatch[1];
                continue;
            }

            // for PCL extracted from GCL : do not process lines beginning by :/
            if(line.contains(QRegExp("^:/.*")))
            {
                if(start)
                {
                    ended = true;
                    start = false;
                }
                QRegExp nameLine("^:/ *ADD NAME=(\\w+).*");
                if(line.contains(nameLine))
                {
                    QStringList substrings = nameLine.capturedTexts();
                    name = substrings[1];
                }
                continue;
            }

            // Divide lines into three blocks : DDNAME, TYPE OF DDCARD, and PARAMETERS
            QRegExp ddnameLine("^(\\w*) +(\\w+)( +(.*)| *)$");

            if(line.contains(ddnameLine))
            {
                QStringList substrings = ddnameLine.capturedTexts();
                if(substrings[1]!="") DDName = substrings[1];

                QString type = substrings[2];
                QString paramLine = substrings[4];

                // Traiter les lignes qui se continuent sur la suivante, et qui finissent donc par une virgule ou un tiret
                QRegExp continuedLine("^(\\w*) *(\\w+) *([^ ]*)( *-|,)$");
                if(line.contains(continuedLine))
                {
                    bool coma = line.contains(QRegExp(".*,$"));
                    QStringList substrings = continuedLine.capturedTexts();
                    paramLine = substrings[3];
                    QString newLine = stream.readLine().left(72).simplified();
                    // Changer les variables par les valeurs données à l'ouverture
                    newLine = changeVariables(newLine);
                    line+=newLine;

                    QRegExp continuedAgain("^ *(.*)( -|,)$");
                    while(newLine.contains(continuedAgain))
                    {
                        if(coma) paramLine+=",";
                        coma = newLine.contains(QRegExp(".*,"));
                        QStringList extraParams = continuedAgain.capturedTexts();
                        paramLine += extraParams[1];
                        newLine = stream.readLine().left(72).simplified();
                        // Changer les variables par les valeurs données à l'ouverture
                        newLine = changeVariables(newLine);
                        line+=newLine;
                    }

                    if(coma) paramLine+=",";
                    paramLine+= newLine;

                }



                if (type=="JOBSET")
                {
                    if(name=="") name = DDName;
                    jobset.setAttributes(name, type, paramLine, comment);
                    start = true;
                }

                else if(type=="PATTERN" && !start)
                {
                    if(name=="") name = "Pattern";
                    jobset.setAttributes(name, "JOBSET", paramLine, comment);
                    start = true;

                    currentJob->setAttributes("Job", "JOB", "", "");

                }

                else if(type=="JOB" && start)
                {
                    currentJob->setAttributes(DDName, type, paramLine, comment);
                }

                else if((type=="STEP"||type=="SORT"||type=="MERGE"||type=="PATTERN") && start)
                {
                    if(currentStep->getDDName()!="")
                    {
                        currentJob->addStep(*currentStep);
                        currentStep = new Step();
                    }

                    currentStep->setAttributes(DDName, type, paramLine, comment);

                    QString finalCondition;

                    /// >> Conditions par bloc
                    if(conditions.size())
                    {
                        finalCondition = "If ";
                        for(int i=0; i<conditions.size(); i++)
                        {
                           finalCondition += conditions[i].getCondition();
                           if(i+1<conditions.size()) finalCondition += "\nAnd ";
                        }

                    }

                    /// >> Conditions sur le step
                    for(int i=1; i<9; i++)
                    {
                        QString condParam = currentStep->getParameter("COND"+QString::number(i)).getValue();

                        // si il y a déja une condition sur le bloc et qu'on  va en rajouter => saut de ligne
                        if(!finalCondition.isEmpty() && !condParam.isEmpty()) finalCondition += "\n";
                        // sinon si la condition est vide et qu'on va la remplir => ajout If
                        else if(!condParam.isEmpty() && finalCondition.isEmpty()) finalCondition += "If ";
                        if(!condParam.isEmpty()) finalCondition += "Not " + condParam;
                        if(!finalCondition.isEmpty() && currentStep->getParameter("COND"+QString::number(i+1)).getValue()!="") finalCondition += "\nAnd ";

                    }

                    /// >> Création de la condition
                    if(finalCondition!="")
                    {
                        Condition condition;
                        condition.setCondition(finalCondition);
                        currentStep->setCondition(condition);
                    }

                    if(!currentStep->getParameter("PATKW").getValue().isEmpty())
                    {
                        Data data;

                        data.setAttributes("PATKW", "DATA", "", "");
                        data.setDataContent(currentStep->getParameter("PATKW").getValue());
                        currentStep->addData(data);
                    }

                }

                else if((type=="FILE"||type=="REPORT")  && start)
                {
                    Data file;
                    file.setAttributes(DDName, type, paramLine, comment);
                    currentStep->addData(file);
                }

                else if(type=="DATA" && start)
                {
                    Data data;
                    data.setAttributes(DDName, type, "", comment);

                    if(paramLine.contains(QRegExp("^ *\\*.*")))
                    {
                        QString sysin = "";
                        bool first=true;
                        while(!line.contains(QRegExp("DATAEND")) && !stream.atEnd())
                        {
                            if(sysin!="") sysin += "\n";
                            if(!first) sysin += line;
                            line = stream.readLine().left(72);
                            // Changer les variables par les valeurs données à l'ouverture
                            line = changeVariables(line);
                            bool beginBySpace = false;
                            if(line.startsWith(" ")) beginBySpace = true;
                            line = line.simplified();
                            if(beginBySpace) line = " " + line;
                            first = false;
                        }

                        data.setDataContent(sysin);
                    }
                    else
                    {
                        data.splitParams(paramLine);
                        data.setDataContent(paramLine);
                    }

                    currentStep->addData(data);

                }

                else if(type=="IF" && start)
                {
                    Condition newCondition;
                    if(paramLine.endsWith("THEN")) paramLine.chop(5);

                    newCondition.setCondition(paramLine);

                    newCondition.setType(type);
                    newCondition.setName("");
                    conditions.push_back(newCondition);
                }

                else if(type=="ELSE" && start)
                {
                    conditions[conditions.size()-1].setCondition("NOT " + conditions[conditions.size()-1].getCondition());
                }

                else if(type=="ENDIF" && start)
                {
                    conditions.pop_back();
                }

                // DCLF are not in the jobset part so we dont test the start boolean
                else if(type=="DCLF")
                {
                    DCLF file;
                    file.setDefinition(line.simplified());
                    qDebug()<<file.getDefinition();
                    file.setAttributes(DDName, type, paramLine, comment);
                    jobset.addDCLF(file);
                }

                else if(type=="END" && start)
                {
                    // Un job a au moins un step, ou fait appel à un pattern
                    if(currentStep->getDDName().isEmpty() && !jobset.getParameter("PATTERN").getValue().isEmpty()) currentStep->setAttributes("PATTERN","PATTERN","NAME="+jobset.getParameter("PATTERN").getValue(),"");
                    currentJob->addStep(*currentStep);
                    jobset.addJob(*currentJob);
                    start = false;
                }

                else if(type=="PATEND" && start)
                {
                    // Un job a au moins un step, ou fait appel à un pattern
                    if(currentStep->getDDName().isEmpty() && !jobset.getParameter("PATTERN").getValue().isEmpty()) currentStep->setAttributes("PATTERN","PATTERN","NAME="+jobset.getParameter("PATTERN").getValue(),"");
                    currentJob->addStep(*currentStep);
                    jobset.addJob(*currentJob);
                    start = false;
                }

                else if(start)
                {
                    qDebug()<<"Unknown type : =>"<<type<<"<=";
                }

                comment="";
            }

        }
        if(stream.atEnd() && (start||ended))
        {
            // Un job a au moins un step, ou fait appel à un pattern
            if(currentStep->getDDName().isEmpty() && !jobset.getParameter("PATTERN").getValue().isEmpty()) currentStep->setAttributes("PATTERN","PATTERN","NAME="+jobset.getParameter("PATTERN").getValue(),"");
            currentJob->addStep(*currentStep);
            jobset.addJob(*currentJob);
            start = false;
        }
    }

    inputPCLFile.close();

    jobset.deduceDCLF();
    return jobset;
}

VariableValue *PCLTranslator::getVariableValue(const QString &_name)
{
    foreach(VariableValue* variable, variables)
    {
        if(variable->getName()==_name) return variable;
    }
    return 0;
}

QString PCLTranslator::changeVariables(QString _line)
{
    QRegExp variablePresent("^(.*)&(\\w+)(\\.?)(.*)");
    if(_line.contains(variablePresent))
    {
        foreach(VariableValue* variable, variables)
        {
            _line = _line.replace("&"+variable->getName()+".", variable->getValue());
            _line = _line.replace("&"+variable->getName(), variable->getValue());
        }
    }

    return _line;

}
