#include "customtabbar.h"
#include "customgraphics.h"
#include <QInputDialog>
#include <QMouseEvent>


CustomTabBar::CustomTabBar(QWidget *_parent) :
QTabBar(_parent), parent(static_cast<CustomTabWidget*>(_parent))
{
    setMovable(true);
    setTabsClosable(true);
}

void CustomTabBar::mouseDoubleClickEvent(QMouseEvent *e)
{
    if (e->button () != Qt::LeftButton)
    {
        QTabBar::mouseDoubleClickEvent (e);
        return;
    }

    int idx = currentIndex ();
    bool ok = true;
    QString newName = QInputDialog::getText (this, tr ("Changer le nom"),tr ("Changer le nom du dessin"), QLineEdit::Normal, tabText (idx), &ok);

    if (ok)
    {
        setTabText (idx, newName);
        CustomGraphics* graphics = static_cast<CustomGraphics*>(parent->widget(idx));
        graphics->setTabName(newName);
        graphics->setModified(true);
    }
}

void CustomTabBar::mousePressEvent(QMouseEvent *e)
{
    if(e->button() == Qt::MiddleButton) emit tabCloseRequested(tabAt(e->pos()));

    if(e->button() == Qt::RightButton)
    {
        QMenu *contextMenu = new QMenu(this);
        contextMenu->setStyleSheet("background-color: lightGrey");
        QAction* actionAddLink = contextMenu->addAction("Créer noeud lien");
        QAction* actionClose = contextMenu->addAction("Fermer le DDC");
        QAction* actionOpenExplorer = contextMenu->addAction("Ouvrir dans l'explorateur");
        QAction* actionFermerToutLesAutres = contextMenu->addAction("Fermer tous les autres ddc");

        QAction* myAction = contextMenu->exec(e->screenPos().toPoint());
        if(myAction== actionAddLink)
        {
            int idxCurrent = currentIndex ();
            int idxClicked = tabAt(e->pos());
            CustomGraphics* currentGraphics = static_cast<CustomGraphics*>(parent->widget(idxCurrent));
            CustomGraphics* clickedGraphics = static_cast<CustomGraphics*>(parent->widget(idxClicked));
            QString tabName = clickedGraphics->getTabName(), path = clickedGraphics->getCurrentFilename();
            currentGraphics->addLinkedNode(tabName, path);
        }

        if(myAction==actionClose) emit tabCloseRequested(tabAt(e->pos()));

        if(myAction==actionOpenExplorer)
        {
            // spécifique à windows
            QString pathIn = static_cast<CustomGraphics*>(parent->widget(tabAt(e->pos())))->getCurrentFilename();
            const QString explorer = QLatin1String("explorer.exe");
            if (explorer.isEmpty())
            {
                QMessageBox::warning(parent,tr("Launching Windows Explorer failed"), tr("Could not find explorer.exe in path to launch Windows Explorer."));
                return;
            }
            QString param;
            if (!QFileInfo(pathIn).isDir()) param = QLatin1String("/select,");
            param += QDir::toNativeSeparators(pathIn);
            QString command = explorer + " " + param;
            QProcess::startDetached(command);
        }
        if(myAction==actionFermerToutLesAutres)
        {
            int idxClicked = tabAt(e->pos());
            for(int i=count()-1; i>=0; i--) {if(i!=idxClicked) emit tabCloseRequested(i);}
        }
    }

    else QTabBar::mousePressEvent(e);
}


void CustomTabBar::focusInEvent(QFocusEvent *e)
{
    QTabBar::focusInEvent(e);

}
