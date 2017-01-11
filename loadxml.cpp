#include "loadxml.h"
#include "utils.h"

LoadXML::LoadXML(const QString &_filename, CustomGraphics* _graphics, bool _critical):
    graphics(_graphics),
    loaded(false),
    error(false)
{
    QFile file(_filename);
    QDomDocument doc;

    if(_filename.isEmpty())
    {
        error = true;
        return;
    }
    if (!file.open(QIODevice::ReadOnly))
    {
        if(_critical)
        {

            qDebug()<<file.openMode();
            QMessageBox message;
            message.critical(graphics, tr("ERREUR"), "Fichier illisible : "+_filename);

        }

        error = true;
        return;
    }
    if (!doc.setContent(&file)) { file.close(); return; }
    else
    {
        file.close();
        loaded = true;
        config = doc.documentElement();
        return;
    }

    file.close();
}

QDate LoadXML::readModificationDate()
{
    if(! loaded) return QDate();
    QDate modificationDate = QDate::fromString(config.toElement().attribute("modificationDate",QDate().toString()));
    return modificationDate;
}

QString LoadXML::readUser()
{
    if(! loaded) return "";
    QString user = config.toElement().attribute("user","Utilisateur inconnu");
    return user;
}

QTime LoadXML::readModificationTime()
{
    if(! loaded) return QTime();
    QTime modificationTime = QTime::fromString(config.toElement().attribute("modificationTime", QTime().toString()));
    return modificationTime;
}

