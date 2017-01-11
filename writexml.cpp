#include "writexml.h"
#include "loadxml.h"
#include "conflictdialog.h"


int WriteXML::getAccepted() const { return accepted; }

void WriteXML::setAccepted(int value) { accepted = value; }

WriteXML::WriteXML(CustomGraphics* _graphics, const QString &_filename, bool _tmp):graphics(_graphics)
{
    //Vérification si enregistrement concurrent
    accepted = 1;
    LoadXML load(_filename, _graphics, false);
    //qDebug()<<"WriteXML constructeur, error:"<<load.getError();
    if(!load.getError()&&_graphics->getModificationDate()!=QDate() && !_tmp)
    {
        //Warning si le fichier a été enregistré par qqun d'autre
        if(load.readModificationDate()>_graphics->getModificationDate()||
           (load.readModificationDate()==_graphics->getModificationDate()&&load.readModificationTime()>_graphics->getModificationTime()))
        {
            ConflictDialog dialog;
            dialog.setUser(load.readUser());
            dialog.setDateAndTime(load.readModificationDate().toString()+" "+load.readModificationTime().toString());
            accepted = dialog.exec();
        }
    }

    if(accepted<=0) return;

    //Début de l'enregistrement
    graphData = doc.createElement("Data");
    doc.appendChild(graphData);

    if(graphics->getTitle())
    {
        graphData.setAttribute("tabname", graphics->getTabName());
        graphData.setAttribute("title", graphics->getTitle()->getTitle());
        graphData.setAttribute("version", graphics->getTitle()->getVersion());

        QDate modificationDate = QDate::currentDate();
        graphData.setAttribute("modificationDate", modificationDate.toString());
        _graphics->setModificationDate(modificationDate);

        QTime modificationTime = QTime::currentTime();
        graphData.setAttribute("modificationTime", modificationTime.toString());
        _graphics->setModificationTime(modificationTime);

        QString username = qgetenv("USER");
        if (username.isEmpty()) username = qgetenv("USERNAME");
        graphData.setAttribute("user",username);
        _graphics->setModificationUser(username);
    }
    file.setFileName(_filename);
    if (!file.open(QIODevice::WriteOnly)) // ouverture du fichier de sauvegarde
        return; // en écriture
    out.setDevice(&file); // association du flux au fichier
}

QDomElement WriteXML::write()
{
    //qDebug()<<"WriteXML::write()"<<graphics->getNodeGraphics().size()<<graphics->getLinkGraphics().size();
    QDomElement nodes = doc.createElement("Nodes");
    graphData.appendChild(nodes);
    for(int i=0; i<graphics->getNodeGraphics().size(); i++)
    {
        QDomElement currentNode = createNode(graphics->getNodeGraphics()[i]);
        nodes.appendChild(currentNode);
    }

    QDomElement links = doc.createElement("Links");
    graphData.appendChild(links);
    for(int i=0; i<graphics->getLinkGraphics().size(); i++)
    {
        QDomElement currentLink = createLink(graphics->getLinkGraphics()[i]);
        links.appendChild(currentLink);
    }

    return graphData;
}

