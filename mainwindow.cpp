#include <QFileDialog>
#include <QDebug>
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "plot.h"
#include "imagegenerator.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    QString filePath = QFileDialog::getOpenFileName(this, tr("Open File"),QDir::homePath(),tr("WAV files")+" (*.wav)");
    ui->plot->openFile(filePath);
}

MainWindow::~MainWindow()
{
    delete ui;
    delete generator;
}

void MainWindow::on_actionImageGenerator_triggered()
{
    // ImageGenerator test
    generator = new ImageGenerator(ui->plot->file, &ui->plot->tempFile, this);
    connect(generator, SIGNAL(finished()), ui->plot, SLOT(imageGenerated()));
    generator->plotImage(50,100,1.5);
}
