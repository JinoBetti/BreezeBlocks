#ifndef SEARCHDOCKWIDGET_H
#define SEARCHDOCKWIDGET_H

#include <QDockWidget>
#include <QCommandLinkButton>
#include <QListWidgetItem>
#include "customgraphics.h"

namespace Ui {
class SearchDockWidget;
}

namespace ResultPlace
{
     typedef enum
     {
          Comment,
          Name,
          Precision,
          Class,
          Condition,
          DataCards,
          CallMethod,
          Text,
          Program,
          Link,
          Level
     } ResultPlace;
}

class SearchResult
{
public:
    SearchResult(NodeGraphic* _node, int _place, const QString &_type);
    SearchResult(LinkGraphic* _link, int _place);
    QString getResultString();
    NodeGraphic *getNode() const { return node;}
    void setNode(NodeGraphic *value) { node = value; }

    LinkGraphic *getLink() const { return link;}
    void setLink(LinkGraphic *value) { link = value; }

    QString getCompleteString() const { return completeString; }
    void setCompleteString(const QString &value) { completeString = value; }

private:
    int place;
    QString completeString;
    NodeGraphic* node;
    LinkGraphic* link;
    QString type;
};

class SearchDockWidget : public QDockWidget
{
    Q_OBJECT

public:
    explicit SearchDockWidget(QWidget *parent = 0);
    ~SearchDockWidget();

    void setLabel(const QString &_newLabel);
    void clearResults();

    void searchInNode(const QString &_searched, const QString &_in , NodeGraphic* _node, int _place, const QString &_type, bool _dontskip);
    void searchInLink(const QString &_searched, const QString &_in, LinkGraphic *_link);

    CustomGraphics *getGraphics() const { return graphics; }
    void setGraphics(CustomGraphics *value);

    QSequentialAnimationGroup *hilight(NodeGraphic *_node, int _duration = 200);

public slots:
    void monitorAnimation(QAbstractAnimation::State _new, QAbstractAnimation::State _old);
    void setVisible(bool _visibility);
    void animate(const QModelIndex &_index);
    void search();

private slots:
    void on_lineEdit_textEdited(const QString &){ search();}
    void on_groupBox_toggled(bool _checked);

    void on_pushButton_clicked();

private:
    Ui::SearchDockWidget *ui;
    CustomGraphics* graphics;
    QParallelAnimationGroup* animation;
    MainWindow* main;
    QVector<SearchResult*> results;

};

#endif // SEARCHDOCKWIDGET_H
