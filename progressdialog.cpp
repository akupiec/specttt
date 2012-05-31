#include "progressdialog.h"
#include "ui_progressdialog.h"
#include <QCloseEvent>
#include <QDebug>

// ProgressDialog

ProgressDialog::ProgressDialog(const QString &title, QWidget *parent) :
    QProgressDialog(parent),
    ui(new Ui::ProgressDialog)
{
    ui->setupUi(this);
    setWindowTitle(title);
    messageBox = 0;
}

ProgressDialog::~ProgressDialog()
{
    delete ui;
    delete messageBox;
}

void ProgressDialog::setETA(int seconds)
{
    static const QString eta = tr("Estimated time of arrival: ");
    static const QString days = tr(" days");
    static const QString hour = tr(" hours");
    static const QString min = tr(" minuts");
    static const QString sec = tr(" seconds");
    QString str = eta;
    int s = seconds % 60;
    int m = seconds / 60;
    int h = m / 60;
    int d = h / 24;
    h -= d * 24;
    m -= h * 60;
    s -= m * 60;
//    qDebug() << d << h << m << s;
    if (m < 1)
        str += QString::number(s) + sec + '.';
    else if (m < 5)
        str += QString::number(m) + min + ' ' + QString::number(s) + sec + '.';
    else if (h < 1)
        str += QString::number(m) + min + '.';
    else if (h < 3)
        str += QString::number(h) + hour + ' ' + QString::number(m) + min + '.';
    else if (d < 1)
        str += QString::number(h) + hour + '.';
    else
        str += QString::number(d) + days + ' ' + QString::number(h) + hour + '.';
    ui->label->setText(str);
}

QProgressBar * ProgressDialog::progressBar()
{
    return ui->progressBar;
}

void ProgressDialog::closeEvent(QCloseEvent *event)
{
    if (ProgressMessageBox::question(this, tr("Cancel?"), tr("Are you sure you want to cancel and leave progress?"), QMessageBox::Yes | QMessageBox::No) == QMessageBox::Yes)
    {
        emit progressAborted();
        event->accept();
    }
    else
        event->ignore();
}

// ProgressMessageBox

ProgressMessageBox::ProgressMessageBox(Icon icon, const QString &title, const QString &text, StandardButtons buttons, QWidget *parent) :
    QMessageBox(icon,title,text,buttons,parent) {}

QMessageBox::StandardButton ProgressMessageBox::question(ProgressDialog *parent, const QString &title, const QString &text, StandardButtons buttons, StandardButton defaultButton)
{
    parent->messageBox = new ProgressMessageBox(Question,title,text,buttons,parent);
    parent->messageBox->setDefaultButton(defaultButton);
    return static_cast<StandardButton> (parent->messageBox->exec());
}
