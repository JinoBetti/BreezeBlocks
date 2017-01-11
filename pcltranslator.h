#ifndef PCLTRANSLATOR_H
#define PCLTRANSLATOR_H

#include "jobset.h"

class VariableValue;
class Jobset;
class PCLTranslator
{
public:
    PCLTranslator(QVector<VariableValue*> _variables);
    ~PCLTranslator();
    Jobset translate(const QString&);
    VariableValue* getVariableValue(const QString &_name);
    QString changeVariables(QString _line); // pas const
private:
    QVector<VariableValue *> variables;

};

#endif // PCLTRANSLATOR_H
