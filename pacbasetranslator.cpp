#include "pacbasetranslator.h"
#include<QRegExp>
#include<QFile>
#include<QDebug>

PacbaseProgram PacbaseTranslator::translate(const QString &_path)
{
    QFile inputPCLFile(_path);
    if(!inputPCLFile.open(QIODevice::ReadOnly|QIODevice::Text)) { qDebug()<<"Erreur de lecture du programmme pacbase!"; exit(1); }
    else
    {
        bool hasSD = false,
             hasMacro = false,
             hasWorking = false,
             hasFunctions = false;
        bool inLevel99 = false;
        PacbaseProgram program;
        QString workingStorage = "";
        QString macroDefinitions = "";
        QString dataStructures = "";
        QString _sep= "";
        Function* currentFunction = 0;
        SubFunction* currentSubFunction = 0;
        int currentLevel = 0;

        QTextStream stream(&inputPCLFile);
        while(!stream.atEnd())
        {
            QString line = stream.readLine();

            qDebug()<<"First : "<<line;
            if(line.startsWith("0LISTE DES STRUCTURES DE DONNEES DU PROGRAMME"))
            {
               //qDebug()<<"Liste de SD trouvée";
               hasSD = true;
               qDebug()<<"Skipped : "<<stream.readLine(); // 0----
               line = stream.readLine(); // ! PROGR.
               _sep = line.at(1);
               qDebug()<<"Skipped : "<<line;
               qDebug()<<"Skipped : "<<stream.readLine(); // !
               qDebug()<<"Skipped : "<<stream.readLine(); // -----
               line = stream.readLine();
               qDebug()<<"Processed (?) (structure) : "<<line;

               QRegExp SDLine("^ "+_sep+" (.{6}) "+_sep+" (.{2}) (.{2}) (.{6}) "+_sep+" (.{5}) "+_sep+" (.{25}) "+_sep+" (.{5}) "+_sep+" (.{12}) "+_sep+" (.{1}) "+_sep+" (.{6}) "+_sep+" (.{9}) "+_sep+".*");

               while(SDLine.indexIn(line)!=-1)
               {
                   dataStructures += line + "\n";
                   //QStringList smatch = SDLine.capturedTexts();
                   //qDebug()<<smatch[2]<<smatch[3]<<smatch[4]<<smatch[5]<<smatch[11];
                   line = stream.readLine();
                   qDebug()<<"Processed (?) (structure) : "<<line;
               }
            }

            if(line.startsWith("0MACRO-STRUCTURES"))
            {
               //qDebug()<<"Liste de Macros trouvée";
               hasMacro = true;
               qDebug()<<"Skipped : "<<stream.readLine(); // 0----
               line = stream.readLine(); // ! MACRO.
               if(_sep.isEmpty()) _sep = line.at(1);
               qDebug()<<"Skipped : "<<line;
               qDebug()<<"Skipped : "<<stream.readLine(); // -----
               line = stream.readLine();
               qDebug()<<"Processed (?) (macro) : "<<line;
               QRegExp macroLine("^ "+_sep+" (.{6}) "+_sep+" (.{2}) "+_sep+" (.{1}) "+_sep+" (.{50}) "+_sep+".*");

               while(macroLine.indexIn(line)!=-1)
               {
                   macroDefinitions += line + "\n";
                   //QStringList smatch = macroLine.capturedTexts();
                   //qDebug()<<smatch[1]<<smatch[2]<<smatch[4];
                   line = stream.readLine();
                   qDebug()<<"Processed (?) (macro) : "<<line;
               }
            }

            if(line.startsWith("0ZONES DE TRAVAIL UTILISATEUR"))
            {
               //qDebug()<<"Working trouvée";
               hasWorking = true;
               qDebug()<<"Skipped (1) : "<<stream.readLine(); // 0----
               line = stream.readLine(); // ! LIGNE
               if(_sep.isEmpty()) _sep = line.at(1);
               qDebug()<<"Skipped (2) : "<<line;
               qDebug()<<"Skipped (3) : "<<stream.readLine(); // -----

               line = stream.readLine();
               qDebug()<<"Processed (?) (Working) : "<<line;
               QRegExp workingLine("^ "+_sep+" (.{5}) "+_sep+" (.{1}) "+_sep+" (.{75}) "+_sep+" (.{5}) "+_sep+" (.{13}) "+_sep+" (.{6}) "+_sep+".*");
               QRegExp overridenLine("^ "+_sep+"[*].*");

               while(workingLine.indexIn(line)!=-1 || overridenLine.indexIn(line)!=-1)
               {
                   if(overridenLine.indexIn(line)!=-1)
                   {
                       qDebug()<<"Ligne de macro écrasée : "<<line;
                   }
                   else if(workingLine.indexIn(line)!=-1)
                   {
                       //QStringList smatch = workingLine.capturedTexts();
                       workingStorage+=line+"\n";
                       //qDebug()<<smatch[1]<<smatch[3]<<smatch[6];
                   }

                   line = stream.readLine();
                   qDebug()<<"Processed (?) (Working) : "<<line;
               }
            }

            QRegExp traitement("^0TRAITEMENTS SPECIFIQUES DU PROGRAMME +([0-9A-Za-z]+) +(.+)NATURE PROGRAMME");
            QRegExp subTraitement("^0TRAITEMENTS SPECIFIQUES DU PROGRAMME +([0-9A-Za-z]+) +(.+)NATURE SOUS-PROGRAMME");

            if(traitement.indexIn(line)!=-1 || subTraitement.indexIn(line)!=-1)
            {
               //qDebug()<<"traitements trouvés"<<line;
               QStringList smatchTraitement = traitement.capturedTexts();
               if(subTraitement.indexIn(line)!=-1) smatchTraitement = subTraitement.capturedTexts();
               program.setName(smatchTraitement[1]);
               program.setDescription(smatchTraitement[2].simplified().replace("‚","é"));
               hasFunctions = true;
               qDebug()<<"Skipped (1) : "<<stream.readLine(); // 0----
               line = stream.readLine(); // ! FO SF
               if(_sep.isEmpty()) _sep = line.at(1);
               qDebug()<<"Skipped (2) : "<<line;
               qDebug()<<"Skipped (3) : "<<stream.readLine(); // -----
               line = stream.readLine();
               qDebug()<<"Processed (?) (traitement) : "<<line;

               QRegExp functionLine("^ "+_sep+" (.{2}) (.{2}) (.{3}) "+_sep+" (.{3}) (.{36}) "+_sep+" (.{15}) "+_sep+" (.{2}) "+_sep+" (.{35}) "+_sep+"(.{6}) "+_sep+".*");
               QRegExp separationLine("^ -{130}.*");
               QRegExp overridenLine("^ "+_sep+"[*].*");
               qDebug()<<line<<functionLine.indexIn(line)<<separationLine.indexIn(line)<<overridenLine.indexIn(line);

               while(functionLine.indexIn(line)!=-1 || separationLine.indexIn(line)!=-1 || overridenLine.indexIn(line)!=-1)
               {
                   if(overridenLine.indexIn(line)!=-1)
                   {
                       qDebug()<<"Ligne de macro écrasée : "<<line;
                   }
                   if(functionLine.indexIn(line)!=-1)
                   {
                       // ! 02 ID     ! N   PB START /BDPACT                     !     15          ! IT ! ZS15-ZCDRTR NOT = '00'              !       !9230  !

                       QStringList smatch = functionLine.capturedTexts();
                       //qDebug()<<smatch[1]<<smatch[2]<<smatch[3]<<smatch[4]<<smatch[5];
                       Instruction* currentLine = new Instruction(line, smatch);
                       /// Rupture sur fonction
                       if(!currentFunction || smatch[1] != currentFunction->getName())
                       {
                           currentFunction = new Function(smatch[1]);
                           program.addFunction(currentFunction);
                       }
                       /// Rupture sur sous fonction
                       if(!currentSubFunction || smatch[2] != currentSubFunction->getName())
                       {
                           if(currentSubFunction) qDebug()<<smatch[2]<<currentSubFunction->getName();

                           currentLevel = 0;
                           inLevel99 = false;
                           if(!smatch[6].simplified().isEmpty()) currentLevel = smatch[6].toInt();
                           currentSubFunction = new SubFunction(smatch[2],currentLevel, smatch[7].simplified(), smatch[8].simplified());
                           if(smatch[4]=="SUP") currentSubFunction->setIsSup(true);
                           currentFunction->addSubFunction(currentSubFunction);
                           if(smatch[4]=="N  "||smatch[4]=="SUP") currentSubFunction->setTitle(smatch[5].simplified());
                       }
                       /// 99IT
                       else if (smatch[6].toInt()==99)
                       {
                           inLevel99 = true;
                       }
                       else if(!inLevel99 && !smatch[8].simplified().isEmpty()) /// gérer les conditions sur plusieurs lignes (sauf pour les niveaux 99)
                       {
                            if(smatch[7]=="AN") currentSubFunction->setConditions(currentSubFunction->getConditions()+" AND "+smatch[8].simplified());
                            else if(smatch[7]=="OR") currentSubFunction->setConditions(currentSubFunction->getConditions()+" OR "+smatch[8].simplified());
                            else currentSubFunction->setConditions(currentSubFunction->getConditions()+" "+smatch[8].simplified());
                       }

                       currentSubFunction->addInstruction(currentLine);
                   }

                   line = stream.readLine();
                   qDebug()<<"Processed (?) (traitement) : "<<line;

               }
            }
        }

        program.setDataStructures(dataStructures);
        program.setWorkingStorage(workingStorage);
        program.setMacroDefinitions(macroDefinitions);

        return program;
    }
    return PacbaseProgram();
}
