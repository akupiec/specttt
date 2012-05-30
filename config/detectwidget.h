#ifndef DETECTWIDGET_H
#define DETECTWIDGET_H

#include <QWidget>

namespace Ui {
class DetectWidget;
}
class QSlider;
class QSpinBox;
class QDoubleSpinBox;
class Settings;

class DetectWidget : public QWidget
{
    Q_OBJECT

public:
    explicit DetectWidget(Settings *s, QWidget *parent = 0);
    ~DetectWidget();
    void setRange(int min, int max, QSlider *slider, QSpinBox *spinBox);
    void setValue(int value, QSlider *slider, QSpinBox *spinBox);
    void saveSettings();

private:
    void init();
    void connectSSB(const QSlider *slider, const QSpinBox *spinBox); // connect Slider and SpinBox
    Ui::DetectWidget *ui;
    Settings *settings;
};

#endif // DETECTWIDGET_H
