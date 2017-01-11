#include "utils.h"

int LINK_TYPE;
int TITLE_SIZE;// = 40;
int TITLE_MARGIN;// = 40;
int COPY_OFFSET;// = 25;
int ANCHOR_SIZE;// = 8;
int SYSIN_SIZE;// = 4;
int CONDITION_SIZE;// = 15;
int SCENE_MARGIN;// = 50;
int ALIGN_GRID_X;// = 150;
int ALIGN_GRID_X_DIVISION;// = 4;
int ALIGN_GRID_Y_DIVISION;// = 2;
int ALIGN_GRID_Y;// = 75;
int SHADOW_OFFSET;// = 5;
int FONT_SIZE;// = 14;
int DDNAME_FONT_SIZE;// = 12;
int TECHNICAL_FONT_SIZE;// = 14;
int DEFAULT_HEIGHT;// = 50;
int DEFAULT_WIDTH;// = 100;
int DETAILED_WEIGHT;// = 3;
int INSIDE_MARGIN;// = 5;
int RESIZE_STEP;// = 5;
int MAX_RECENT_FILES_SIZE = 20;

double SCALE_CHANGE;// = 1.15

QVector<ColorRule> COLOR_RULES;
QVector<PCLRule> PCL_RULES;
QVector<ActionRecentFile*> RECENT_FILES;

QColor TECHNICAL_STEP_COLOR;// = QColor(90,255,60);
QColor NORMAL_STEP_COLOR;// = QColor(120,255,255);
QColor NORMAL_FILE_COLOR;// = QColor(255,255,100);
QColor PATTERN_STEP_COLOR; // = QColor(255,220,100);
QColor SELECTED_COLOR;// = Qt::yellow;
QColor FILE_HOVER_COLOR;// = Qt::red;

bool DISPLAY_STEPNAME;// = true;
bool DISPLAY_DCLFCLASS;// = true;
bool CONDITIONS_DISPLAYED;// = true;
bool ANCHORS_DISPLAYED;// = false;
bool DISPLAY_DDNAME;// = false;
bool SYSIN_HIDDEN;// = false;
bool DISPLAY_GRID = false;
bool SELECT_MODE;// = false;
bool SELECT_BRANCH;// = false;
bool SELECTABLE_STATE;// = false;
bool HIDE_TITLE;// = false;
bool DISPLAY_SCHEMATICS;// = false;
bool TITLE_ON_TOP;// = false;

QVector<QString> TECHNICAL_LIST;

QString LOAD_PATH;// = "./"
QString WRITE_PATH;// ="./"
QString WRITE_IMAGE_PATH;// ="./"
QString RESTORE_PATH;// = "./"
QString WORKSPACE;// = EXE_PATH
QString CLASS_SEPARATOR;

// Non paramétrable
QDir EXE_PATH("./");

QString VERSION = "V4.2.1";
QString PROGRAM = "BreezeBlocks";
QString BASE_NAME = PROGRAM + " " + VERSION +" - Dessins de chaines";


