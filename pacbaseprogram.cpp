#include "pacbaseprogram.h"
#include <QDebug>

SubFunction *PacbaseProgram::getSubfunction(const QString &_name) const
{
    for(int i=0; i<functions.size(); i++)
    {
        if(!_name.startsWith(functions[i]->getName())) continue;
        QVector<SubFunction*> subfunctions = functions[i]->getSubfonctions();
        for(int j=0; j<subfunctions.size(); j++)
        {
            if(!_name.endsWith(subfunctions[j]->getName())) continue;
            return subfunctions[j];
        }
    }

    return 0;
}

QString Function::getStructure() const
{
    /// \todo retrouver comment faire pour mettre un flux a la place avec un endl
    QString structure;
    QString indent;
    for(int i=0; i<subfonctions.size(); i++)
    {
        if(i!=0)
        {
            if(subfonctions[i-1]->getLevel() < subfonctions[i]->getLevel()) indent += "  ";
            else if(subfonctions[i-1]->getLevel() > subfonctions[i]->getLevel()) indent.chop(2);
        }

        structure+=indent + name + subfonctions[i]->getName() +" ("+QString::number(subfonctions[i]->getLevel())+ subfonctions[i]->getType()+")";
        if(i<subfonctions.size()-1) structure += "\r\n";
    }

    return structure;
}

QColor Function::getDefaultColor() const
{
    QColor color = Qt::white;

    if(name<"8" && name>"20") color = Qt::green;
    if(name<"20") color = Qt::cyan;
    if(name>"90") color = Qt::yellow;

    return color;
}

Instruction::Instruction(QString _fullText, QStringList _smatch)
{
    fullText = _fullText;
    function = _smatch[1];
    subfunction = _smatch[2];
    line = _smatch[3].toInt();
    operation = _smatch[4];
    operandes = _smatch[5];
    level = _smatch[6].toInt();
    type = _smatch[7];
    conditions = _smatch[8];
    macro = _smatch[9];
}

QString SubFunction::getFullText() const
{
    QString fullText;
    for(int i=0; i<instructions.size(); i++)
    {
        fullText+=instructions[i]->getFullText();
        if(i<instructions.size()-1) fullText += "\r\n";

    }
    return fullText;
}

QString SubFunction::getLevel99Text(Instruction *_firstInstruction) const
{
    QString text;
    bool firstReached = false;
    for(int i=0; i<instructions.size(); i++)
    {
        /// Tope l'instruction passée en paramètre quand on la trouve
        if(instructions[i] == _firstInstruction) firstReached = true;
        /// On passe toutes les lignes précédentes
        if(!firstReached) continue;
        /// pour les instructions suivantes (pas celle où se trouve le niveau 99)
        if(instructions[i] != _firstInstruction)
        {
            if(instructions[i]->getLevel()) break;
            text += "\r\n";
        }

        text += instructions[i]->getFullText();

    }
    return text;
}

QString SubFunction::getLevel99Condition(Instruction *_firstInstruction,QString _lastCondition) const
{
    QString condition = "";
    if(_firstInstruction->getType()=="EL")
    {
        condition = "NOT ("+_lastCondition+")";
        return condition;
    }
    bool firstReached = false;
    for(int i=0; i<instructions.size(); i++)
    {
        /// Tope l'instruction passée en paramètre quand on la trouve
        if(instructions[i] == _firstInstruction) firstReached = true;
        /// On passe toutes les lignes précédentes
        if(!firstReached) continue;
        /// pour les instructions suivantes (pas celle où se trouve le niveau 99)
        if(instructions[i] != _firstInstruction)
        {
            if(instructions[i]->getLevel()||instructions[i]->getConditions().simplified().isEmpty()) break;
            condition += "\r\n";
        }

        if(instructions[i]->getType() == "AN") condition += " AND ";
        if(instructions[i]->getType() == "OR") condition += " OR ";

        condition += instructions[i]->getConditions().simplified();

    }

    return condition;
}

QVector<Instruction *> SubFunction::getInstructions() const { return instructions; }
void SubFunction::setInstructions(const QVector<Instruction *> &value) { instructions = value; }

bool SubFunction::getIsSup() const { return isSup; }
void SubFunction::setIsSup(bool value) { isSup = value; }

QString SubFunction::getTitle() const { return title; }
void SubFunction::setTitle(const QString &value) { title = value; }


NodeGraphic* SubFunction::getAssociatedNode() const
{
    return associatedNode;
}

void SubFunction::setAssociatedNode(NodeGraphic *value)
{
    associatedNode = value;
}

bool SubFunction::getIsITOfCaseOf() const
{
    return isITOfCaseOf;
}

void SubFunction::setIsITOfCaseOf(bool value)
{
    isITOfCaseOf = value;
}

SubFunction::SubFunction(QString _name, int _level, QString _type, QString _condition)
{
    isSup = false;
    isITOfCaseOf = false;
    name = _name;
    level = _level;
    type = _type;
    conditions = _condition;
    associatedNode = 0;
}


