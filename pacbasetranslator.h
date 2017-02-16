#ifndef PACBASETRANSLATOR_H
#define PACBASETRANSLATOR_H
#include <QString>
#include "pacbaseprogram.h"

class PacbaseTranslator
{
public:
    PacbaseProgram translate(const QString &_path);
};

#endif // PACBASETRANSLATOR_H
