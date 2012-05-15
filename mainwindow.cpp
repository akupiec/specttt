#include <QFileDialog>
#include <QDebug>
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "plot.h"
#include "colorsdialog.h"

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
    // ImageGenerator test
    connect(ui->actionBeep_detect, SIGNAL(triggered()), ui->plot->file, SLOT(detectBeeps()));
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_actionImageGenerator_triggered()
{
    QString filePath = QFileDialog::getOpenFileName(this, tr("Open File"),QDir::homePath(),tr("WAV files")+" (*.wav)");
    ui->plot->openFile(filePath);
}

void MainWindow::setScrollBarMaximumValue(int value)
{
    ui->horizontalScrollBar->setMinimum(0);
    ui->horizontalScrollBar->setMaximum(value);
}

void MainWindow::on_actionColors_triggered()
{
    ColorsDialog *dialog = new ColorsDialog(ui->plot->settings,this);
    dialog->show();
}
