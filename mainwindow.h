#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QDebug>
#include <QFileDialog>
#include "plot.h"
#include "ui_mainwindow.h"
#include "ui_config.h"

namespace Ui {
    class MainWindow;
}
class ImageGenerator;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void tableWidget_update(int); // updating table of markers
    void on_actionImageGenerator_triggered();
    void setScrollBarMaximumValue(int);

    void on_actionSplit_triggered();
    void on_actionExit_triggered();
    void on_actionPreferences_triggered();

    void on_config_buttonBox_clicked(QAbstractButton *);

    void on_buttonRefreshPlot_clicked();

    void on_actionMark_detect_triggered();

    void on_tableWidget_cellChanged(int row, int column);

    void on_tableWidget_itemSelectionChanged();

    void on_textEdit_textChanged();

    void on_buttonMarkerDelete_clicked();

private:
    //ui parametrs
    Ui::MainWindow *ui;
    Ui::Dialog uiConfig;
    QDialog config;

    //file
    QString filePath;

    bool allowEditingCells;
};

#endif // MAINWINDOW_H
