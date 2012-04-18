#include "mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{    
    ui->setupUi(this);

    //config ui
    uiConfig.setupUi(&config);
    connect(uiConfig.buttonBox,SIGNAL(clicked(QAbstractButton*)),this,SLOT(on_config_buttonBox_clicked(QAbstractButton*)));

    // Horizontal ScrollBar config
    ui->horizontalScrollBar->setMinimum(0);
    ui->horizontalScrollBar->setMaximum(0);
    connect(ui->horizontalScrollBar,SIGNAL(valueChanged(int)),ui->plot,SLOT(setImgOffset(int)));
    connect(ui->plot,SIGNAL(ImgOffset(int)),ui->horizontalScrollBar,SLOT(setValue(int)));
    connect(ui->plot,SIGNAL(MaximumOffset(int)),this,SLOT(setScrollBarMaximumValue(int)));

    // ImageGenerator test
    connect(ui->actionBeep_detect, SIGNAL(triggered()), ui->plot, SLOT(detectBeeps()));

    //table filling
    ui->tableWidget->setRowCount(2);
    QTableWidgetItem *item = new QTableWidgetItem("AGFaawddddddddddddddddddddddddddwdawdawdawdawdawdawdadadawdAdw");
    ui->tableWidget->setItem(0,0,item);
    ui->tableWidget->setRowCount(3);

    //splitter default size
    QList<int> size;
    size.append(10);
    size.append(800);
    ui->splitter_2->setSizes(size);

}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_actionImageGenerator_triggered()
{
    filePath = QFileDialog::getOpenFileName(this, tr("Open File"),QDir::homePath(),tr("WAV files")+" (*.wav)");
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

void MainWindow::on_actionPreferences_triggered()
{
    config.exec();
}

void MainWindow::on_actionExit_triggered()
{
    exit(0);
}

void MainWindow::on_config_buttonBox_clicked(QAbstractButton *button)
{
    if(button->text() == tr("OK"))
        qDebug() << "OK" ;
    if(button->text() == tr("Cancel"))
        qDebug()  << "Cancel";
}

void MainWindow::on_buttonRefreshPlot_clicked()
{
    qDebug() << ui->zoomSpinBox->value() << ui->plot->zoom()*100;

    if(static_cast<int>(ui->zoomSpinBox->value()) != static_cast<int>(ui->plot->zoom()*100)) //reopening file and all configurations
    {
        ui->plot->setZoom(ui->zoomSpinBox->value()/100);
        if(filePath != "")
            ui->plot->openFile(filePath);
        qDebug() <<"regenerated";
    }
    else //refreshing all parametrs and images
        ui->plot->refreshPlot();
}
