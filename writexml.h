#ifndef WRITEXML_H
#define WRITEXML_H

#include <QString>
#include <QObject>
#include <QtXml>
#include "nodegraphic.h"
#include "linkgraphic.h"
#include "customgraphics.h"

class WriteXML : public QObject
{
    Q_OBJECT
private:
    CustomGraphics* graphics;
    QDomDocument doc;
    QDomElement jobset;
    QDomElement graphData;
    QFile file;
    QTextStream out;
    int accepted;

public:
    WriteXML(CustomGraphics *_graphics, const QString &_filename, bool _tmp = false);
    ~WriteXML();
    int getAccepted() const;
    void setAccepted(int value);

public slots:
    QDomElement write();
    QDomElement createNode(NodeGraphic* _node);
    QDomElement createLink(LinkGraphic* _link);

};

#endif // WRITEXML_H
