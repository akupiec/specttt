#ifndef SETTINGS_H
#define SETTINGS_H

#include <QSettings>
#include <QVector>
#include <QColor>

class Settings : public QSettings
{
    Q_OBJECT
public:
    Settings(QObject *parent = 0);
    Settings(const QString &organization, const QString &application=QString(), QObject *parent=0);
    ~Settings();
    void setColors();
    void saveColors();
    QVector<QRgb> *colors() { return &colorVector; }

private:
    void init();
    QVector<QRgb> colorVector;

signals:

public slots:

};

#endif // SETTINGS_H
