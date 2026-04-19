#ifndef METRONOMESETTINGS_H
#define METRONOMESETTINGS_H

#include <QWidget>

#include <QCloseEvent>
#include <QMessageBox>

#include <QHBoxLayout>
#include <QFormLayout>
#include <QSlider>
#include <QSpinBox>

class MetronomeSettings : public QDialog
{
    Q_OBJECT

private:
    QSlider* m_volume_slider;
    QSpinBox* m_volume_display;

protected:
    void closeEvent(QCloseEvent *event) override;

public:
    MetronomeSettings(quint8 volume_percent = 100, QWidget *parent = nullptr);

signals:
    void changedVolume(int volum_percent);
};

#endif // METRONOMESETTINGS_H
