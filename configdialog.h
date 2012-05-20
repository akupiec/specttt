#ifndef CONFIGDIALOG_H
#define CONFIGDIALOG_H

#include <QDialog>

namespace Ui {
class ConfigDialog;
}
class ColorsWidget;
class Settings;

class ConfigDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ConfigDialog(Settings *s, QWidget *parent = 0);
    ~ConfigDialog();

public slots:
    void accept();

private:
    Ui::ConfigDialog *ui;
    ColorsWidget *colorsWidget;
};

#endif // CONFIGDIALOG_H
