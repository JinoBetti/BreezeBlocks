#include "jcltranslator.h"
#include<QRegExp>
#include<QFile>
#include<QDebug>

Jobset JCLTranslator::translate(const QString &_path)
{
    qDebug()<<"JCLTranslator::translate("<<_path<<")";
    Jobset jobset;
    jobset.setSource("JCL");

    QFile inputJCLFile(_path);
    if(!inputJCLFile.open(QIODevice::ReadOnly|QIODevice::Text)) { qDebug()<<"ERROR when opening the input PCL File !"; exit(1); }
    else
    {
        QString comment;
        QString DDName;
        QString name = "";

        Job* currentJob = new Job;
        Step* currentStep = new Step;
        QVector<Condition> conditions;
        bool doNotRead = false;
        QString line;
        QTextStream stream(&inputJCLFile);
        while(!stream.atEnd())
        {
            if(!doNotRead) line = stream.readLine();
            doNotRead = false;
            /// On enlève les commentaires présents après le caractère 72
            line = line.left(72);
            if(line.startsWith("/*")) continue;
            bool beginBySpace = false;
            if(line.startsWith("// ")) beginBySpace = true;
            line = line.simplified();


            // Extract commented block
            QRegExp commentedLine("^//\\* ?(.*)[\\- \\!=]*"); /// line beginning with //*, keeping all the characters but the final " -!=" and the " " in front
            if(commentedLine.indexIn(line)!=-1)
            {
                QStringList smatch = commentedLine.capturedTexts();
                QRegExp uselessComment("^\\*[\\-\\*_=\\! ]*$");
                if(line.contains(uselessComment)) continue;
                if(comment != "") comment+="\n";
                comment+=smatch[1];
                continue;
            }

            // Divide lines into three blocks : DDNAME, TYPE OF DDCARD, and PARAMETERS
            QRegExp ddnameLine("^//([\\w|\\.]*) +(\\w+)( +(.*)| *)$");

            if(line.contains(ddnameLine))
            {
                QStringList substrings = ddnameLine.capturedTexts();
                if(substrings[1]!="") DDName = substrings[1];

                QString type = substrings[2];
                QString paramLine = substrings[4];

                // Process continued lines, ending with an hyphen or a coma
                QRegExp continuedLine("^//(\\w*) +(\\w+)(.*)( *-|,)$");
                if(line.contains(continuedLine))
                {
                    bool coma = line.contains(QRegExp(".*,$"));
                    QStringList substrings = continuedLine.capturedTexts();
                    paramLine = substrings[3];
                    line = stream.readLine();
                    line = line.left(72).remove(0,2).simplified();

                    QRegExp continuedAgain("^ *(.*)( -|,)$");
                    while(line.contains(continuedAgain))
                    {
                        if(coma) paramLine+=",";
                        coma = line.contains(QRegExp(".*,"));
                        QStringList extraParams = continuedAgain.capturedTexts();
                        paramLine += extraParams[1];
                        line = stream.readLine().left(72).remove(0,2).simplified();
                    }

                    if(coma) paramLine+=",";
                    paramLine+= line;
                }

                if(type=="JOB")
                {
                    if(name=="") name = DDName;
                    jobset.setAttributes(name, type, paramLine, comment);

                    //for second job if any
                    if(currentJob->getDDName()!="")
                    {
                        // considering a job contains at least one step...
                        currentJob->addStep(*currentStep);
                        jobset.addJob(*currentJob);
                        currentJob = new Job;
                    }
                    currentJob->setAttributes(DDName, type, paramLine, comment);
                }

                else if(type=="EXEC")
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
                    QString condParam = currentStep->getParameter("COND").getValue();
                    if(!condParam.isEmpty()) finalCondition += "If not " + condParam;
                    for(int i=1; i<9; i++)
                    {
                        QString condParamI = currentStep->getParameter("COND" + QString::number(i)).getValue();
                        // si il y a déja une condition sur le bloc et qu'on  va en rajouter => saut de ligne
                        if(!finalCondition.isEmpty() && !condParamI.isEmpty()) finalCondition += "\n";
                        // sinon si la condition est vide et qu'on va la remplir => ajout If
                        else if(!condParamI.isEmpty() && finalCondition.isEmpty()) finalCondition += "If ";
                        if(!condParamI.isEmpty()) finalCondition += "Not " + condParamI;
                        if(!finalCondition.isEmpty() && currentStep->getParameter("COND"+QString::number(i+1)).getValue()!="") finalCondition += "\nAnd ";
                    }

                    /// >> Création de la condition
                    if(finalCondition!="")
                    {
                        Condition condition;
                        condition.setCondition(finalCondition);
                        currentStep->setCondition(condition);
                    }
                }

                else if(type=="DD" && paramLine.contains("DSN="))
                {
                    Data file;
                    file.setAttributes(DDName, "FILEJCL", paramLine, comment);
                    currentStep->addData(file);
                }

                else if(type=="DD" && paramLine=="*") /// JCL
                {
                    Data data;
                    data.setAttributes(DDName, "DATAJCL", "", comment);

                    QString dataContent = "";
                    line = stream.readLine();
                    line = line.left(72);

                    while(!line.startsWith("//") && !stream.atEnd())
                    {
                        if(line.startsWith("/*")) break;
                        if(dataContent!="") dataContent += "\n";
                        dataContent += line;
                        line = stream.readLine();
                        line = line.left(72);

                        bool beginBySpace = false;
                        if(line.startsWith(" ")) beginBySpace = true;
                        line = line.simplified();
                        if(beginBySpace) line = " " + line;
                    }
                    ///ne pas relire à la prochaine itération : on vient de lire une ligne à traiter
                    doNotRead = true;

                    data.setDataContent(dataContent);
                    currentStep->addData(data);
                }

                else if (type=="DD") continue; ///JCL

                else
                {
                    qDebug()<<"Unknown type : =>"<<type<<"<="<<paramLine;
                }

                comment="";
            }

        }

        if(stream.atEnd())
        {
            // considering a job contains at least one step...
            currentJob->addStep(*currentStep);
            jobset.addJob(*currentJob);
        }
    }

    inputJCLFile.close();

    jobset.deduceDCLF();

    return jobset;
}
