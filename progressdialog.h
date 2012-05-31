#ifndef PROGRESSDIALOG_H
#define PROGRESSDIALOG_H

#include <QProgressDialog>
#include <QMessageBox>

namespace Ui {
class ProgressDialog;
}
class ProgressMessageBox;

class ProgressDialog : public QProgressDialog
{
    Q_OBJECT

public:
    explicit ProgressDialog(const QString &title, QWidget *parent = 0);
    ~ProgressDialog();
    void setETA(int seconds);
    QProgressBar *progressBar();

protected:
    void closeEvent(QCloseEvent *event);

signals:
    void progressAborted();

private:
    Ui::ProgressDialog *ui;
    ProgressMessageBox *messageBox;
    friend class ProgressMessageBox;
};

class ProgressMessageBox : public QMessageBox
{
    Q_OBJECT

public:
    explicit ProgressMessageBox(Icon icon, const QString &title, const QString &text, StandardButtons buttons = Yes | No, QWidget *parent = 0);
    static StandardButton question(ProgressDialog *parent, const QString &title, const QString &text, StandardButtons buttons, StandardButton defaultButton = No);
};

#endif // PROGRESSDIALOG_H
