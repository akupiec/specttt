#ifndef PLOTWIDGET_H
#define PLOTWIDGET_H

#include <QWidget>

namespace Ui {
class PlotWidget;
}
class QSlider;
class QSpinBox;
class Settings;
class QCheckBox;

class PlotWidget : public QWidget
{
    Q_OBJECT

public:
    explicit PlotWidget(Settings *s, QWidget *parent = 0);
    ~PlotWidget();
    void setRange(int min, int max, QSlider *slider, QSpinBox *spinBox);
    void setValue(int value, QSlider *slider, QSpinBox *spinBox);
    void setValue(bool value, QCheckBox *checkBox);
    void saveSettings();

private:
    void init();
    void connectSSB(const QSlider *slider, const QSpinBox *spinBox); // connect Slider and SpinBox
    Ui::PlotWidget *ui;
    Settings *settings;
};

#endif // PLOTWIDGET_H
