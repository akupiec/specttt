#include "plotwidget.h"
#include "ui_plotwidget.h"
#include "settings.h"

PlotWidget::PlotWidget(Settings *s, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::PlotWidget)
{
    ui->setupUi(this);
    settings = s;
    init();
}

PlotWidget::~PlotWidget()
{
    delete ui;
}

void PlotWidget::setRange(int min, int max, QSlider *slider, QSpinBox *spinBox)
{
    if (min > -2)
    {
        if (slider)
            slider->setMinimum(min);
        if (spinBox)
            spinBox->setMinimum(min);
    }
    if (max > -2)
    {
        if (slider)
            slider->setMaximum(max);
        if (spinBox)
            spinBox->setMaximum(max);
    }
}

void PlotWidget::setValue(int value, QSlider *slider, QSpinBox *spinBox)
{
    if (slider)
        slider->setValue(value);
    if (spinBox)
        spinBox->setValue(value);
}

void PlotWidget::connectSSB(const QSlider *slider, const QSpinBox *spinBox)
{
    connect(slider, SIGNAL(valueChanged(int)), spinBox, SLOT(setValue(int)));
    connect(spinBox, SIGNAL(valueChanged(int)), slider, SLOT(setValue(int)));
}

void PlotWidget::init()
{   // setting up sliders and spin boxes
    setRange(0,10, ui->frameWidthSlider, ui->frameWidthSpinBox);
    setValue(settings->plotFrameWidth(), ui->frameWidthSlider, ui->frameWidthSpinBox);
    setRange(20,400, ui->gridHorizontalSlider, ui->gridHorizontalSpinBox);
    setValue(settings->plotGridHorizontalSpacing(), ui->gridHorizontalSlider, ui->gridHorizontalSpinBox);
    setRange(10,400, ui->gridVerticalSlider, ui->gridVerticalSpinBox);
    setValue(settings->plotGridVerticalSpacing(), ui->gridVerticalSlider, ui->gridVerticalSpinBox);
    setRange(0,20000, ui->imgGeneratorBufferSlider, ui->imgGeneratorBufferSpinBox);
    setValue(settings->plotImageGeneratorBuffer(), ui->imgGeneratorBufferSlider, ui->imgGeneratorBufferSpinBox);
    setRange(1,1000, ui->zoomSlider, ui->zoomSpinBox);
    setValue(100*settings->plotZoomX(), ui->zoomSlider, ui->zoomSpinBox);
    // connecting sliders and spin boxes
    connectSSB(ui->frameWidthSlider, ui->frameWidthSpinBox);
    connectSSB(ui->gridHorizontalSlider, ui->gridHorizontalSpinBox);
    connectSSB(ui->gridVerticalSlider, ui->gridVerticalSpinBox);
    connectSSB(ui->imgGeneratorBufferSlider, ui->imgGeneratorBufferSpinBox);
    connectSSB(ui->zoomSlider, ui->zoomSpinBox);
}

void PlotWidget::saveSettings()
{
    settings->setPlotFrameWidth( ui->frameWidthSlider->value() );
    settings->setPlotGridVerticalSpacing( ui->gridVerticalSlider->value() );
    settings->setPlotGridHorizontalSpacing( ui->gridHorizontalSlider->value() );
    settings->setPlotImageGeneratorBuffer( ui->imgGeneratorBufferSlider->value() );
    settings->setPlotZoomX( ui->zoomSlider->value() / 100.f );
}