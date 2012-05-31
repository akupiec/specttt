#ifndef CONFIGDIALOG_H
#define CONFIGDIALOG_H

#include <QDialog>

namespace Ui {
class ConfigDialog;
}
class ColorsWidget;
class PlotWidget;
class FFTWidget;
class DetectWidget;
class Settings;

class ConfigDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ConfigDialog(Settings *s, QWidget *parent = 0);
    ~ConfigDialog();
    static bool reloadFile() { return fileSettingsChanged; }

public slots:
    void accept();
    void setCurrentWidget(int);

private:
    Ui::ConfigDialog *ui;
    ColorsWidget *colorsWidget;
    PlotWidget *plotWidget;
    FFTWidget *fftWidget;
    DetectWidget *detectWidget;
    QWidgetList widgetList;
    Settings *settings;
    static bool fileSettingsChanged;
};

#endif // CONFIGDIALOG_H
