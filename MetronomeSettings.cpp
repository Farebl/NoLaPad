#include "MetronomeSettings.h"

#include <QCloseEvent>
#include <QMessageBox>

#include <QHBoxLayout>
#include <QFormLayout>
#include <QSlider>
#include <QSpinBox>

MetronomeSettings::MetronomeSettings(quint8 volume_percent, QWidget *parent)
    : QDialog(parent),
    m_volume_slider(new QSlider(this)),
    m_volume_display (new QSpinBox(this))
{
    setWindowTitle("Metronome settings");
    setWindowFlags(Qt::Dialog);
    setFixedSize(250, 100);

    m_volume_slider->setOrientation(Qt::Horizontal);
    m_volume_slider->setRange(0, 100);
    m_volume_slider->setSingleStep(1);
    if(volume_percent < 0){
        m_volume_slider->setValue(0);
    }
    else if(volume_percent > 100){
        m_volume_slider->setValue(100);
    }
    else{
        m_volume_slider->setValue(volume_percent);
    }

    m_volume_display->setRange(0, 100);
    m_volume_display->setFixedWidth(40);
    m_volume_display->setValue(volume_percent);


    connect(m_volume_slider, &QSlider::valueChanged, this, [this](int value) {
        m_volume_display->setValue(value);
        emit changedVolume(value);
    });

    connect(m_volume_display, &QSpinBox::valueChanged, this, [this](int value) {
        m_volume_slider->setValue(value);
        emit changedVolume(value);
    });

    QHBoxLayout *volumeLayout = new QHBoxLayout();
    volumeLayout->addWidget(m_volume_slider);
    volumeLayout->addWidget(m_volume_display);

    QFormLayout *layout_with_settings  = new QFormLayout(this);
    layout_with_settings->addRow("volume", volumeLayout);
}

void MetronomeSettings::closeEvent(QCloseEvent *event) {
    event->ignore();
    this->hide();
}

void MetronomeSettings::setVolume(int value){
    m_volume_slider->setValue(value);
}



