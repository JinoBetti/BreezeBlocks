#ifndef ORTIFILE_H
#define ORTIFILE_H
#include <QList>
#include <QString>
#include <QDebug>

namespace Orti
{
    typedef enum
    {
         Predecessor,
         Resource,
         TopH
    } LinkTypes;
}

class OrtiJob
{
private:
    QString jobname;
    QString application;
    QString planification;
    QString group;
    int nodeGraphicId;


public:
    OrtiJob(QString _jobname, QString _application, QString _planification, QString _group);
    QString getJobname() const;
    void setJobname(const QString &value);
    QString getApplication() const;
    void setApplication(const QString &value);
    QString getPlanification() const;
    void setPlanification(const QString &value);
    QString getGroup() const;
    void setGroup(const QString &value);
    int getNodeGraphicId() const;
    void setNodeGraphicId(int value);
};

class OrtiResource
{
    QString name;
    int nodeGraphicId;
    bool desactivated;

public:
    OrtiResource(QString _name):name(_name.simplified()){qDebug()<<"Création ressource"<<_name;}
    QString getName() const;
    void setName(const QString &value);
    int getNodeGraphicId() const;
    void setNodeGraphicId(int value);
    bool getDesactivated() const;
    void setDesactivated(bool value);
};

class OrtiTopH
{
    QString name;
    int nodeGraphicId;

public:
    OrtiTopH(QString _name):name(_name){qDebug()<<"Création topH"<<_name;}
    QString getName() const;
    void setName(const QString &value);
    int getNodeGraphicId() const;
    void setNodeGraphicId(int value);
};

class OrtiLink
{
private:
    QString type;
    OrtiJob* predecessor;
    OrtiJob* successor;
    OrtiResource* resource;
    OrtiTopH* topH;
    Orti::LinkTypes linkType;

public:
    OrtiLink(OrtiJob *_predecessor, OrtiJob *_successor, QString _type);
    OrtiLink(OrtiJob *_successor, OrtiTopH *_topH, QString _type);
    OrtiLink(OrtiJob *_successor, OrtiResource *_resource, QString _type);

    int getPredecessorNodeGraphicId() const;

    QString getType() const;
    void setType(const QString &value);

    OrtiJob *getSuccessor() const;
    void setSuccessor(OrtiJob *value);

    OrtiJob *getPredecessor() const;
    void setPredecessor(OrtiJob *value);

    Orti::LinkTypes getLinkType() const;
    void setLinkType(const Orti::LinkTypes &value);
    OrtiTopH *getTopH() const;
    void setTopH(OrtiTopH *value);
    OrtiResource *getResource() const;
    void setResource(OrtiResource *value);
};

class OrtiFile
{
private:
    QString artefact;
    QList<OrtiJob*> jobs;
    QList<OrtiLink*> links;
    QList<OrtiResource*> resources;
    QList<OrtiTopH*> topHs;

public:
    OrtiJob* getJob(QString _jobname);
    void addJob(OrtiJob* _job);

    OrtiResource* getResource(QString _name);
    void addResource(OrtiResource* _resource);

    OrtiTopH* getTopH(QString _name);
    void addTopH(OrtiTopH* _topH);

    void addLink(OrtiLink* _link);

    QString getArtefact() const;
    void setArtefact(const QString &value);

    QList<OrtiJob*> getJobs() const;
    void setJobs(const QList<OrtiJob *> &value);

    QList<OrtiLink*> getLinks() const;
    void setLinks(const QList<OrtiLink*> &value);

    QList<OrtiResource *> getResources() const;
    void setResources(const QList<OrtiResource *> &value);

    QList<OrtiTopH *> getTopHs() const;
    void setTopHs(const QList<OrtiTopH *> &value);
};

#endif // ORTIFILE_H
