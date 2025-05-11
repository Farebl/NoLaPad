#include <QFileDialog>
#include "TrackSettings.h"


TrackSettings::TrackSettings(bool is_loop, ushort volume_percent, ushort beats_per_measure, const QColor& color, QWidget* parent)
    : QDialog(parent),
    m_loop_state(new QCheckBox(this)),
    m_volume_slider(new QSlider(this)),
    m_volume_display (new QSpinBox(this)),
    m_beats_per_measure(new QSpinBox(this)),
    m_color_settings(new QColorDialog(color, this)),
    m_color_preview(new QLabel(this))
{
    setWindowTitle("Track settings");
    setWindowFlags(Qt::Dialog);
    setFixedSize(300, 200);


    m_loop_state->setTristate(false);
    if(is_loop){
        m_loop_state->setCheckState(Qt::Checked);
    }
    else{
        m_loop_state->setCheckState(Qt::Unchecked);
    }

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


    connect(m_volume_slider, &QSlider::valueChanged, this, [&](int value) {
       m_volume_display->setValue(value);
    });

    connect(m_volume_display, &QSpinBox::valueChanged, this, [&](int value) {
        m_volume_slider->setValue(value);
    });

    QHBoxLayout *volumeLayout = new QHBoxLayout();
    volumeLayout->addWidget(m_volume_slider);
    volumeLayout->addWidget(m_volume_display);

    if(beats_per_measure > 32){
        m_beats_per_measure->setValue(32);
    }
    else if(beats_per_measure < 1){
        m_beats_per_measure->setValue(1);
    }
    else{
        m_beats_per_measure->setValue(beats_per_measure);
    }

    m_beats_per_measure->setFixedWidth(50);
    m_beats_per_measure->setRange(1, 32);
    m_beats_per_measure->setButtonSymbols(QAbstractSpinBox::PlusMinus);
    m_beats_per_measure->setAlignment(Qt::AlignCenter);
    m_beats_per_measure->setWrapping(true);



    QPushButton* select_music_file_button = new QPushButton(QString("Select music"), this);
    select_music_file_button->setStyleSheet(QString("background-color: lightgray; border-radius: 2px;"));


    m_color_preview->setFixedSize(50, 20);
    m_color_preview->setStyleSheet(QString("background-color: %1;").arg(color.name()));

    QPushButton* select_color_button = new QPushButton("Choose Color", this);
    select_color_button->setStyleSheet(QString("background-color: lightgray; border-radius: 2px;"));
    connect(select_color_button, &QPushButton::clicked, this, [this]() {
        m_color_settings->setCurrentColor(m_color_settings->currentColor());
        if (m_color_settings->exec() == QDialog::Accepted) {
            QColor selected = m_color_settings->selectedColor();
            m_color_preview->setStyleSheet(QString("background-color: %1;").arg(selected.name()));
            emit changedBackgroundColor(selected);
        }
    });
    QHBoxLayout* colorLayout = new QHBoxLayout;
    colorLayout->addWidget(m_color_preview);
    colorLayout->addWidget(select_color_button);


    QFormLayout *layout_with_settings  = new QFormLayout(this);
    layout_with_settings->addRow("looping", m_loop_state);
    layout_with_settings->addRow("volum", volumeLayout);
    layout_with_settings->addRow("beats count per beat", m_beats_per_measure);
    layout_with_settings->addRow("select audio file", select_music_file_button);
    layout_with_settings->addRow("color of button", colorLayout);

    connect(m_loop_state, &QCheckBox::checkStateChanged, this, &TrackSettings::changedLoopState);
    connect(m_volume_slider, &QSlider::valueChanged, this, &TrackSettings::changedVolume);
    connect(m_beats_per_measure, &QSpinBox::valueChanged, this, &TrackSettings::changedBeatsPerMeasure);

    connect(select_music_file_button, & QPushButton::clicked, [this]() -> void {
        QString soundPath = QFileDialog::getOpenFileName(this, "Choose MP3 file", "..//..//music", "Audio Files (*.mp3 *.wav)");
        if (soundPath.isEmpty()) {
            return;
        }
        emit(changedSoundPath(soundPath));
    });
}

void TrackSettings::closeEvent(QCloseEvent *event) {
    event->ignore();
    this->hide();
}


