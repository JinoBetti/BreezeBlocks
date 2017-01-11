#ifndef FILEPLUGININTERFACE_H
#define FILEPLUGININTERFACE_H
#include <QString>
#include <QtXml>

class TestPlugin
{
public:
    virtual ~TestPlugin() {}
    virtual void display()=0;
};
#define TestPlugin_iid "BreezeBlocks.TestPlugin"
Q_DECLARE_INTERFACE(TestPlugin, TestPlugin_iid)

class FilePlugin
{
public:
    virtual ~FilePlugin() {}
    virtual void translate(const QString &_path)=0;
    virtual QString getExtension()=0;
    virtual QDomElement translateToGraphics()=0;
    virtual QString getTabName()=0;
    virtual QString getTitle()=0;
};
#define FilePlugin_iid "BreezeBlocks.FilePlugin"
Q_DECLARE_INTERFACE(FilePlugin, FilePlugin_iid)

#endif // FILEPLUGININTERFACE_H

