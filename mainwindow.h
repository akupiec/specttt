#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QDebug>
#include <QFileDialog>
#include "plot.h"
#include "ui_mainwindow.h"

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

protected:
    void closeEvent(QCloseEvent *);

private slots:
    void on_actionImageGenerator_triggered();
    void setScrollBarMaximumValue(int);

    void on_actionSplit_triggered();
    void on_actionExit_triggered();
    void on_actionPreferences_triggered();

    void on_buttonRefreshPlot_clicked();

    void on_actionMark_detect_triggered();

    void on_tableWidget_cellChanged(int row, int column);
    void tableWidget_update(int index);
    void on_tableWidget_itemSelectionChanged();

    void on_textEdit_textChanged();

    void on_buttonMarkerDelete_clicked();

    void on_actionMark_load_triggered();

private:
    //ui parametrs
    Ui::MainWindow *ui;
    bool allowEditingCells;

    //file
    QString filePath;
};

#endif // MAINWINDOW_H
