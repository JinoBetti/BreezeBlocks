#include "utils.h"
#include "jobset.h"
#include "pcltranslator.h"
#include <QApplication>
//#include "breezeblocksapp.h"
#include <QtGui>
#include <QtPlugin>
#include "mainwindow.h"
using namespace std;

/*!
 *  \fn int main(int argc, char *argv[])
 *  \brief Fonction principale
 *
 *  \param argc : Nombre d'arguments
 *  \param argv : Tableau des arguments
 */

int main(int argc, char *argv[])
{
    /// Extraction du chemin de l'application pour en déduire le chemin des icônes
    QString pathToExe = argv[0];
    EXE_PATH = QFileInfo(pathToExe).dir();

    /// Extraction du chemin du fichier à partir duquel l'application a été ouverte pour ouvrir le fichier
    QString pathToInputFile = "";
    if(argc>1) pathToInputFile = argv[1];

    /// Création et affichage de la fenêtre principale
//    BreezeBlocksApp app(argc, argv);
    QApplication app(argc, argv);
    MainWindow w(pathToInputFile);
//    app.setWindow(&w);
    w.show();

    return app.exec();
}
