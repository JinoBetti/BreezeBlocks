#include "ortitranslator.h"
#include<QRegExp>
#include<QFile>
#include<QDebug>


OrtiFile OrtiTranslator::translate(const QString &_path)
{
    QFile inputPCLFile(_path);
    if(!inputPCLFile.open(QIODevice::ReadOnly|QIODevice::Text)) { qDebug()<<"Erreur de lecture du fichier ORTI !"; exit(1); }
    else
    {
        OrtiFile orti;
        // stage 0 : liens entre jobs, 1 : top horaires, 3 ressources
        int stage = 0;
        QTextStream stream(&inputPCLFile);
        while(!stream.atEnd())
        {
            QString line = stream.readLine();


            QRegExp predecessors("( [*]{2} PREDECESSEURS DE) ([A-Za-z0-9]*) .*");
            //GJS42G01 GBG0JJ   CPTG CPU  J.O.-23 ADEP GJS42G01  GCPTJ9   CPTG J.O.-23 CPTPO5
            QRegExp linkBetweenJobs("(.{8}) (.{8}) (.{4}) (.{3})  (.{7}) (.{4}) (.{8})  (.{8}) (.{4}) (.{7}) (.{6}).*");
            if(predecessors.indexIn(line)!=-1)
            {
                QStringList smatch = predecessors.capturedTexts();
                orti.setArtefact(smatch[2].simplified());
            }
            if(stage==0 && linkBetweenJobs.indexIn(line)!=-1)
            {
                qDebug()<<"Lien: "<<line;
                QStringList smatch = linkBetweenJobs.capturedTexts();

                OrtiJob* successor = orti.getJob(smatch[2].simplified());
                if(! successor)
                {
                    successor = new OrtiJob(smatch[2].simplified(), smatch[3], smatch[5], smatch[1]);
                    orti.addJob(successor);
                }
                OrtiJob* predecessor =  orti.getJob(smatch[8].simplified());
                if(!predecessor)
                {
                    predecessor = new OrtiJob(smatch[8].simplified(), smatch[9], smatch[10], smatch[7]);
                    orti.addJob(predecessor);
                }
                if(!(smatch[1]==smatch[7] && (smatch[2].endsWith(orti.getArtefact().remove(0,1))||smatch[8].endsWith(orti.getArtefact().remove(0,1)))))
                {
                    OrtiLink* link = new OrtiLink(predecessor, successor, smatch[6]);
                    orti.addLink(link);
                }
            }

            QRegExp topH(" [*]{2} DEPENDANCES HORAIRES .*");
            if(topH.indexIn(line)!=-1) stage = 1;

            QRegExp topHLink("(.{8}) (.{8}) (.{4}) (.{3})  (.{7}) (.{4}) ZZIA([0-9]{2})([0-9]{2}).*");

            if(stage==1 && topHLink.indexIn(line)!=-1)
            {
                qDebug()<<"TopH: "<<line;
                //"GJS42G01 GR8GJ1   GREE CPU  J.O.-23 ADDH ZZIA2000                        CPTPO5"
                QStringList smatch = topHLink.capturedTexts();

                OrtiJob* successor = orti.getJob(smatch[2].simplified());

                if(! successor)
                {
                    successor = new OrtiJob(smatch[2].simplified(), smatch[3], smatch[5], smatch[1]);
                    orti.addJob(successor);
                }

                OrtiTopH* topH =  orti.getTopH(smatch[7]+"h"+smatch[8]);
                if(!topH)
                {
                    topH = new OrtiTopH(smatch[7]+"h"+smatch[8]);
                    orti.addTopH(topH);
                }

                OrtiLink* link = new OrtiLink(successor, topH, smatch[6]);
                orti.addLink(link);

            }

            QRegExp resources(" [*]{2} DEPENDANCES RESSOURCES .*");
            if(resources.indexIn(line)!=-1) stage = 2;

            QRegExp resourceLink("^(.{8}) (.{8}) (.{4}) (.{3})  (.{7}) (.{4}) (.{13}).*");

            if(stage==2 && resourceLink.indexIn(line)!=-1)
            {
                qDebug()<<"Ressource : "<<line;
                //"GJS42G01 GBG0JJ   CPTG CPU  J.O.-23 ADSR INSG-PROD     X             -   CPTPO5"
                QStringList smatch = resourceLink.capturedTexts();
                OrtiJob* successor = orti.getJob(smatch[2].simplified());
                if(! successor)
                {
                    successor = new OrtiJob(smatch[2].simplified(), smatch[3], smatch[5], smatch[1]);
                    orti.addJob(successor);
                }
                OrtiResource* resource =  orti.getResource(smatch[7]);
                if(!resource)
                {
                    resource = new OrtiResource(smatch[7]);
                    orti.addResource(resource);
                }
                OrtiLink* link = new OrtiLink(successor, resource, smatch[6]);
                orti.addLink(link);

            }

        }

        return orti;
    }

    return OrtiFile();
}
