#include "actionrecentfile.h"
#include "mainwindow.h"

ActionRecentFile::ActionRecentFile(QAction* _action, const QString &_relativePath, MainWindow *_main):
    relativePath(_relativePath), action(_action), main(_main)
{ connect(action, SIGNAL(triggered()), this, SLOT(loadFile())); }

void ActionRecentFile::loadFile()
{
    QDir workspace(WORKSPACE);
    QString finalPath = workspace.cleanPath(workspace.absoluteFilePath(relativePath));
    main->load(finalPath);
}
