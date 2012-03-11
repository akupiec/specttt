#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "plot.h"
#include <QFileDialog>
#include <QDebug>
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    Plot p;
    QString filePath = QFileDialog::getOpenFileName(this, tr("Open File"),
                                                    QDir::homePath(),
                                                    tr("WAV files")+" (*.wav)");
    qDebug() << filePath;
    p.openFile(filePath);
}

MainWindow::~MainWindow()
{
    delete ui;
}
