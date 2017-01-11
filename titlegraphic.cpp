#include "titlegraphic.h"
#include "modifytitledialog.h"
#include<QLabel>

/// Constructeur

QString TitleGraphic::getVersion() const { return version; }
void TitleGraphic::setVersion(const QString &value) { version = value; }

TitleGraphic::TitleGraphic(const QString &_title, CustomGraphics *_graphics): graphics(_graphics),width(0),height(0)
{
    setTitle(_title);
    if(title.isEmpty()) title =  "Dessin de chaine du PCL " + graphics->getTabName() + " ";
}

/// Change le titre
void TitleGraphic::setTitle(const QString &value)
{
    title = value;
}
QString TitleGraphic::getText() const
{
    return title + (version.isEmpty()? "" : "\n") + version;
}

void TitleGraphic::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    QLabel label(getText());
    QFont font;
    font.setPixelSize(TITLE_SIZE);

    // gérer la largeur d'un titre multiligne en trouvant la largeur de la ligne la plus longue
    QStringList lines = getText().split("\n");
    for(int i=0; i<lines.size(); i++)
    {
        QLabel currentLineLabel(lines[i]);
        currentLineLabel.setFont(font);
        int currentWidth = currentLineLabel.fontMetrics().boundingRect(currentLineLabel.text()).width() + 6;
        if(currentWidth > width) width = currentWidth;
    }
    // et la hauteur
    label.setFont(font);
    height = label.fontMetrics().boundingRect(label.text()).height()*lines.size();

    Q_UNUSED(option);
    Q_UNUSED(widget);

    painter->setRenderHint(QPainter::Antialiasing);

    painter->setFont(font);
    painter->setPen(Qt::black);

    //painter->drawRect(boundingRect());

    QTextOption textOptions(QTextOption(Qt::AlignAbsolute | Qt::AlignHCenter | Qt::AlignVCenter));
    painter->drawText(QRectF(0, 0, width, height),QString("%1").arg(getText()), textOptions);

    setPosition();
}

void TitleGraphic::setPosition()
{
    if(HIDE_TITLE) return;

    QVector<float> boundaries = graphics->getBoundaries();

    float xmin = boundaries[0],
          ymin = boundaries[1],
          W = boundaries[2],
          H = boundaries[3];

    if(TITLE_ON_TOP) setPos(xmin+(W-width)/2, ymin - height - TITLE_MARGIN);

    else setPos(xmin+(W-width)/2, ymin + H + TITLE_MARGIN);
    graphics->resizeScene();
}

void TitleGraphic::mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event)
{
    event->accept();
    if(event->button()==Qt::LeftButton )
    {
        ModifyTitleDialog modifyTitleWindow(this, graphics);
        modifyTitleWindow.exec();
    }
}
