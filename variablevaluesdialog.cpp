#include "variablevaluesdialog.h"
#include "ui_variablevaluesdialog.h"


QString InitDialog::getVersion() const { return version; }
void InitDialog::setVersion(const QString &value) { version = value; }

InitDialog::InitDialog(QVector<VariableValue*> _variables, QWidget *parent) :
    QDialog(parent),
    graphics(dynamic_cast<CustomGraphics*>(parent)),
    ui(new Ui::InitDialog)
{
    ui->setupUi(this);

    if(_variables.isEmpty())
    {
        ui->variablesWidget->setVisible(false);
        setFixedHeight(100);
    }
    foreach(VariableValue* variable, _variables)
    {
        createVariableWidget(variable);
    }
}

InitDialog::InitDialog(QList<OrtiResource*> _resources, QWidget *parent) :
    QDialog(parent),
    graphics(dynamic_cast<CustomGraphics*>(parent)),
    ui(new Ui::InitDialog)
{
    ui->setupUi(this);
    ui->variablesWidget->setVisible(false);
    if(_resources.isEmpty())
    {
        ui->variablesWidget->setVisible(false);
        setFixedHeight(100);
    }
    foreach(OrtiResource* resource, _resources)
    {
        createResourceWidget(resource);
    }
}

InitDialog::~InitDialog() { delete ui; }

void InitDialog::on_buttonBox_accepted()
{
    foreach(VariableValueWidget* widget, variableValueWidgets) { widget->getVariable()->setValue(widget->getLineEditValue()); }
    foreach(ResourceWidget* widget, resourceWidgets) { widget->getResource()->setDesactivated(!widget->getCheckValue()); }

    graphics->getTitle()->setVersion(ui->lineEditVersion->text());
}

void InitDialog::createVariableWidget(VariableValue* _variable)
{
    VariableValueWidget* newWidget = new VariableValueWidget(_variable, this);
    ui->scrollAreaWidgetContents->layout()->addWidget(newWidget);
    variableValueWidgets.push_back(newWidget);
}

void InitDialog::createResourceWidget(OrtiResource* _resource)
{
    ResourceWidget* newWidget = new ResourceWidget(_resource, this);
    ui->resourceArea->addWidget(newWidget);
    resourceWidgets.push_back(newWidget);
}

VariableValueWidget::VariableValueWidget(VariableValue* _variable, QWidget *parent):QWidget(parent)
{
    variable = _variable;

    QHBoxLayout* layout = new QHBoxLayout(this);
    QLabel* label =  new QLabel(variable->getName(), this);
    label->setToolTip(variable->getLine());

    lineEdit = new QLineEdit(variable->getName(), this);
    lineEdit->setToolTip(variable->getLine());

     QLabel* labelLine =  new QLabel(variable->getLine(), this);

    layout->addWidget(label);
    layout->addWidget(lineEdit);
    layout->addWidget(labelLine);
    layout->setStretch(0,1);
    layout->setStretch(1,5);
    layout->setStretch(2,5);
}

ResourceWidget::ResourceWidget(OrtiResource* _resource, QWidget *parent):QWidget(parent)
{
    resource = _resource;

    QHBoxLayout* layout = new QHBoxLayout(this);
    QLabel* label =  new QLabel("Conserver la ressource ?", this);

    checkbox = new QCheckBox(resource->getName(), this);
    checkbox->setChecked(true);
    QRegExp crResource("(.{4})-CR.{2}$"); // INST-CRA0
    QRegExp dbResource("(.{4})-DB1"); // INST-DB1

    if(crResource.indexIn(resource->getName())!=-1 || dbResource.indexIn(resource->getName())!=-1) checkbox->setChecked(false);

    layout->addWidget(label);
    layout->addWidget(checkbox);
}
OrtiResource *ResourceWidget::getResource() const
{
    return resource;
}

void ResourceWidget::setResource(OrtiResource *value)
{
    resource = value;
}


void InitDialog::on_InitDialog_rejected()
{
    accept();
}



