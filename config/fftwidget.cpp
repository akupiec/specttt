#include "fftwidget.h"
#include "ui_fftwidget.h"
#include "settings.h"
#include <cmath>

FFTWidget::FFTWidget(Settings *s, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FFTWidget)
{
    ui->setupUi(this);
    settings = s;
    oldBufferSize = s->FFT_bufferSize();
    ui->bufferSizeSpinBox->setValue(oldBufferSize);
    ui->windowComboBox->setCurrentIndex(s->FFT_window());
}

FFTWidget::~FFTWidget()
{
    delete ui;
}

void FFTWidget::on_bufferSizeSpinBox_valueChanged(int v)
{
    int powerFactor = log(oldBufferSize) / log(2); // log2(oldBufferSize)
    if (oldBufferSize > v)
        powerFactor--;
    else if (oldBufferSize < v)
        powerFactor++;
    oldBufferSize = pow(2,powerFactor);
    ui->bufferSizeSpinBox->setValue(oldBufferSize);
}

void FFTWidget::saveSettings()
{
    settings->setFFT_bufferSize( ui->bufferSizeSpinBox->value() );
    settings->setFFT_window( static_cast<FFT::Window> (ui->windowComboBox->currentIndex()) );
}
