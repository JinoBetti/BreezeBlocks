#include "jobset.h"
#include<QDebug>
#include<QRegExp>
#include<QStringList>
#include "utils.h"

Parameter::Parameter(QString _name, QString _value)
{
    name=_name;
    value=_value;
    splitSubparams();    
}

// attention : prévu pour un seul niveau de sous paramètres pour l'instant
void Parameter::splitSubparams()
{
    QRegExp params("\\((.*)\\)");

    if(value.contains(params))
    {
        QStringList woBrackets = params.capturedTexts();
        QString parameters = woBrackets[1];

        QRegExp firstParam("(\\w+)=(\\w+|\\(.*\\)),?(.*)");

        while(parameters.contains(firstParam))
        {
            QStringList subparams = firstParam.capturedTexts();
            Parameter p(subparams[1],subparams[2]);
            subParameters.push_back(p);
            parameters = subparams[3];
        }
    }
}

Parameter Parameter::getSubparam(QString _name) const
{
    foreach(const Parameter &subParam, subParameters){ if(subParam.getName()==_name) return subParam; }
    return Parameter("","");
}

void Parameter::display() const
{
    if (subParameters.size()==0) qDebug()<<"  "<<name<<" => "<<value;
    else
    {
        qDebug()<<"  "<<name<<" => ";
        foreach(const Parameter &subParam, subParameters) qDebug()<<"    "<<subParam.name<<" => "<<subParam.value;
    }
}


void Step::display() const
{
    condition.display();
    DDCard::display();
    foreach(const Data & dat, data) dat.display();
}

QString Step::getProgram() const
{
    if(getType()=="MERGE" || getType()=="SORT") return getType();
    //Pattern monostep
    if(getType()=="PATTERN" && getParameter("PGM").getValue()=="*") return getParameter("PATTERN").getValue();
    //Pattern multistep
    if(getType()=="PATTERN") return getParameter("NAME").getValue();

    /// Spécifique JCL
    if(getType()=="EXEC")
    {
        QString program = getParameter("PGM").getValue();
        if(program.isEmpty()) program = getParameter("PROC").getValue();
        if(program.isEmpty()) program = getParameter("PLAN").getValue();
        if(program.isEmpty()) program = getParameter("FIRSTPARAM").getValue();
        /// \todo virer cette partie spécifique volvo :
        if(program=="DB2BATCT")
        {
            for(int i=0;i<getData().size();i++)
            {
                if(getData()[i].getDDName()=="SYSTSIN")
                {
                    QRegExp programInCard("(.*)PROGRAM\\((\\w+)\\).*");
                    if(programInCard.indexIn(getData()[i].getDataContent())!=-1)
                    {
                        QStringList smatch = programInCard.capturedTexts();
                        program = smatch[2];
                        break;
                    }
                }
            }
        }
        return program;
    }

    return getParameter("PGM").getValue();
}


void DDCard::splitParams(QString _paramLine)
{
    /// On enlève les espaces inutiles
    _paramLine = _paramLine.simplified();

    /// Expression régulière pour un paramètre unique
    QRegExp onlyParam("^([\\w|\\.|\\*]+).?$");
    if(_paramLine.contains(onlyParam))
    {
        QStringList subparams = onlyParam.capturedTexts();
        addParameter(Parameter("FIRSTPARAM",subparams[1]));
        return;
    }
    /// Expression régulière pour le premier paramètre d'une liste
    QRegExp firstParam("^([\\w|\\.|\\*]+),(.*)");
    if(_paramLine.contains(firstParam))
    {
        QStringList subparams = firstParam.capturedTexts();
        addParameter(Parameter("FIRSTPARAM",subparams[1]));
        _paramLine = subparams[2];
    }

    /// Expression de la forme TRUC=ABCD.ED*F.G ou TRUC=(quelquechose sans parenthèse fermante)
    QRegExp params("(\\w+)=([\\w|\\.|\\*]+|\\([^\\)]*\\)),?(.*)");

    while(_paramLine.contains(params))
    {
        QStringList subparams = params.capturedTexts();
        addParameter(Parameter(subparams[1],subparams[2]));
        _paramLine = subparams[3];
    }
}