bool LoadXML::read()
{
    //qDebug()<<"LoadXML::read()";
    if(! loaded) return false;

    QString tabname = config.toElement().attribute("tabname",config.toElement().attribute("jobname"));

    QString title = config.toElement().attribute("title");
    QString version = config.toElement().attribute("version");
    graphics->setTabName(tabname);
    QDate modificationDate = QDate::fromString(config.toElement().attribute("modificationDate"));
    graphics->setModificationDate(modificationDate);
    QTime modificationTime = QTime::fromString(config.toElement().attribute("modificationTime"));
    graphics->setModificationTime(modificationTime);
    graphics->setModificationUser(config.toElement().attribute("user"));
    QDomNode nodes = config.firstChild();
    QDomNode node = nodes.firstChild();

    while(!node.isNull())
    {
        QDomElement nodeElement = node.toElement();

        QString type = nodeElement.attribute("type");
        QString name = nodeElement.attribute("name");
        QString x = nodeElement.attribute("x","0");
        QString y = nodeElement.attribute("y","0");
        QString automatic = nodeElement.attribute("automatic","0");
        QString gridX = nodeElement.attribute("gridX","-1");
        QString gridY = nodeElement.attribute("gridY","-1");
        QString id = nodeElement.attribute("id");
        QString width = nodeElement.attribute("width","100");
        QString height = nodeElement.attribute("height","50");
        QString shape = nodeElement.attribute("shape","0");
        QString contour = nodeElement.attribute("contour","0");
        QString callMethod = nodeElement.attribute("callmethod","");
        QString level = nodeElement.attribute("level","");
        QString background = nodeElement.attribute("background","0");
        QString shadow = nodeElement.attribute("shadow","0");
        QString zValue = nodeElement.attribute("zvalue","1");
        QString movable = nodeElement.attribute("movable","1");
        QString hzAlign = nodeElement.attribute("hzAlign","1");
        QString vtAlign = nodeElement.attribute("vtAlign","1");
        QString font = nodeElement.attribute("font","1");
        QString fontSize = nodeElement.attribute("fontSize","0");

        QDomElement colorElement = nodeElement.firstChildElement("Color");
        QString alpha = colorElement.attribute("alpha","255");

        QColor color;
        if(colorElement != QDomElement()) color = colorElement.firstChild().toText().data();

        QDomElement commentElement = nodeElement.firstChildElement("Comment");
        QString comment(commentElement.firstChild().toText().data());

        QDomElement detailElement = nodeElement.firstChildElement("Detail");
        QString detail(detailElement.firstChild().toText().data());

        QDomElement sysinElement = nodeElement.firstChildElement("Sysin");
        QString sysin(sysinElement.firstChild().toText().data());

        QDomElement conditionElement = nodeElement.firstChildElement("Condition");
        QString condition(conditionElement.firstChild().toText().data());
        QString conditionColor = conditionElement.attribute("color");

        QDomElement programElement = nodeElement.firstChildElement("Program");
        QString program(programElement.firstChild().toText().data());

        QDomElement textElement = nodeElement.firstChildElement("Text");
        QString text(textElement.firstChild().toText().data());

        QDomElement classeElement = nodeElement.firstChildElement("Class");
        QString classe("");
        if(!classeElement.isNull()) classe = classeElement.firstChild().toText().data();

        QDomElement formatElement = nodeElement.firstChildElement("Format");
        QString format("");
        if(! formatElement.isNull()) format = formatElement.firstChild().toText().data();

        QDomElement lengthElement = nodeElement.firstChildElement("Length");
        QString length("0");
        if(! lengthElement.isNull()) length = lengthElement.firstChild().toText().data();

        ///Relique : on laisse la lecture pour garder la retrocompatibilité
        QDomElement segmentsElement = nodeElement.firstChildElement("Segments");
        QString segments = "";
        if(! segmentsElement.isNull()) segments = segmentsElement.firstChild().toText().data();

        QDomElement precisionElement = nodeElement.firstChildElement("Precision");
        QString precision("");
        if(! precisionElement.isNull()) precision = precisionElement.firstChild().toText().data();


        if(type == "DCLF")
        {
            QString precisionV2 = precision + segments;
            DCLFGraphic* newDCLFNode = new DCLFGraphic(name, classe, comment, format, precisionV2, length.toInt(), graphics, false);
            newDCLFNode->setPos(x.toDouble(),y.toDouble());
            newDCLFNode->setWidth(width.toInt());
            newDCLFNode->setHeight(height.toInt());
            if(gridX.toDouble()!=-1 && gridY.toDouble()!=-1)
            {
                newDCLFNode->setXPos(gridX.toDouble());
                newDCLFNode->setYPos(gridY.toDouble());
            }
            newDCLFNode->setAutomatic(automatic.toInt());

            if(color == QColor()) color = newDCLFNode->getColorFromRules(classe);
            newDCLFNode->setColorSaved(color);
            newDCLFNode->setColor(color);

            newDCLFNode->setId(id.toInt());
            newDCLFNode->setDetailPath(detail);
            graphics->addNode(newDCLFNode);
        }
        else if(type=="STEP")
        {
            StepGraphic* newStepNode = new StepGraphic(name, type, program, comment, graphics, false);
            newStepNode->setPos(x.toDouble(), y.toDouble());
            newStepNode->setWidth(width.toInt());
            newStepNode->setHeight(height.toInt());
            if(gridX.toDouble()!=-1 && gridY.toDouble()!=-1)
            {
                newStepNode->setXPos(gridX.toDouble());
                newStepNode->setYPos(gridY.toDouble());
            }
            newStepNode->setAutomatic(automatic.toInt());
            QString isTechnical = nodeElement.attribute("technical");
            if(!isTechnical.isEmpty()) newStepNode->setIsTechnical(isTechnical.toInt());
            if(color == QColor())
            {
                if(isTechnical.toInt()) color = TECHNICAL_STEP_COLOR;
                else color = NORMAL_STEP_COLOR;
            }
            newStepNode->setColorSaved(color);
            newStepNode->setColor(color);

            newStepNode->setPrecision(precision);
            newStepNode->setId(id.toInt());
            newStepNode->setDetailPath(detail);
            newStepNode->setSysin(sysin);
            newStepNode->setCondition(condition);
            newStepNode->getCondition()->setColor(QColor(conditionColor).isValid()?QColor(conditionColor):Qt::red);
            graphics->addNode(newStepNode);

        }
        else if(type=="CUSTOM")
        {
            CustomNodeGraphic* newNode = new CustomNodeGraphic(text, comment, MyShapes::Shape(shape.toInt()), graphics, false);
            newNode->setPos(x.toDouble(), y.toDouble());
            newNode->setAutomatic(0);
            newNode->setWidth(width.toInt());
            newNode->setHeight(height.toInt());

            if(gridX.toDouble()!=-1 && gridY.toDouble()!=-1)
            {
                newNode->setXPos(gridX.toDouble());
                newNode->setYPos(gridY.toDouble());
            }

            if(alpha=="255" && background=="1") alpha="0";
            color.setAlpha(alpha.toInt());
            if(color == QColor()) { color = QColor(Qt::white); }

            newNode->setColorSaved(color);
            newNode->setColor(color);

            newNode->setId(id.toInt());
            newNode->setDetailPath(detail);
            newNode->setZValue(zValue.toInt());

            newNode->setTransparentContour(contour.toInt());
            newNode->setHasShadow(shadow.toInt());
            newNode->setHzAlign(hzAlign.toInt());
            newNode->setVtAlign(vtAlign.toInt());
            newNode->setMovable(movable.toInt());

            newNode->setFont(QFont(font));
            newNode->setFontSize(fontSize.toInt());

            graphics->addNode(newNode);
        }
        else if(type=="FUNCTION")
        {
            FunctionGraphic* newFunctionNode = new FunctionGraphic(name, level, precision ,callMethod, comment, graphics, false);
            newFunctionNode->setPos(x.toDouble(), y.toDouble());
            newFunctionNode->setAutomatic(automatic.toInt());
            newFunctionNode->setWidth(width.toInt());
            newFunctionNode->setHeight(height.toInt());
            if(gridX.toDouble()!=-1 && gridY.toDouble()!=-1)
            {
                newFunctionNode->setXPos(gridX.toDouble());
                newFunctionNode->setYPos(gridY.toDouble());
            }

            if(color == QColor()) color = QColor(Qt::white);

            newFunctionNode->setColorSaved(color);
            newFunctionNode->setColor(color);

            newFunctionNode->setId(id.toInt());
            newFunctionNode->setDetailPath(detail);

            graphics->addNode(newFunctionNode);
        }

        node = node.nextSiblingElement();
    }

    /// Création des liens
    QDomNode links = nodes.nextSibling();

    QDomNode link = links.firstChild();

    while(!link.isNull())
    {
        QDomElement linkElement = link.toElement();
        QString inputId = linkElement.attribute("inputID");
        QString outputId = linkElement.attribute("outputID");

        QString ddname = linkElement.attribute("ddname","");
        QString linkType = linkElement.attribute("linkType","0");
        QString penStyle = linkElement.attribute("penStyle","1");
        QString endShape = linkElement.attribute("endShape","1");
        QString ponderation = linkElement.attribute("ponderation","50");

        if(graphics->getNodeGraphic(inputId.toInt()) && graphics->getNodeGraphic(outputId.toInt()))
        {
            LinkGraphic* newLink = new LinkGraphic(graphics->getNodeGraphic(inputId.toInt()), graphics->getNodeGraphic(outputId.toInt()), ddname, graphics, false);
            newLink->setShape(linkType.toInt());
            newLink->setPenStyle(Qt::PenStyle(penStyle.toInt()));
            newLink->setEndShape(endShape.toInt());
            newLink->setPonderation(ponderation.toInt());
            graphics->addLink(newLink);
        }


        link = link.nextSiblingElement();
    }

    graphics->setTitle(title);
    graphics->getTitle()->setVersion(version);

    //qDebug()<<"LoadXML::read() end";

    return true;
}

