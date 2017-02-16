#ifndef ORTITRANSLATOR_H
#define ORTITRANSLATOR_H
#include <QString>
#include "ortifile.h"

class OrtiTranslator
{
public:
    OrtiFile translate(const QString &_path);
};

#endif // ORTITRANSLATOR_H
