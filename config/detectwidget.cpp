#include "detectwidget.h"
#include "ui_detectwidget.h"
#include "settings.h"

DetectWidget::DetectWidget(Settings *s, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::DetectWidget)
{
    ui->setupUi(this);
    settings = s;
    init();
}

DetectWidget::~DetectWidget()
{
    delete ui;
}

void DetectWidget::setRange(int min, int max, QSlider *slider, QSpinBox *spinBox)
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

void DetectWidget::setValue(int value, QSlider *slider, QSpinBox *spinBox)
{
    if (slider)
        slider->setValue(value);
    if (spinBox)
        spinBox->setValue(value);
}

void DetectWidget::connectSSB(const QSlider *slider, const QSpinBox *spinBox)
{
    connect(slider, SIGNAL(valueChanged(int)), spinBox, SLOT(setValue(int)));
    connect(spinBox, SIGNAL(valueChanged(int)), slider, SLOT(setValue(int)));
}

void DetectWidget::init()
{   // setting up sliders and spin boxes
    setRange(1,100, ui->thresholdSlider, ui->thresholdSpinBox);
    setValue(100*settings->beepThreshold(), ui->thresholdSlider, ui->thresholdSpinBox);
    setRange(1,2000, ui->periodSlider, ui->periodSpinBox);
    setValue(settings->beepPeriod(), ui->periodSlider, ui->periodSpinBox);
    setRange(0,30, ui->pauseSlider, ui->pauseSpinBox);
    setValue(settings->maxBeepPausePeriods(), ui->pauseSlider, ui->pauseSpinBox);
    // connecting sliders and spin boxes
    connectSSB(ui->thresholdSlider, ui->thresholdSpinBox);
    connectSSB(ui->periodSlider, ui->periodSpinBox);
    connectSSB(ui->pauseSlider, ui->pauseSpinBox);
}

void DetectWidget::saveSettings()
{
    settings->setBeepThreshold( ui->thresholdSpinBox->value() * 0.01f );
    settings->setBeepPeriod( ui->periodSpinBox->value() );
    settings->setMaxBeepPausePeriods( ui->pauseSpinBox->value() );
}