bool LoadXML::readTmp()
{
    //qDebug()<<"LoadXML::read()";
    if(! loaded) return false;

    QString tabname = config.toElement().attribute("tabname",config.toElement().attribute("jobname"));

    QString title = config.toElement().attribute("title");
    graphics->setTabName(tabname);
    QDate modificationDate = QDate::fromString(config.toElement().attribute("modificationDate"));
    graphics->setModificationDate(modificationDate);
    QTime modificationTime = QTime::fromString(config.toElement().attribute("modificationTime"));
    graphics->setModificationTime(modificationTime);
    graphics->setModificationUser(config.toElement().attribute("user"));
    QDomNode nodes = config.firstChild();
    QDomNode node = nodes.firstChild();

    while(!node.isNull())
    {
        QDomElement nodeElement = node.toElement();

        QString type = nodeElement.attribute("type");
        QString name = nodeElement.attribute("name");
        QString x = nodeElement.attribute("x","0");
        QString y = nodeElement.attribute("y","0");
        QString automatic = nodeElement.attribute("automatic","0");
        QString gridX = nodeElement.attribute("gridX","-1");
        QString gridY = nodeElement.attribute("gridY","-1");
        QString id = nodeElement.attribute("id");
        QString width = nodeElement.attribute("width","100");
        QString height = nodeElement.attribute("height","50");
        QString shape = nodeElement.attribute("shape","0");
        QString contour = nodeElement.attribute("contour","0");
        QString callMethod = nodeElement.attribute("callmethod","");
        QString level = nodeElement.attribute("level","");
        QString background = nodeElement.attribute("background","0");
        QString shadow = nodeElement.attribute("shadow","0");
        QString zValue = nodeElement.attribute("zvalue","1");
        QString movable = nodeElement.attribute("movable","1");
        QString hzAlign = nodeElement.attribute("hzAlign","1");
        QString vtAlign = nodeElement.attribute("vtAlign","1");
        QString font = nodeElement.attribute("font","1");
        QString fontSize = nodeElement.attribute("fontSize","0");

        QDomElement colorElement = nodeElement.firstChildElement("Color");
        QString alpha = colorElement.attribute("alpha","255");

        QColor color;
        if(colorElement != QDomElement()) color = colorElement.firstChild().toText().data();

        QDomElement commentElement = nodeElement.firstChildElement("Comment");
        QString comment(commentElement.firstChild().toText().data());

        QDomElement detailElement = nodeElement.firstChildElement("Detail");
        QString detail(detailElement.firstChild().toText().data());

        QDomElement sysinElement = nodeElement.firstChildElement("Sysin");
        QString sysin(sysinElement.firstChild().toText().data());

        QDomElement conditionElement = nodeElement.firstChildElement("Condition");
        QString condition(conditionElement.firstChild().toText().data());
        QString conditionColor = conditionElement.attribute("color");

        QDomElement programElement = nodeElement.firstChildElement("Program");
        QString program(programElement.firstChild().toText().data());

        QDomElement textElement = nodeElement.firstChildElement("Text");
        QString text(textElement.firstChild().toText().data());

        QDomElement classeElement = nodeElement.firstChildElement("Class");
        QString classe("");
        if(!classeElement.isNull()) classe = classeElement.firstChild().toText().data();

        QDomElement formatElement = nodeElement.firstChildElement("Format");
        QString format("");
        if(! formatElement.isNull()) format = formatElement.firstChild().toText().data();

        QDomElement lengthElement = nodeElement.firstChildElement("Length");
        QString length("0");
        if(! lengthElement.isNull()) length = lengthElement.firstChild().toText().data();

        ///Relique : on laisse la lecture pour garder la retrocompatibilité
        QDomElement segmentsElement = nodeElement.firstChildElement("Segments");
        QString segments = "";
        if(! segmentsElement.isNull()) segments = segmentsElement.firstChild().toText().data();

        QDomElement precisionElement = nodeElement.firstChildElement("Precision");
        QString precision("");
        if(! precisionElement.isNull()) precision = precisionElement.firstChild().toText().data();


        if(type == "DCLF")
        {
            QString precisionV2 = precision + segments;
            DCLFGraphic* newDCLFNode = new DCLFGraphic(name, classe, comment, format, precisionV2, length.toInt(), graphics, false);
            newDCLFNode->setPos(x.toDouble(),y.toDouble());
            newDCLFNode->setWidth(width.toInt());
            newDCLFNode->setHeight(height.toInt());
            if(gridX.toDouble()!=-1 && gridY.toDouble()!=-1)
            {
                newDCLFNode->setXPos(gridX.toDouble());
                newDCLFNode->setYPos(gridY.toDouble());
            }
            newDCLFNode->setAutomatic(automatic.toInt());

            if(color == QColor()) color = newDCLFNode->getColorFromRules(classe);
            newDCLFNode->setColorSaved(color);
            newDCLFNode->setColor(color);

            newDCLFNode->setId(id.toInt());
            newDCLFNode->setDetailPath(detail);
            graphics->addNode(newDCLFNode);
        }
        else if(type=="STEP")
        {
            StepGraphic* newStepNode = new StepGraphic(name, type, program, comment, graphics, false);
            newStepNode->setPos(x.toDouble(), y.toDouble());
            newStepNode->setWidth(width.toInt());
            newStepNode->setHeight(height.toInt());
            if(gridX.toDouble()!=-1 && gridY.toDouble()!=-1)
            {
                newStepNode->setXPos(gridX.toDouble());
                newStepNode->setYPos(gridY.toDouble());
            }
            newStepNode->setAutomatic(automatic.toInt());
            QString isTechnical = nodeElement.attribute("technical");
            if(!isTechnical.isEmpty()) newStepNode->setIsTechnical(isTechnical.toInt());
            if(color == QColor())
            {
                if(isTechnical.toInt()) color = TECHNICAL_STEP_COLOR;
                else color = NORMAL_STEP_COLOR;
            }
            newStepNode->setColorSaved(color);
            newStepNode->setColor(color);

            newStepNode->setPrecision(precision);
            newStepNode->setId(id.toInt());
            newStepNode->setDetailPath(detail);
            newStepNode->setSysin(sysin);
            newStepNode->setCondition(condition);
            newStepNode->getCondition()->setColor(QColor(conditionColor).isValid()?QColor(conditionColor):Qt::red);
            graphics->addNode(newStepNode);

        }
        else if(type=="CUSTOM")
        {
            CustomNodeGraphic* newNode = new CustomNodeGraphic(text, comment, MyShapes::Shape(shape.toInt()), graphics, false);
            newNode->setPos(x.toDouble(), y.toDouble());
            newNode->setAutomatic(0);
            newNode->setWidth(width.toInt());
            newNode->setHeight(height.toInt());

            if(gridX.toDouble()!=-1 && gridY.toDouble()!=-1)
            {
                newNode->setXPos(gridX.toDouble());
                newNode->setYPos(gridY.toDouble());
            }

            if(alpha=="255" && background=="1") alpha="0";
            color.setAlpha(alpha.toInt());
            if(color == QColor()) { color = QColor(Qt::white); }

            newNode->setColorSaved(color);
            newNode->setColor(color);

            newNode->setId(id.toInt());
            newNode->setDetailPath(detail);
            newNode->setZValue(zValue.toInt());

            newNode->setTransparentContour(contour.toInt());
            newNode->setHasShadow(shadow.toInt());
            newNode->setHzAlign(hzAlign.toInt());
            newNode->setVtAlign(vtAlign.toInt());
            newNode->setMovable(movable.toInt());
            newNode->setFontSize(fontSize.toInt());
            newNode->setFont(QFont(font));

            graphics->addNode(newNode);
        }
        else if(type=="FUNCTION")
        {
            FunctionGraphic* newFunctionNode = new FunctionGraphic(name, level, precision ,callMethod, comment, graphics, false);
            newFunctionNode->setPos(x.toDouble(), y.toDouble());
            newFunctionNode->setAutomatic(automatic.toInt());
            newFunctionNode->setWidth(width.toInt());
            newFunctionNode->setHeight(height.toInt());
            if(gridX.toDouble()!=-1 && gridY.toDouble()!=-1)
            {
                newFunctionNode->setXPos(gridX.toDouble());
                newFunctionNode->setYPos(gridY.toDouble());
            }

            if(color == QColor()) color = QColor(Qt::white);

            newFunctionNode->setColorSaved(color);
            newFunctionNode->setColor(color);

            newFunctionNode->setId(id.toInt());
            newFunctionNode->setDetailPath(detail);

            graphics->addNode(newFunctionNode);
        }

        node = node.nextSiblingElement();
    }

    /// Création des liens
    QDomNode links = nodes.nextSibling();

    QDomNode link = links.firstChild();

    while(!link.isNull())
    {
        QDomElement linkElement = link.toElement();
        QString inputId = linkElement.attribute("inputID");
        QString outputId = linkElement.attribute("outputID");

        QString ddname = linkElement.attribute("ddname","");
        QString linkType = linkElement.attribute("linkType","0");
        QString penStyle = linkElement.attribute("penStyle","1");
        QString endShape = linkElement.attribute("endShape","1");
        QString ponderation = linkElement.attribute("ponderation","50");

        if(graphics->getNodeGraphic(inputId.toInt()) && graphics->getNodeGraphic(outputId.toInt()))
        {
            LinkGraphic* newLink = new LinkGraphic(graphics->getNodeGraphic(inputId.toInt()), graphics->getNodeGraphic(outputId.toInt()), ddname, graphics, false);
            newLink->setShape(linkType.toInt());
            newLink->setPenStyle(Qt::PenStyle(penStyle.toInt()));
            newLink->setEndShape(endShape.toInt());
            newLink->setPonderation(ponderation.toInt());
            graphics->addLink(newLink);
        }


        link = link.nextSiblingElement();
    }

    graphics->setTitle(title);

    //qDebug()<<"LoadXML::read() end";

    return true;
}
