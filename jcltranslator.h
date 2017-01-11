#ifndef JCLTRANSLATOR_H
#define JCLTRANSLATOR_H


#include "jobset.h"
class Jobset;

class JCLTranslator
{
public:
    Jobset translate(const QString &_path);
};

#endif // JCLTRANSLATOR_H
