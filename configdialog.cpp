#include "configdialog.h"
#include "ui_configdialog.h"
#include "settings.h"
#include "config/colorswidget.h"

ConfigDialog::ConfigDialog(Settings *s, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ConfigDialog)
{
    ui->setupUi(this);
    QList<int> sizes;
    const int listWidgetWidth = 70;
    sizes << listWidgetWidth << this->width() - listWidgetWidth;
    ui->splitter->setSizes(sizes);
    colorsWidget = new ColorsWidget(s,this);
    ui->scrollAreaWidgetContents->layout()->addWidget(colorsWidget);
}

ConfigDialog::~ConfigDialog()
{
    delete ui;
    delete colorsWidget;
}

void ConfigDialog::accept()
{   // save settings when Ok button clicked
    colorsWidget->saveColors();
    close();
}
