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
public:
    explicit MetronomeSettings(QWidget *parent = nullptr, quint8 volume_percent = 100);

protected:
    void closeEvent(QCloseEvent *event) override;

private:
    QSlider* m_volume_slider;
    QSpinBox* m_volume_display;

signals:
    void changedVolume(int volum_percent);

};

#endif // METRONOMESETTINGS_H
