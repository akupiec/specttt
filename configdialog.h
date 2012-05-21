#ifndef CONFIGDIALOG_H
#define CONFIGDIALOG_H

#include <QDialog>

namespace Ui {
class ConfigDialog;
}
class ColorsWidget;
class PlotWidget;
class Settings;

class ConfigDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ConfigDialog(Settings *s, QWidget *parent = 0);
    ~ConfigDialog();

public slots:
    void accept();
    void setCurrentWidget(int);

private:
    Ui::ConfigDialog *ui;
    ColorsWidget *colorsWidget;
    PlotWidget *plotWidget;
    QWidgetList widgetList;
};

#endif // CONFIGDIALOG_H
