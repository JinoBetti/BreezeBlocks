#ifndef LOADXML_H
#define LOADXML_H
#include <QtXml>
#include <QObject>
#include "customgraphics.h"
#include <QMessageBox>


class LoadXML : public QObject
{
    Q_OBJECT
public:
    /// Ouverture d'un fichier
    LoadXML(const QString&, CustomGraphics *_graphics, bool _critical = true);
    /// Ouverture d'une sauvegarde temporaire
    LoadXML(const QDomElement &_config, CustomGraphics *_graphics):graphics(_graphics),config(_config),loaded(true),error(false){}
    bool getLoaded() const{return loaded;}
    void setLoaded(bool value){loaded = value;}

    bool getError() const {return error;}
    void setError(bool value){error = value;}
    QDate readModificationDate();
    QTime readModificationTime();
    QString readUser();

public slots :
    bool read();
    bool readTmp();

private :
    CustomGraphics* graphics;
    QDomElement config;
    bool loaded;
    bool error;

};

#endif // LOADXML_H
