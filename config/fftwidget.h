#ifndef FFTWIDGET_H
#define FFTWIDGET_H

#include <QWidget>

namespace Ui {
class FFTWidget;
}
class Settings;

class FFTWidget : public QWidget
{
    Q_OBJECT

public:
    explicit FFTWidget(Settings *s, QWidget *parent = 0);
    ~FFTWidget();
    void saveSettings();

private slots:
    void on_bufferSizeSpinBox_valueChanged(int v);

private:
    Ui::FFTWidget *ui;
    Settings *settings;
    int oldBufferSize;
};

#endif // FFTWIDGET_H
