
QT       += core gui xml

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets printsupport

INCLUDEPATH += $$PWD

SOURCES += \
    main.cpp \
    pcltranslator.cpp \
    jobset.cpp \
    customgraphics.cpp \
    mainwindow.cpp \
    stepgraphic.cpp \
    dclfgraphic.cpp \
    linkgraphic.cpp \
    nodegraphic.cpp \
    writexml.cpp \
    sysingraphic.cpp \
    loadxml.cpp \
    anchorgraphic.cpp \
    modifydclfdialog.cpp \
    modifystepdialog.cpp \
    utils.cpp \
    titlegraphic.cpp \
    conditiongraphic.cpp \
    modifylinkdialog.cpp \
    modifytitledialog.cpp \
    savedialog.cpp \
    customtabwidget.cpp \
    customtabbar.cpp \
    savebeforequitdialog.cpp \
    reloaddetaildialog.cpp \
    customnodegraphic.cpp \
    settingsdialog.cpp \
    colorrulewidget.cpp \
    splitdclfdialog.cpp \
    splitdclfwidget.cpp \
    pacbasetranslator.cpp \
    pacbaseprogram.cpp \
    functiongraphic.cpp \
    modifyfunctiondialog.cpp \
    pclrulewidget.cpp \
    jcltranslator.cpp \
    opennewdialog.cpp \
    conditionnodegraphic.cpp \
    modifyconditiondialog.cpp \
    modifycustomnodedialog.cpp \
    actionrecentfile.cpp \
    searchdockwidget.cpp \
    versionupdatedialog.cpp \
    variablevaluesdialog.cpp \
    customgraphicscene.cpp \
    loadlastversiondialog.cpp \
    conflictdialog.cpp \
    breezeblocksapp.cpp \
    propertiesdialog.cpp \
    ortitranslator.cpp \
    ortifile.cpp


HEADERS += \
    pcltranslator.h \
    jobset.h \
    customgraphics.h \
    mainwindow.h \
    stepgraphic.h \
    dclfgraphic.h \
    linkgraphic.h \
    nodegraphic.h \
    writexml.h \
    sysingraphic.h \
    loadxml.h \
    anchorgraphic.h \
    modifydclfdialog.h \
    modifystepdialog.h \
    utils.h \
    titlegraphic.h \
    conditiongraphic.h \
    modifylinkdialog.h \
    modifytitledialog.h \
    savedialog.h \
    customtabwidget.h \
    customtabbar.h \
    savebeforequitdialog.h \
    reloaddetaildialog.h \
    customnodegraphic.h \
    settingsdialog.h \
    colorrulewidget.h \
    splitdclfdialog.h \
    splitdclfwidget.h \
    pacbasetranslator.h \
    pacbaseprogram.h \
    functiongraphic.h \
    modifyfunctiondialog.h \
    pclrulewidget.h \
    jcltranslator.h \
    opennewdialog.h \
    conditionnodegraphic.h \
    modifyconditiondialog.h \
    modifycustomnodedialog.h \
    actionrecentfile.h \
    searchdockwidget.h \
    versionupdatedialog.h \
    variablevaluesdialog.h \
    customgraphicscene.h \
    loadlastversiondialog.h \
    conflictdialog.h \
    breezeblocksapp.h \
    propertiesdialog.h \
    fileplugininterface.h \
    ortitranslator.h \
    ortifile.h

FORMS += \
    mainwindow.ui \
    modifydclfdialog.ui \
    modifystepdialog.ui \
    modifylinkdialog.ui \
    modifytitledialog.ui \
    savedialog.ui \
    savebeforequitdialog.ui \
    reloaddetaildialog.ui \
    settingsdialog.ui \
    colorrulewidget.ui \
    splitdclfdialog.ui \
    splitdclfwidget.ui \
    modifyfunctiondialog.ui \
    pclrulewidget.ui \
    opennewdialog.ui \
    modifyconditiondialog.ui \
    modifycustomnodedialog.ui \
    searchdockwidget.ui \
    versionupdatedialog.ui \
    variablevaluesdialog.ui \
    loadlastversiondialog.ui \
    conflictdialog.ui \
    propertiesdialog.ui

RC_FILE = myapp.rc

TARGET     = breezeblocks
win32 {
    CONFIG(debug, release|debug):DESTDIR = ../debug/
    CONFIG(release, release|debug):DESTDIR = ../release/
} else {
    DESTDIR    = ../
}

# install
INSTALLS += target
