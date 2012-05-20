#ifndef COLORSDIALOG_H
#define COLORSDIALOG_H

#include <QWidget>

namespace Ui {
class ColorsWidget;
}
class Settings;
class QPushButton;

class ColorsWidget : public QWidget
{
    Q_OBJECT

public:
    explicit ColorsWidget(Settings *settings, QWidget *parent = 0);
    ~ColorsWidget();

protected:
    void resizeEvent(QResizeEvent *);
    void showEvent(QShowEvent *);

private:
    Ui::ColorsWidget *ui;
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
