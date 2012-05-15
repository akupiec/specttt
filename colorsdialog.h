#ifndef COLORSDIALOG_H
#define COLORSDIALOG_H

#include <QDialog>

namespace Ui {
class ColorsDialog;
}
class Settings;

class ColorsDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ColorsDialog(Settings *settings, QWidget *parent = 0);
    ~ColorsDialog();

protected:
    void resizeEvent(QResizeEvent *);
    void showEvent(QShowEvent *);

private:
    Ui::ColorsDialog *ui;
    Settings *settings;
    QLinearGradient spectrumGradient;
    QColor *spectrumColors;
    void showSpectrum();
    inline void connectPushButtonColor(QPushButton *button);

public slots:
    void setPushButtonColor(QPushButton *button = 0, const QColor &color = QColor());
    void saveColors();
};

#endif // COLORSDIALOG_H