QDomElement WriteXML::createNode(NodeGraphic*_node)
{
    QDomElement node = doc.createElement("Node");
    node.setAttribute("type", _node->getType());
    node.setAttribute("name", _node->getName());
    node.setAttribute("x", _node->pos().x());
    node.setAttribute("y", _node->pos().y());
    node.setAttribute("gridX", _node->getXPos());
    node.setAttribute("gridY", _node->getYPos());
    node.setAttribute("id", _node->getId());
    node.setAttribute("width", _node->getWidth());
    node.setAttribute("height",_node->getHeight());
    node.setAttribute("automatic",_node->getAutomatic());

    if(_node->getColorSaved().name()!= _node->getDefaultColor().name())
    {
        QDomElement colorNode = doc.createElement("Color");

        if(_node->getColorSaved().alpha() != 255) colorNode.setAttribute("alpha",_node->getColorSaved().alpha());

        node.appendChild(colorNode);
        QDomText colorValue = doc.createTextNode(_node->getColorSaved().name());
        colorNode.appendChild(colorValue);
    }

    if(_node->getComment()!="")
    {
        QDomElement commentNode = doc.createElement("Comment");
        node.appendChild(commentNode);
        QDomText commentValue = doc.createTextNode(_node->getComment());
        commentNode.appendChild(commentValue);
    }

    if(_node->getDetailPath()!="")
    {
        QDomElement detailNode = doc.createElement("Detail");
        node.appendChild(detailNode);
        QDomText detailValue = doc.createTextNode(_node->getDetailPath());
        detailNode.appendChild(detailValue);
    }

    if(_node->getType()=="DCLF")
    {
        if(!_node->getClass().isEmpty())
        {
            QDomElement classNode = doc.createElement("Class");
            node.appendChild(classNode);
            QDomText classValue = doc.createTextNode(_node->getClass());
            classNode.appendChild(classValue);
        }

        if(!_node->getFormat().isEmpty())
        {
            QDomElement formatNode = doc.createElement("Format");
            node.appendChild(formatNode);
            QDomText formatValue = doc.createTextNode(_node->getFormat());
            formatNode.appendChild(formatValue);
        }

        if(_node->getLength()!=0)
        {
            QDomElement lengthNode = doc.createElement("Length");
            node.appendChild(lengthNode);
            QDomText lengthValue = doc.createTextNode(QString::number(_node->getLength()));
            lengthNode.appendChild(lengthValue);
        }

        if(! _node->getPrecision().isEmpty())
        {
            QDomElement precisionNode = doc.createElement("Precision");
            node.appendChild(precisionNode);
            QDomText precisionValue = doc.createTextNode(_node->getPrecision());
            precisionNode.appendChild(precisionValue);
        }
    }

    if(_node->getType()=="CUSTOM")
    {
        node.setAttribute("contour", _node->getTransparentContour());
        node.setAttribute("shadow", _node->getHasShadow());
        node.setAttribute("shape", _node->getShape());
        node.setAttribute("zvalue", _node->zValue());
        node.setAttribute("movable", _node->getMovable());
        node.setAttribute("hzAlign", _node->getHzAlign());
        node.setAttribute("vtAlign", _node->getVtAlign());

        node.setAttribute("font", _node->getFont().family());
        if(_node->getFontSize()) node.setAttribute("fontSize", _node->getFontSize());

        QDomElement textNode = doc.createElement("Text");
        node.appendChild(textNode);
        QDomText textValue = doc.createTextNode(_node->getProgram());
        textNode.appendChild(textValue);

    }

    if(_node->getType()=="FUNCTION")
    {
        node.setAttribute("name", _node->getName());
        node.setAttribute("level", _node->getLevel());
        node.setAttribute("callmethod", _node->getCallMethod());
        if(! _node->getPrecision().isEmpty())
        {
            QDomElement precisionNode = doc.createElement("Precision");
            node.appendChild(precisionNode);
            QDomText precisionValue = doc.createTextNode(_node->getPrecision());
            precisionNode.appendChild(precisionValue);
        }
    }

    if(_node->getType()=="STEP")
    {
        node.setAttribute("technical", _node->getIsTechnical());

        QDomElement pgmNode = doc.createElement("Program");
        node.appendChild(pgmNode);
        QDomText pgmValue = doc.createTextNode(_node->getProgram());
        pgmNode.appendChild(pgmValue);
        if(_node->getSysinValue()!="")
        {
            QDomElement sysinNode = doc.createElement("Sysin");
            node.appendChild(sysinNode);
            QDomText sysinValue = doc.createTextNode(_node->getSysinValue());
            sysinNode.appendChild(sysinValue);
        }

        if(_node->getConditionValue()!="")
        {
            QDomElement conditionNode = doc.createElement("Condition");
            node.appendChild(conditionNode);
            conditionNode.setAttribute("color",_node->getCondition()->getColor().name());
            QDomText conditionValue = doc.createTextNode(_node->getConditionValue());
            conditionNode.appendChild(conditionValue);
        }

        if(! _node->getPrecision().isEmpty())
        {
            QDomElement precisionNode = doc.createElement("Precision");
            node.appendChild(precisionNode);
            QDomText precisionValue = doc.createTextNode(_node->getPrecision());
            precisionNode.appendChild(precisionValue);
        }
    }

    return node;
}

QDomElement WriteXML::createLink(LinkGraphic* _link)
{
    QDomElement link = doc.createElement("Link");
    link.setAttribute("inputID", QString::number(_link->getInput()->getId()));
    link.setAttribute("outputID", QString::number(_link->getOutput()->getId()));
    link.setAttribute("ddname",_link->getDDName());
    if(_link->getShape()) link.setAttribute("linkType", QString::number(_link->getShape()));
    if(_link->getPenStyle()!= Qt::SolidLine) link.setAttribute("penStyle", QString::number(_link->getPenStyle()));
    if(_link->getPonderation()!=50) link.setAttribute("ponderation", QString::number(_link->getPonderation()));
    if(!_link->getEndShape()) link.setAttribute("endShape", _link->getEndShape());

    return link;
}

WriteXML::~WriteXML()
{
    QDomNode noeud = doc.createProcessingInstruction("xml","version=\"1.0\"");
    doc.insertBefore(noeud,doc.firstChild());
    doc.save(out,2);
    file.close();
}