Parameter DDCard::getParameter(QString _name) const
{
    foreach(const Parameter &param, parameters) { if(param.getName()==_name) return param; }
    return Parameter("","");
}

void DDCard::setAttributes(QString _DDName, QString _type, QString _paramLine, QString _comment)
{
    setType(_type);
    setComment(_comment);
    setName(_DDName);
    splitParams(_paramLine);
    if(getParameter("PGM").getValue()=="*") setType("PATTERN");
}

void DDCard::display() const
{
    qDebug()<<type<<" "<<DDName<<" : ";
    if(comment!="") qDebug()<<"COMMENT : "<<endl<<comment;
    if(parameters.size())
    {
        qDebug()<<"PARAMETERS : ";
        foreach(const Parameter &param, parameters) param.display();
    }
}

void Job::display() const
{
    DDCard::display();
    foreach(const Step &step, steps) step.display();
}

void Data::display() const
{
    DDCard::display();
    if(dataContent != "") qDebug()<<dataContent;
}

/*!
 *  \fn
 *  \brief
 *
 * \param
 * \return
 *
 */
void Condition::display() const { if (!condition.isEmpty()) qDebug()<<"Condition 'si' du step : "<<condition; }


void Jobset::display()
{
    DDCard::display();

    if(!jobs.size()) qDebug()<<"WARNING : No Job found";
    foreach(const Job &job, jobs) job.display();

    if(dclf.size()) qDebug()<<"DCLF List : ";
    foreach(const DCLF &dc, dclf) dc.display();
}

void Jobset::deduceDCLF()
{
    foreach(const Job &job, jobs)
    {
        foreach(const Step &currentStep, job.getSteps())
        {
            // Création des DCLF, /!\ si des règles sont positionnées dans les paramètres
            createDCLFFromRules(currentStep);

            foreach(const Data &currentData, currentStep.getData())
            {
                if(currentData.getType()=="FILE")
                {
                    QString value = currentData.getParameter("NAME").getValue();
                    DCLF newDCLF(value);
                    if(value.length()> 6 && value.mid(3,2)=="WK") newDCLF.addParameter(Parameter("CLASS","XP"));
                    else newDCLF.addParameter(Parameter("CLASS",""));
                    if(value!="" && !searchDCLF(value)) addDCLF(newDCLF);
                }
                else if(currentData.getType()=="FILEJCL") ///JCL
                {
                    QString value = currentData.getParameter("DSN").getValue();
                    QString path = value, name = value;//.remove(0,value.lastIndexOf(".")+1);
                    path.truncate(path.lastIndexOf("."));

                    DCLF newDCLF(name);
                    newDCLF.addParameter(Parameter("CLASS",path));
                    if(value!="" && !searchDCLF(value)) addDCLF(newDCLF);
                }
                else if(currentData.getType()=="REPORT")
                {
                    QString value = currentData.getParameter("SYSOUT").getValue();
                    QRegExp reportParams("\\((.*),(.*),(.*)\\)");
                    if(value=="*") continue;
                    else if(value.contains(reportParams))
                    {
                        QStringList subparams = reportParams.capturedTexts();
                        DCLF newDCLF(subparams[2]);
                        newDCLF.addParameter(Parameter("CLASS","ETAT"));
                        addDCLF(newDCLF);
                    }
                }
            }
        }
    }
}

bool Jobset::searchDCLF(const QString &_ddname)
{
    foreach(const DCLF &dc, dclf) if(dc.getDDName() == _ddname) return true;
    return false;
}

void Jobset::createDCLFFromRules(const Step &_step)
{
    foreach(const PCLRule &rule, PCL_RULES)
    {
        if(rule.program == _step.getProgram())
        {
            QString value = _step.getParameter(rule.param).getSubparam(rule.subparam).getValue();
            if(value!="" && !searchDCLF(value))
            {
                DCLF newDCLF(value);
                newDCLF.addParameter(Parameter("CLASS",rule.classe));
                addDCLF(newDCLF);
            }
        }
    }
}

QString DCLF::getDefinition() const
{
    return definition;
}

void DCLF::setDefinition(const QString &value)
{
    definition = value;
}
