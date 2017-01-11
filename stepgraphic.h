#ifndef STEPGRAPHIC_H
#define STEPGRAPHIC_H

#include "nodegraphic.h"
#include "sysingraphic.h"
#include "conditiongraphic.h"

class CustomGraphics;
class SysinGraphic;
class ConditionGraphic;

class StepGraphic : public NodeGraphic
{
private:
    QString subtype; /// SORT STEP MERGE ou PATTERN
    QString stepName;
    QString program;
    QString precision;

    SysinGraphic* sysin;
    ConditionGraphic* condition;
    bool isTechnical;

public:
    StepGraphic(const QString &_stepName, const QString &_type, const QString &_program, const QString &_comment, CustomGraphics* _graphics, bool _automatic);
    ~StepGraphic(){}
    QRectF boundingRect() const;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);
    QString getName() const {return stepName;}
    void setName(const QString &_name) {stepName = _name;}
    QString getProgram() const {return program;}
    void setProgram(const QString &_program){program = _program;}

    QString getSysinValue() const;
    QString getConditionValue() const;

    void setSysin(const QString &_sysin);
    void setSysin(SysinGraphic* _sysin){sysin = _sysin;}
    SysinGraphic* getSysinGraphic(){return sysin;}

    NodeGraphic* createCopy(CustomGraphics* _graphics);

    bool getIsTechnical() const { return isTechnical;}
    void setIsTechnical(bool value){ isTechnical = value;}
    ConditionGraphic *getCondition() { return condition;}
    void setCondition(ConditionGraphic *value){ condition = value;}
    void setCondition(const QString &value);

    void deleteCondition();
    QString getPrecision() const { return precision;}
    void setPrecision(const QString &value){ precision = value;}

    void repositionSubComponents();

    QString getSubtype() const {return subtype;}
    void setSubtype(const QString &value){subtype = value;}
    QVariant itemChange(GraphicsItemChange change, const QVariant &value);
    QString getText() const;
};

#endif // STEPGRAPHIC_H
