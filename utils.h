#ifndef UTILS_H
#define UTILS_H
#include <QColor>
#include <QDir>
#include <QVector>
#include "actionrecentfile.h"

class ColorRule
{
public:
    ColorRule(){}
    ColorRule(QString _classe, QColor _color):color(_color),classe(_classe){}
    QColor color;
    QString classe;
};

class PCLRule
{
public:
    PCLRule(){}
    PCLRule(QString _program, QString _param, QString _subparam, QString _classe, bool _outputFile):program(_program),param(_param),classe(_classe),subparam(_subparam),outputFile(_outputFile){}
    QString program;
    QString param;
    QString subparam;
    QString classe;
    bool outputFile;
};

namespace MyShapes
{
     typedef enum
     {
          Rectangle,
          Ellipse,
          RoundedRectangle,
          Cylinder,
          Diamond
     } Shape;

    typedef enum
    {
         StraightLine,
         CurvedLine,
         BrokenLine
    } LinkPath;
}

#define TECHNICAL_LIST technical_list
extern QVector<QString> TECHNICAL_LIST;

#define EXE_PATH base_path
extern QDir EXE_PATH;

#define WORKSPACE work_space
extern QString WORKSPACE;

#define LOAD_PATH load_path
extern QString LOAD_PATH;

#define WRITE_PATH write_path
extern QString WRITE_PATH;

#define WRITE_IMAGE_PATH write_image_path
extern QString WRITE_IMAGE_PATH;

#define RESTORE_PATH restore_path
extern QString RESTORE_PATH;

#define BASE_NAME base_name
extern QString BASE_NAME;

#define VERSION version
extern QString VERSION;

#define PROGRAM program_name
extern QString PROGRAM;

#define CLASS_SEPARATOR class_separator
extern QString CLASS_SEPARATOR;

#define DISPLAY_STEPNAME display_stepname
extern bool DISPLAY_STEPNAME;

#define DISPLAY_DCLFCLASS display_dclfclass
extern bool DISPLAY_DCLFCLASS;

#define CONDITIONS_DISPLAYED conditions_displayed
extern bool CONDITIONS_DISPLAYED;

#define ANCHORS_DISPLAYED anchors_displayed
extern bool ANCHORS_DISPLAYED;

#define SYSIN_HIDDEN sysin_hidden
extern bool SYSIN_HIDDEN;

#define DISPLAY_GRID display_grid
extern bool DISPLAY_GRID;

#define SELECT_MODE select_mode
extern bool SELECT_MODE;

#define SELECTABLE_STATE selectable_state
extern bool SELECTABLE_STATE;

#define SELECT_BRANCH select_branch
extern bool SELECT_BRANCH;

#define HIDE_TITLE hide_title
extern bool HIDE_TITLE;

#define DISPLAY_SCHEMATICS display_schematics
extern bool DISPLAY_SCHEMATICS;

#define DISPLAY_DDNAME display_ddname
extern bool DISPLAY_DDNAME;

#define TITLE_ON_TOP title_on_top
extern bool TITLE_ON_TOP;

#define TITLE_SIZE title_size
extern int TITLE_SIZE;

#define TITLE_MARGIN title_margin
extern int TITLE_MARGIN;

#define COPY_OFFSET copy_offset
extern int COPY_OFFSET;

#define INSIDE_MARGIN inside_margin
extern int INSIDE_MARGIN;

#define ANCHOR_SIZE anchor_size
extern int ANCHOR_SIZE;

#define SYSIN_SIZE sysin_size
extern int SYSIN_SIZE;

#define LINK_TYPE link_type
extern int LINK_TYPE;

#define DETAILED_WEIGHT detailed_weight
extern int DETAILED_WEIGHT;

#define CONDITION_SIZE condition_size
extern int CONDITION_SIZE;

#define SCENE_MARGIN scene_margin
extern int SCENE_MARGIN;

#define ALIGN_GRID_X align_grid_x
extern int ALIGN_GRID_X;

#define ALIGN_GRID_X_DIVISION align_grid_x_division
extern int ALIGN_GRID_X_DIVISION;

#define ALIGN_GRID_Y_DIVISION align_grid_y_division
extern int ALIGN_GRID_Y_DIVISION;

#define ALIGN_GRID_Y align_grid_y
extern int ALIGN_GRID_Y;

#define SHADOW_OFFSET shadow_offset
extern int SHADOW_OFFSET;

#define FONT_SIZE font_size
extern int FONT_SIZE;

#define DDNAME_FONT_SIZE ddname_font_size
extern int DDNAME_FONT_SIZE;

#define TECHNICAL_FONT_SIZE technical_font_size
extern int TECHNICAL_FONT_SIZE;

#define DEFAULT_WIDTH default_width
extern int DEFAULT_WIDTH;

#define RESIZE_STEP resize_step
extern int RESIZE_STEP;

#define DEFAULT_HEIGHT default_height
extern int DEFAULT_HEIGHT;

#define MAX_RECENT_FILES_SIZE max_recent_files_size
extern int MAX_RECENT_FILES_SIZE;

#define SCALE_CHANGE scale_change
extern double SCALE_CHANGE;

#define TECHNICAL_STEP_COLOR technical_step_color
extern QColor TECHNICAL_STEP_COLOR;

#define NORMAL_STEP_COLOR normal_step_color
extern QColor NORMAL_STEP_COLOR;

#define PATTERN_STEP_COLOR pattern_step_color
extern QColor PATTERN_STEP_COLOR;

#define NORMAL_FILE_COLOR normal_file_color
extern QColor NORMAL_FILE_COLOR;

#define SELECTED_COLOR selected_color
extern QColor SELECTED_COLOR;

#define FILE_HOVER_COLOR file_hover_color
extern QColor FILE_HOVER_COLOR;

#define COLOR_RULES color_rules
extern QVector<ColorRule> COLOR_RULES;

#define PCL_RULES pcl_rules
extern QVector<PCLRule> PCL_RULES;

#define RECENT_FILES recent_files
extern QVector<ActionRecentFile*> RECENT_FILES;

#endif // UTILS_H
