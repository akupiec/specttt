#include "configdialog.h"
#include "ui_configdialog.h"
#include "settings.h"
#include "config/colorswidget.h"
#include "config/plotwidget.h"

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
    plotWidget = new PlotWidget(s,this);
    ui->scrollAreaWidgetContents->layout()->addWidget(plotWidget);
    widgetList << colorsWidget << plotWidget;
    foreach (QWidget *widget, widgetList)
        widget->hide();
    connect(ui->listWidget, SIGNAL(currentRowChanged(int)), SLOT(setCurrentWidget(int)));
    ui->listWidget->setCurrentRow(0);
}

ConfigDialog::~ConfigDialog()
{
    delete ui;
    delete colorsWidget;
}

void ConfigDialog::accept()
{   // save settings when Ok button clicked
    colorsWidget->saveColors();
    plotWidget->saveSettings();
    close();
}

void ConfigDialog::setCurrentWidget(int idx)
{
    static int lastWidgetIndex = (idx < widgetList.length()-1) ? idx+1 : idx-1;
    widgetList.at(lastWidgetIndex)->hide();
    widgetList.at(idx)->show();
    lastWidgetIndex = idx;
}
