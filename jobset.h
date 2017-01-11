#ifndef JOBSET_H
#define JOBSET_H
#include <QVector>
#include <QString>

class Parameter
{
private:
    QString name;
    QString value;
    QVector<Parameter> subParameters;
public:
    Parameter(){}
    Parameter(QString, QString);
    QString getValue() const {return value;}
    QString getName() const {return name;}
    void splitSubparams();
    Parameter getSubparam(QString _name) const;
    QVector<Parameter> getSubparams() const {return subParameters;}
    void display() const;

};

class DDCard
{
private:
    QString DDName;
    QString type;
    QString comment;
    QVector<Parameter> parameters;

public:
    DDCard(){}
    DDCard(QString _ddname, QString _type){DDName = _ddname; type = _type;}
    QString getDDName() const {return DDName;}
    QString getType() const {return type;}
    QString getComment() const {return comment;}

    Parameter getParameter(QString _name) const;

    QVector<Parameter> getParameters() {return parameters;}
    void setAttributes(QString _DDName, QString _type, QString _paramLine, QString _comment);

    void setName(QString _name){DDName = _name;}
    void setType(QString _type){type = _type;}
    void setComment(QString _comment){comment = _comment;}
    void addParameter(Parameter _p){parameters.push_back(_p);}

    void display() const;

    void splitParams(QString);
};

class Condition : public DDCard
{
private:
    QString condition;
public:
    Condition(){}
    QString getCondition() const {return condition;}
    void setCondition(const QString _condition){condition = _condition;}
    void display() const;
};


class DCLF : public DDCard
{
    QString definition;
public:
    DCLF(){}
    DCLF(QString _ddname):DDCard(_ddname, "DCLF"){}
    QString getClass() const {return getParameter("CLASS").getValue();}
    QString getFormat() const {return getParameter("RECF").getValue();}
    QString getLength() const {return getParameter("RECL").getValue();}
    QString getDefinition() const;
    void setDefinition(const QString &value);
};

class Data : public DDCard
{
private:
    QString dataContent;
public:
    Data(){}
    void setDataContent(const QString &_dataContent){dataContent = _dataContent;}
    void display() const;
    QString getDataContent() const {return dataContent;}
};

class Step : public DDCard
{
private:
    QVector<Data> data;
    Condition condition;
public:
    void addData(const Data &_data){data.push_back(_data);}
    QVector<Data> getData() const {return data;}

    void display() const;
    const Condition getCondition(){return condition;}
    void setCondition(const Condition _condition){condition = _condition;}
    QString getProgram() const;

};


class Job : public DDCard
{
private:
    QVector<Step> steps;
public:
    void addStep(const Step &_step){steps.push_back(_step);}
    QVector<Step> getSteps() const {return steps;}
    void display() const;

};


class Jobset : public DDCard
{
private:
    QString source; /// PCL ou JCL

    QVector<Job> jobs;
    QVector<DCLF> dclf;

public:
    void addJob(const Job &_job){jobs.push_back(_job);}
    void addDCLF(const DCLF &_dclf){dclf.push_back(_dclf);}
    void deduceDCLF();
    bool searchDCLF(const QString &_ddname);
    unsigned getDCLFNb(){return dclf.size();}
    QVector<Job> getJobs() const {return jobs;}
    QVector<DCLF> getDclf() const {return dclf;}

    void display();
    void createDCLFFromRules(const Step &_step);
    QString getSource() const { return source; }
    void setSource(const QString &value) { source = value; }
};

#endif // JOBSET_H
