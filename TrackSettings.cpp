#include "TrackSettings.h"


TrackSettings::TrackSettings(quint8 volume_percent, bool is_loop, std::vector<bool> beats_per_measure, const QColor& color, QWidget* parent)
    : QDialog(parent),
    m_loop_state(new QCheckBox(this)),
    m_volume_slider(new QSlider(this)),
    m_volume_display (new QSpinBox(this)),
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

    if (beats_per_measure.size() < 8){
        beats_per_measure.resize(8, false);
    }
    QHBoxLayout* beatsLine1Layout = new QHBoxLayout();
    beatsLine1Layout->addWidget((beats_per_measure[0])?new BeatCheckBox(true):new BeatCheckBox(false)); //beat1

    beatsLine1Layout->addWidget((beats_per_measure[1])?new BeatCheckBox(true):new BeatCheckBox(false)); //beat2

    beatsLine1Layout->addWidget((beats_per_measure[2])?new BeatCheckBox(true):new BeatCheckBox(false)); //beat3

    beatsLine1Layout->addWidget((beats_per_measure[3])?new BeatCheckBox(true):new BeatCheckBox(false)); //beat4


    connect(qobject_cast<QCheckBox*>(beatsLine1Layout->itemAt(0)->widget()), &QCheckBox::toggled, this, &TrackSettings::changedBeat1);
    connect(qobject_cast<QCheckBox*>(beatsLine1Layout->itemAt(1)->widget()), &QCheckBox::toggled, this, &TrackSettings::changedBeat2);
    connect(qobject_cast<QCheckBox*>(beatsLine1Layout->itemAt(2)->widget()), &QCheckBox::toggled, this, &TrackSettings::changedBeat3);
    connect(qobject_cast<QCheckBox*>(beatsLine1Layout->itemAt(3)->widget()), &QCheckBox::toggled, this, &TrackSettings::changedBeat4);

    QHBoxLayout* beatsLine2Layout = new QHBoxLayout();
    beatsLine2Layout->addWidget((beats_per_measure[4])?new BeatCheckBox(true):new BeatCheckBox(false)); //beat5
    beatsLine2Layout->addWidget((beats_per_measure[5])?new BeatCheckBox(true):new BeatCheckBox(false)); //beat6
    beatsLine2Layout->addWidget((beats_per_measure[6])?new BeatCheckBox(true):new BeatCheckBox(false)); //beat7
    beatsLine2Layout->addWidget((beats_per_measure[7])?new BeatCheckBox(true):new BeatCheckBox(false)); //beat8

    connect(qobject_cast<QCheckBox*>(beatsLine2Layout->itemAt(0)->widget()), &QCheckBox::toggled, this, &TrackSettings::changedBeat5);
    connect(qobject_cast<QCheckBox*>(beatsLine2Layout->itemAt(1)->widget()), &QCheckBox::toggled, this, &TrackSettings::changedBeat6);
    connect(qobject_cast<QCheckBox*>(beatsLine2Layout->itemAt(2)->widget()), &QCheckBox::toggled, this, &TrackSettings::changedBeat7);
    connect(qobject_cast<QCheckBox*>(beatsLine2Layout->itemAt(3)->widget()), &QCheckBox::toggled, this, &TrackSettings::changedBeat8);

    QVBoxLayout* beatsLayout = new QVBoxLayout();
    beatsLayout->addLayout(beatsLine1Layout);
    beatsLayout->addLayout(beatsLine2Layout);

    QFormLayout *layout_with_settings  = new QFormLayout(this);
    layout_with_settings->addRow("looping", m_loop_state);
    layout_with_settings->addRow("volum", volumeLayout);
    layout_with_settings->addRow("select audio file", select_music_file_button);
    layout_with_settings->addRow("color of button", colorLayout);
    layout_with_settings->addRow("halves of beats ", beatsLayout);

    connect(m_loop_state, &QCheckBox::checkStateChanged, this, &TrackSettings::changedLoopState);
    connect(m_volume_slider, &QSlider::valueChanged, this, &TrackSettings::changedVolume);

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


