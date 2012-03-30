#include <QFileDialog>
#include <QDebug>
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "plot.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->horizontalScrollBar->setMinimum(0);
    ui->horizontalScrollBar->setMaximum(0);
    connect(ui->horizontalScrollBar,SIGNAL(valueChanged(int)),ui->plot,SLOT(setImgOffset(int)));
    connect(ui->plot,SIGNAL(ImgOffset(int)),ui->horizontalScrollBar,SLOT(setValue(int)));
    connect(ui->plot,SIGNAL(MaximumOffset(int)),this,SLOT(setScrollBarMaximumValue(int)));
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_actionImageGenerator_triggered()
{
    QString filePath = QFileDialog::getOpenFileName(this, tr("Open File"),QDir::homePath(),tr("WAV files")+" (*.wav)");
    ui->plot->openFile(filePath);
    // ImageGenerator test
    connect(ui->actionBeep_detect, SIGNAL(triggered()), ui->plot, SLOT(detectBeeps()));
}

void MainWindow::setScrollBarMaximumValue(int value)
{
//    qDebug() << value;
    ui->horizontalScrollBar->setMinimum(0);
    ui->horizontalScrollBar->setMaximum(value);
//    qDebug() << "scrollbar:" << ui->horizontalScrollBar->minimum() << ui->horizontalScrollBar->maximum() << ui->horizontalScrollBar->singleStep() << ui->horizontalScrollBar->pageStep();
}
