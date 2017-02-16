#ifndef ACTIONRECENTFILE_H
#define ACTIONRECENTFILE_H

#include <QAction>
class MainWindow;

/*! \class ActionRecentFile
 * \brief Classe représentant les actions du menu Fichier permettant d'ouvrir un fichier récemment utilisé
 *
 * Comprend notamment une action, et un chemin de fichier lié
 * Au clic sur l'action figurant dans le menu, on lance le slot loadFile() qui ouvre le fichier associé
 * La classe dérive de QObject afin de bénéficier du mécanisme de connexion entre le signal triggered() de l'action et le slot loadFile()
 */
class ActionRecentFile : public QObject
{
    Q_OBJECT

private:
    QString relativePath; /*! Chemin relatif du fichier lié*/
    QAction* action; /*! Action du menu Fichier*/
    MainWindow* main; /*! Fenêtre principale*/
public:
    ActionRecentFile(QAction *_action, const QString &_relativePath, MainWindow* _main);
    ~ActionRecentFile() { delete action; }
    QString getRelativePath() const { return relativePath; }
    void setRelativePath(const QString &value) { relativePath = value; action->setText(relativePath); }
    QAction *getAction() const { return action; }
    void setAction(QAction *value) { action = value; }

public slots:
    void loadFile();
};

#endif // ACTIONRECENTFILE_H
