#include "ortifile.h"

QString OrtiJob::getJobname() const
{
    return jobname;
}

void OrtiJob::setJobname(const QString &value)
{
    jobname = value;
}

QString OrtiJob::getApplication() const
{
    return application;
}

void OrtiJob::setApplication(const QString &value)
{
    application = value;
}

QString OrtiJob::getPlanification() const
{
    return planification;
}

void OrtiJob::setPlanification(const QString &value)
{
    planification = value;
}

QString OrtiJob::getGroup() const
{
    return group;
}

void OrtiJob::setGroup(const QString &value)
{
    group = value;
}


int OrtiJob::getNodeGraphicId() const
{
    return nodeGraphicId;
}

void OrtiJob::setNodeGraphicId(int value)
{
    nodeGraphicId = value;
}

OrtiJob::OrtiJob(QString _jobname, QString _application, QString _planification, QString _group):
    jobname(_jobname),
    application(_application),
    planification(_planification.simplified()),
    group(_group.simplified()),
    nodeGraphicId(0)
{}

QString OrtiLink::getType() const { return type; }

void OrtiLink::setType(const QString &value) { type = value; }

OrtiJob *OrtiLink::getSuccessor() const
{
    return successor;
}

void OrtiLink::setSuccessor(OrtiJob *value)
{
    successor = value;
}

OrtiJob *OrtiLink::getPredecessor() const
{
    return predecessor;
}

void OrtiLink::setPredecessor(OrtiJob *value)
{
    predecessor = value;
}

Orti::LinkTypes OrtiLink::getLinkType() const
{
    return linkType;
}

void OrtiLink::setLinkType(const Orti::LinkTypes &value)
{
    linkType = value;
}

OrtiTopH *OrtiLink::getTopH() const
{
    return topH;
}

void OrtiLink::setTopH(OrtiTopH *value)
{
    topH = value;
}

OrtiResource *OrtiLink::getResource() const
{
    return resource;
}

void OrtiLink::setResource(OrtiResource *value)
{
    resource = value;
}

OrtiLink::OrtiLink(OrtiJob* _predecessor, OrtiJob* _successor, QString _type):
    type(_type),
    predecessor(_predecessor),
    successor(_successor),
    resource(0),
    topH(0),
    linkType(Orti::Predecessor)
{}

OrtiLink::OrtiLink(OrtiJob* _successor, OrtiResource* _resource, QString _type):
    type(_type),
    predecessor(0),
    successor(_successor),
    resource(_resource),
    topH(0),
    linkType(Orti::Resource)
{}

int OrtiLink::getPredecessorNodeGraphicId() const
{
    switch(linkType)
    {
    case Orti::Predecessor :
        return predecessor->getNodeGraphicId();
    case Orti::Resource :
        return resource->getNodeGraphicId();
    case Orti::TopH :
        return topH->getNodeGraphicId();
    default:
        return -1;
    }
}

OrtiLink::OrtiLink(OrtiJob* _successor, OrtiTopH* _topH, QString _type):
    type(_type),
    predecessor(0),
    successor(_successor),
    resource(0),
    topH(_topH),
    linkType(Orti::TopH)
{}

QString OrtiFile::getArtefact() const
{
    return artefact;
}

void OrtiFile::setArtefact(const QString &value)
{
    artefact = value;
}

QList<OrtiJob*> OrtiFile::getJobs() const
{
    return jobs;
}

void OrtiFile::setJobs(const QList<OrtiJob*> &value)
{
    jobs = value;
}

QList<OrtiLink*> OrtiFile::getLinks() const
{
    return links;
}

void OrtiFile::setLinks(const QList<OrtiLink*> &value)
{
    links = value;
}


QList<OrtiResource *> OrtiFile::getResources() const
{
    return resources;
}

void OrtiFile::setResources(const QList<OrtiResource *> &value)
{
    resources = value;
}

QList<OrtiTopH *> OrtiFile::getTopHs() const
{
    return topHs;
}

void OrtiFile::setTopHs(const QList<OrtiTopH *> &value)
{
    topHs = value;
}
OrtiJob *OrtiFile::getJob(QString _jobname)
{
    for(int i=0; i<jobs.size(); i++)
    {
        if(jobs[i]->getJobname()==_jobname ) return jobs[i];
    }

    return 0;
}

void OrtiFile::addJob(OrtiJob *_job)
{
    jobs.push_back(_job);
}

OrtiResource *OrtiFile::getResource(QString _name)
{
    foreach(OrtiResource* resource, resources)
    {
        if(resource->getName().simplified()==_name.simplified()) return resource;
    }

    return 0;
}

void OrtiFile::addResource(OrtiResource *_resource)
{
    resources.push_back(_resource);
}

OrtiTopH *OrtiFile::getTopH(QString _name)
{
    foreach(OrtiTopH* topH, topHs)
    {
        if(topH->getName()==_name) return topH;
    }

    return 0;
}

void OrtiFile::addTopH(OrtiTopH *_topH)
{
    topHs.push_back(_topH);
}

void OrtiFile::addLink(OrtiLink* _link)
{
    for(int i=0; i<links.size(); i++)
    {
        if (links[i]->getSuccessor()->getJobname()==_link->getSuccessor()->getJobname())
        {
            if(links[i]->getLinkType()==Orti::Predecessor && _link->getLinkType()==Orti::Predecessor && links[i]->getPredecessor()->getJobname()==_link->getPredecessor()->getJobname()) return;
            else if(links[i]->getLinkType()==Orti::Resource && _link->getLinkType()==Orti::Resource && links[i]->getResource()->getName()==_link->getResource()->getName()) return;
            else if(links[i]->getLinkType()==Orti::TopH && _link->getLinkType()==Orti::TopH && links[i]->getTopH()->getName()==_link->getTopH()->getName()) return;
        }
    }

    links.push_back(_link);
}


int OrtiResource::getNodeGraphicId() const
{
    return nodeGraphicId;
}

void OrtiResource::setNodeGraphicId(int value)
{
    nodeGraphicId = value;
}

bool OrtiResource::getDesactivated() const
{
    return desactivated;
}

void OrtiResource::setDesactivated(bool value)
{
    desactivated = value;
}
QString OrtiResource::getName() const
{
    return name;
}

void OrtiResource::setName(const QString &value)
{
    name = value;
}


int OrtiTopH::getNodeGraphicId() const
{
    return nodeGraphicId;
}

void OrtiTopH::setNodeGraphicId(int value)
{
    nodeGraphicId = value;
}
QString OrtiTopH::getName() const
{
    return name;
}

void OrtiTopH::setName(const QString &value)
{
    name = value;
}
