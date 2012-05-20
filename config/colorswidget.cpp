#include "settings.h"
#include "colorswidget.h"
#include "ui_colorswidget.h"

#include <QPainter>
#include <QColorDialog>

static const int spectrumMargin = 15;
static const int spectrumDoubleMargin = 2 * spectrumMargin;

ColorsWidget::ColorsWidget(Settings *s, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ColorsWidget)
{
    settings = s;
    ui->setupUi(this);
    spectrumGradient.setFinalStop(0,0);

    // setup colors vector and buttons
    spectrumColors = new QColor[4];
    spectrumColors[0] = QColor(settings->colors()->at(0));
    setPushButtonColor(ui->nullPushButton, spectrumColors[0]);
    spectrumColors[1] = QColor(settings->colors()->at(1));
    setPushButtonColor(ui->minPushButton, spectrumColors[1]);
    spectrumColors[2] = QColor(settings->colors()->at(settings->colors()->size()-2));
    setPushButtonColor(ui->maxPushButton, spectrumColors[2]);
    spectrumColors[3] = QColor(settings->colors()->last());
    setPushButtonColor(ui->overflowPushButton, spectrumColors[3]);

    connectPushButtonColor(ui->nullPushButton);
    connectPushButtonColor(ui->minPushButton);
    connectPushButtonColor(ui->maxPushButton);
    connectPushButtonColor(ui->overflowPushButton);
}

ColorsWidget::~ColorsWidget()
{
    delete ui;
    delete spectrumColors;
}

void ColorsWidget::resizeEvent(QResizeEvent *)
{
    showSpectrum();
}

void ColorsWidget::showEvent(QShowEvent *)
{
    showSpectrum();
}

void ColorsWidget::showSpectrum()
{
    spectrumGradient.setStart(0, ui->spectrumLabel->height()-spectrumDoubleMargin);
    spectrumGradient.setColorAt(0.0, spectrumColors[1]);
    spectrumGradient.setColorAt(1.0, spectrumColors[2]);
    QPixmap pixmap(ui->spectrumLabel->size());
    QPainter painter(&pixmap);
    painter.fillRect(0, 0, pixmap.width(), spectrumMargin, spectrumColors[3]); // draw overflow color field
    painter.fillRect(0, spectrumMargin, pixmap.width(), pixmap.height()-spectrumDoubleMargin, QBrush(spectrumGradient)); // draw gradient
    painter.fillRect(0, pixmap.height()-spectrumMargin, pixmap.width(), spectrumMargin, spectrumColors[0]); // draw null color field
    ui->spectrumLabel->setPixmap(pixmap);
}

void ColorsWidget::connectPushButtonColor(QPushButton *button)
{
    connect(button, SIGNAL(clicked()), SLOT(setPushButtonColor()));
}

void ColorsWidget::setPushButtonColor(QPushButton *button, const QColor &c)
{
    QColor color = c;
    if (button == 0)
    {
        button = (QPushButton*)sender();
        color = QColor(QColorDialog::getColor(QColor(button->text()),this));
        if (button == ui->nullPushButton)
            spectrumColors[0] = color;
        else if (button == ui->minPushButton)
            spectrumColors[1] = color;
        else if (button == ui->maxPushButton)
            spectrumColors[2] = color;
        else if (button == ui->overflowPushButton)
            spectrumColors[3] = color;
    }
    QPixmap pixmap(16,16);
    pixmap.fill(color);
    button->setIcon(QIcon(pixmap));
    button->setText(color.name());
    showSpectrum();
}

void ColorsWidget::saveColors()
{
    settings->colors()->replace(0, spectrumColors[0].rgb());
    int width = settings->colors()->size() - 2;
    QImage img(width,1,QImage::Format_RGB32);
    QLinearGradient gradient(0,0,width-1,0);
    gradient.setColorAt(0.0, spectrumColors[1]);
    gradient.setColorAt(1.0, spectrumColors[2]);
    QPainter painter(&img);
    painter.fillRect(img.rect(), QBrush(gradient));
    for (int i=0; i<width; )
    {
        QRgb rgb = img.pixel(i,0);
        settings->colors()->replace(++i, rgb);
    }
    settings->colors()->replace(width+1, spectrumColors[3].rgb());
}
