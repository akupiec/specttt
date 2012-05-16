#include "mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{    
    ui->setupUi(this);

    //config ui
    uiConfig.setupUi(&config);
    connect(uiConfig.buttonBox,SIGNAL(clicked(QAbstractButton*)),this,SLOT(on_config_buttonBox_clicked(QAbstractButton*)));

    // Table of markers
    ui->tableWidget->setSelectionMode(QAbstractItemView::SingleSelection);

    // Horizontal ScrollBar config
    ui->horizontalScrollBar->setMinimum(0);
    ui->horizontalScrollBar->setMaximum(0);
    connect(ui->horizontalScrollBar,SIGNAL(valueChanged(int)),ui->plot,SLOT(setImgOffset(int)));
    connect(ui->plot,SIGNAL(ImgOffset(int)),ui->horizontalScrollBar,SLOT(setValue(int)));
    connect(ui->plot,SIGNAL(MaximumOffset(int)),this,SLOT(setScrollBarMaximumValue(int)));

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

void MainWindow::on_actionMark_detect_triggered()
{
    if(!ui->plot->isOpened())
        qDebug() << "MainWindow::on_actionMark_detect_triggered() -- read file first";

    //detecting Beeps
    ui->plot->detectBeeps();

    //table filling
    for(int i=0; i<ui->plot->markerList.count();i++)
    {
        ui->tableWidget->setRowCount(i+1);
        QTableWidgetItem *label = new QTableWidgetItem(ui->plot->markerList[i].label());
        ui->tableWidget->setItem(i,0,label);
    }
    ui->tableWidget->update();
    allowEditingCells = true;
}

void MainWindow::on_tableWidget_cellChanged(int row, int column)
{    
    if(allowEditingCells && column == 0)
        ui->plot->markerList[row].setLabel(ui->tableWidget->item(row,column)->text());
}

void MainWindow::on_buttonMarkerAdd_clicked()
{
    for(int i=0; i<ui->plot->markerList.count();i++)
        qDebug() << ui->plot->markerList[i].label() << ui->plot->markerList[i].note();
}

void MainWindow::on_tableWidget_itemSelectionChanged()
{
    //qDebug() << "item selection changed: " << ui->tableWidget->currentRow() << ui->tableWidget->selectedItems().first()->row();
    ui->textEdit->setPlainText(ui->plot->markerList[ui->tableWidget->currentRow()].note());
    ui->plot->selectMarker(ui->tableWidget->currentRow()); // selecting proper marker
}

void MainWindow::on_textEdit_textChanged()
{
    if(ui->tableWidget->currentRow() != -1)
    {
        ui->plot->markerList[ui->tableWidget->currentRow()].setNote(ui->textEdit->toPlainText());
    }
}
