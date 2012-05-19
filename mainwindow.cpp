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

    //config ui
    uiConfig.setupUi(&config);
    connect(uiConfig.buttonBox,SIGNAL(clicked(QAbstractButton*)),this,SLOT(on_config_buttonBox_clicked(QAbstractButton*)));

    // Table of markers
    ui->tableWidget->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->tableWidget->setSelectionBehavior(QAbstractItemView::SelectRows);
    connect(ui->buttonMarkerAdd,SIGNAL(clicked()),ui->plot,SLOT(addMarker()));
    connect(ui->plot,SIGNAL(MarkerListUpdate(int)),this,SLOT(tableWidget_update(int)));

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

void MainWindow::on_actionColors_triggered()
{
    ColorsDialog *dialog = new ColorsDialog(ui->plot->settings,this);
    dialog->show();
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
        ui->tableWidget->setRowCount(i+1); // adding new row
        QTableWidgetItem *item = new QTableWidgetItem(ui->plot->markerList[i].label()); // making new item for table
        ui->tableWidget->setItem(i,0,item); // adding item to table
    }
    ui->tableWidget->update(); // update table
    allowEditingCells = true; // ? it was needed for some reason
}

void MainWindow::on_tableWidget_cellChanged(int row, int column)
{    
    if(allowEditingCells && column == 0)
        ui->plot->markerList[row].setLabel(ui->tableWidget->item(row,column)->text());
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

void MainWindow::tableWidget_update(int index)
{
    ui->tableWidget->clearSelection(); // clearing selection
    if(index != -1 )
    {
        ui->tableWidget->insertRow(index); // inserting new row in index position (end of table)
        QTableWidgetItem *item = new QTableWidgetItem(ui->plot->markerList[index].label()); // new item
        ui->tableWidget->setItem(index,0,item); // adding item
        ui->tableWidget->activateWindow(); // setting table active
        ui->tableWidget->selectRow(index); // selecting new row
    }
    ui->tableWidget->update(); // uptade table
}

void MainWindow::on_buttonMarkerDelete_clicked()
{
    allowEditingCells = true; // ? it was needed for some reason
    int index = ui->tableWidget->currentRow(); // taking index of curent selected row
    ui->tableWidget->clearSelection(); // clearing sellection (nessesery for removing row)
    QTableWidgetItem *item = ui->tableWidget->item(ui->tableWidget->currentRow(),0); // taking pointer of item to delete it
    delete item;
    if (ui->tableWidget->rowCount() > 1) // if ther is more then one maker
        ui->tableWidget->removeRow(index); // delete specyfic row
    else ui->tableWidget->setRowCount(0); // set row number to 0
    ui->plot->delMarker(index); // deleting selected row form Marker List Vector
    ui->plot->selectMarker(ui->tableWidget->currentRow()); // selecting next marker and refreshing plot
}
