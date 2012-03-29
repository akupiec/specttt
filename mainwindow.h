#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QDebug>
#include <QFileDialog>
#include "plot.h"

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
    void on_actionImageGenerator_triggered();
    void setScrollBarMaximumValue(int);

private:
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
