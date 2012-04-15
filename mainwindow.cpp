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
    // Horizontal ScrollBar config
    ui->horizontalScrollBar->setMinimum(0);
    ui->horizontalScrollBar->setMaximum(0);
    connect(ui->horizontalScrollBar,SIGNAL(valueChanged(int)),ui->plot,SLOT(setImgOffset(int)));
    connect(ui->plot,SIGNAL(ImgOffset(int)),ui->horizontalScrollBar,SLOT(setValue(int)));
    connect(ui->plot,SIGNAL(MaximumOffset(int)),this,SLOT(setScrollBarMaximumValue(int)));
    // ImageGenerator test
    connect(ui->actionBeep_detect, SIGNAL(triggered()), ui->plot, SLOT(detectBeeps()));

    ui->tableWidget->setRowCount(2);
    QTableWidgetItem *item = new QTableWidgetItem("AGFaawddddddddddddddddddddddddddwdawdawdawdawdawdawdadadawdAdw");
    ui->tableWidget->setItem(0,0,item);
    ui->tableWidget->setRowCount(3);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_actionImageGenerator_triggered()
{
    QString filePath = QFileDialog::getOpenFileName(this, tr("Open File"),QDir::homePath(),tr("WAV files")+" (*.wav)");
    if(filePath != "")
        ui->plot->openFile(filePath);
    else
        qDebug() << "Can't open file - should be in message box"; // hange it to message box
}

void MainWindow::setScrollBarMaximumValue(int value)
{
    ui->horizontalScrollBar->setMinimum(0);
    ui->horizontalScrollBar->setMaximum(value);
}

void MainWindow::on_actionSplit_triggered()
{
    ui->plot->splitFile();
}
