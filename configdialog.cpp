#include "configdialog.h"
#include "ui_configdialog.h"
#include "settings.h"
#include "config/colorswidget.h"
#include "config/plotwidget.h"
#include "config/fftwidget.h"
#include "config/detectwidget.h"

bool ConfigDialog::fileSettingsChanged = false;

ConfigDialog::ConfigDialog(Settings *s, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ConfigDialog)
{
    settings = s;
    ui->setupUi(this);
    QList<int> sizes;
    const int listWidgetWidth = 70;
    sizes << listWidgetWidth << this->width() - listWidgetWidth;
    ui->splitter->setSizes(sizes);
    colorsWidget = new ColorsWidget(s,this);
    ui->scrollAreaWidgetContents->layout()->addWidget(colorsWidget);
    plotWidget = new PlotWidget(s,this);
    ui->scrollAreaWidgetContents->layout()->addWidget(plotWidget);
    fftWidget = new FFTWidget(s,this);
    ui->scrollAreaWidgetContents->layout()->addWidget(fftWidget);
    detectWidget = new DetectWidget(s,this);
    ui->scrollAreaWidgetContents->layout()->addWidget(detectWidget);
    widgetList << colorsWidget << plotWidget << fftWidget << detectWidget;
    foreach (QWidget *widget, widgetList)
        widget->hide();
    connect(ui->listWidget, SIGNAL(currentRowChanged(int)), SLOT(setCurrentWidget(int)));
    ui->listWidget->setCurrentRow(0);
}

ConfigDialog::~ConfigDialog()
{
    delete ui;
    foreach (QWidget *widget, widgetList)
        delete widget;
}

void ConfigDialog::accept()
{   // save settings when Ok button clicked
    colorsWidget->saveColors();
    plotWidget->saveSettings();
    quint16 bufferSize = settings->FFT_bufferSize();
    int window = settings->FFT_window();
    int dense = settings->FFT_dense();
    fftWidget->saveSettings();
    fileSettingsChanged = bool(bufferSize != settings->FFT_bufferSize() || window != settings->FFT_window() || dense != settings->FFT_dense());
    detectWidget->saveSettings();
    QDialog::accept();
}

void ConfigDialog::setCurrentWidget(int idx)
{
    static int lastWidgetIndex = (idx < widgetList.length()-1) ? idx+1 : idx-1;
    widgetList.at(lastWidgetIndex)->hide();
    widgetList.at(idx)->show();
    lastWidgetIndex = idx;
}
