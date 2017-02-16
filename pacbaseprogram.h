#ifndef PACBASEPROGRAM_H
#define PACBASEPROGRAM_H

#include <QVector>
#include <QColor>
#include <QString>
#include "nodegraphic.h"

class Instruction
{
private:
    QString fullText;
    QString function;
    QString subfunction;
    int line;
    QString operation;
    QString operandes;
    int level;
    QString type;
    QString conditions;
    QString macro;
public:
    Instruction(QString _fullText, QStringList _smatch);

    QString getOperation() const { return operation; }
    void setOperation(const QString &value) { operation = value; }

    QString getOperandes() const { return operandes; }
    void setOperandes(const QString &value) { operandes = value; }

    int getLevel() const { return level; }
    void setLevel(int value) { level = value; }

    QString getType() const { return type; }
    void setType(const QString &value) { type = value; }

    QString getConditions() const { return conditions; }
    void setConditions(const QString &value) { conditions = value; }

    QString getFullText() const { return fullText; }
    void setFullText(const QString &value) { fullText = value; }

    int getLine() const { return line; }
    void setLine(int value) { line = value; }
};

class SubFunction
{
private:
    QString name;
    int level;
    QString type;
    QString conditions;
    bool isSup;
    QString title;
    QVector<Instruction*> instructions;
    NodeGraphic* associatedNode;
    bool isITOfCaseOf;

public:
    SubFunction(QString _name, int _level, QString _type, QString _condition);
    void addInstruction(Instruction* _instruction) { instructions.push_back(_instruction); }

    QString getName() const { return name; }
    void setName(const QString &value) { name = value; }

    int getLevel() const { return level; }
    void setLevel(int value) { level = value; }

    QString getType() const { return type; }
    void setType(const QString &value) { type = value; }

    QString getConditions() const { return conditions; }
    void setConditions(const QString &value) { conditions = value; }

    QString getFullText() const;

    QString getLevel99Text(Instruction* _firstInstruction) const;
    QString getLevel99Condition(Instruction *_firstInstruction, QString _lastCondition) const;

    QVector<Instruction *> getInstructions() const;
    void setInstructions(const QVector<Instruction *> &value);
    bool getIsSup() const;
    void setIsSup(bool value);
    QString getTitle() const;
    void setTitle(const QString &value);
    NodeGraphic *getAssociatedNode() const;
    void setAssociatedNode(NodeGraphic *value);
    bool getIsITOfCaseOf() const;
    void setIsITOfCaseOf(bool value);
};

class Function
{
private:
    QString name;
    QVector<SubFunction*> subfonctions;

public:
    Function(QString _name):name(_name){}
    void addSubFunction(SubFunction* _subfunction) { subfonctions.push_back(_subfunction); }

    QString getName() const { return name; }
    void setName(const QString &value) { name = value; }

    QString getStructure() const;

    QColor getDefaultColor() const;

    QVector<SubFunction *> getSubfonctions() const { return subfonctions; }
    void setSubfonctions(const QVector<SubFunction *> &value) { subfonctions = value; }
};

class PacbaseProgram
{
private:
    QVector<Function*> functions;
    QString name;
    QString description;
    QString workingStorage;
    QString macroDefinitions;
    QString dataStructures;

public:
    PacbaseProgram(){}
    ~PacbaseProgram(){}
    QVector<Function *> getFunctions() const { return functions; }
    void addFunction(Function* _function) { functions.push_back(_function); }
    void setFunctions(const QVector<Function *> &value) { functions = value; }

    QString getName() const { return name; }
    void setName(const QString &value) { name = value; }

    QString getDescription() const { return description; }
    void setDescription(const QString &value) { description = value; }
    SubFunction* getSubfunction(const QString &_name) const;

    QString getWorkingStorage() const { return workingStorage; }
    void setWorkingStorage(const QString &value) { workingStorage = value; }

    QString getMacroDefinitions() const { return macroDefinitions; }
    void setMacroDefinitions(const QString &value) { macroDefinitions = value; }

    QString getDataStructures() const { return dataStructures; }
    void setDataStructures(const QString &value) { dataStructures = value; }
};

#endif // PACBASEPROGRAM_H
