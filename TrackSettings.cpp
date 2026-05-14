#include "TrackSettings.h"
#include "BeatCheckBox.h"
#include "ClickableLabel.h"
#include <QStackedWidget>
#include "LCDCounter.h"
#include <QHeaderView>
#include <QStackedLayout>
#include <QMap>

#include "Potentiometer.h"

#include "Effects.h"


#include <QWidget>
#include <QTableWidget>
#include <QVBoxLayout>
#include <QFormLayout>
#include <QLabel>
#include <QPushButton>
#include <QCloseEvent>
#include <QCheckBox>
#include <QSlider>
#include <QSpinBox>
#include <QColorDialog>
#include <QFileDialog>
#include <QFontDatabase>


#include "LCDDisplay.h"
#include "Fader.h"

#include "TrackColorButtons.h"
#include "RedButton.h"
#include "EffectsSwitcher.h"

#include "AudioSampleSelector.h"



qsizetype EFFECT_POTENTIOMETER_SIZE = 50;


TrackSettings::TrackSettings(quint8 volume_percent, bool is_loop, const std::array<bool, 16>& beats_per_measure, const QColor& outer_background_color, const QColor& inner_background_color, QWidget* parent)
    : QDialog(parent)
    , m_volume_display(new LCDDisplay())
    , m_volume_fader(new Fader(":/images/fader_track.png", ":/images/fader_handle.png", ":/images/fader_measures.png", this))

    , m_select_track_colors_button(new TrackColorButtons(80, Qt::gray, Qt::darkGray, this))
    , m_loop_button(new RedButton(false, 80, 80, this))
    , m_16th_matrix(new QTableWidget(4, 4, this))
    , m_recording_button(new RedButton(false, 80, 40, this))
    , m_whole_tacts_lag_setter(new LCDCounter("whole tacts"))
    , m_whole_tacts_duration_setter(new LCDCounter("whole tacts"))
    , m_effects_switcher(new EffectsSwitcher(140, this))
    , m_audio_input_connector(new AudioSampleSelector(":/images/audio_in_plugged.png", ":/images/audio_in_unplugged.png", "audio in", this))
    , m_beats_per_measure(beats_per_measure)

    , m_effects_settings_stack_widget(new QStackedWidget(this))
    , m_effect_value_display(new LCDDisplay())
    , m_reverb_room_size_potentiometer(new Potentiometer(EFFECT_POTENTIOMETER_SIZE, this))
    , m_reverb_damping_potentiometer(new Potentiometer(EFFECT_POTENTIOMETER_SIZE, this))
    , m_reverb_wet_level_potentiometer(new Potentiometer(EFFECT_POTENTIOMETER_SIZE, this))
    , m_reverb_dry_level_potentiometer(new Potentiometer(EFFECT_POTENTIOMETER_SIZE, this))
    , m_reverb_output_volume_potentiometer(new Potentiometer(EFFECT_POTENTIOMETER_SIZE, this))

    , m_delay_time_potentiometer(new Potentiometer(EFFECT_POTENTIOMETER_SIZE, this))
    , m_delay_feedback_potentiometer(new Potentiometer(EFFECT_POTENTIOMETER_SIZE, this))
    , m_delay_mix_potentiometer(new Potentiometer(EFFECT_POTENTIOMETER_SIZE, this))
    , m_delay_output_volume_potentiometer(new Potentiometer(EFFECT_POTENTIOMETER_SIZE, this))

    , m_chorus_rate_potentiometer(new Potentiometer(EFFECT_POTENTIOMETER_SIZE, this))
    , m_chorus_depth_potentiometer(new Potentiometer(EFFECT_POTENTIOMETER_SIZE, this))
    , m_chorus_center_delay_potentiometer(new Potentiometer(EFFECT_POTENTIOMETER_SIZE, this))
    , m_chorus_feedback_potentiometer(new Potentiometer(EFFECT_POTENTIOMETER_SIZE, this))
    , m_chorus_mix_potentiometer(new Potentiometer(EFFECT_POTENTIOMETER_SIZE, this))
    , m_chorus_output_volume_potentiometer(new Potentiometer(EFFECT_POTENTIOMETER_SIZE, this))

    , m_distortion_drive_potentiometer(new Potentiometer(EFFECT_POTENTIOMETER_SIZE, this))
    , m_distortion_mix_potentiometer(new Potentiometer(EFFECT_POTENTIOMETER_SIZE, this))
    , m_distortion_output_volume_potentiometer(new Potentiometer(EFFECT_POTENTIOMETER_SIZE, this))
{
    setWindowTitle("Track settings");
    setWindowFlags(Qt::Dialog);
    setFixedSize(600, 770);
    setStyleSheet("background-color: #8e8e8e;");
    setWindowModality(Qt::ApplicationModal);




    // -------------------- Volume (label & display & fader) -----------------------------


    // -------------------- Volume label

    QLabel* volume_label = new QLabel("volume");
    volume_label->setStyleSheet("color: #ebebeb");
    volume_label->setAlignment(Qt::AlignCenter);

    // -------------------- Volume fader

    m_volume_fader->setFixedSize(100, 640);
    connect(m_volume_fader, &Fader::valueChanged, this, [this](int value) {
        m_volume_display->setValue(static_cast<uint16_t>(value));
        emit changedVolume(value/100.0f);
    });
    connect(m_volume_display, &LCDDisplay::valueChanged, this, [this](int value) {
        m_volume_fader->setValue(static_cast<uint16_t>(value));
        emit changedVolume(value/100.0f);
    });



    // -------------------- Volume layout (label + display + fader)

    QVBoxLayout* volume_label_display_fader_layout = new QVBoxLayout();
    volume_label_display_fader_layout->setObjectName("volume_label_display_fader_layout");
    volume_label_display_fader_layout->addWidget(volume_label);
    volume_label_display_fader_layout->addWidget(m_volume_display);
    volume_label_display_fader_layout->addWidget(m_volume_fader);
    volume_label_display_fader_layout->setAlignment(Qt::AlignHCenter);
    volume_label_display_fader_layout->setSpacing(5);





    // --------------------  Loop button & 16th beats & Effects Switcher & Emptiness -----------------------------


    // --------------- Loop button

    QLabel* loop_label = new QLabel("loop");
    loop_label->setStyleSheet("color: #ebebeb");
    loop_label->setAlignment(Qt::AlignCenter);

    QVBoxLayout* loop_layout = new QVBoxLayout();
    loop_layout->setObjectName("loop_layout");
    loop_layout->addWidget(loop_label);
    loop_layout->addWidget(m_loop_button);
    loop_layout->setAlignment(Qt::AlignCenter);
    loop_layout->setSpacing(5);




    // -------------- 16th beats

    // ------ label

    QLabel* beats_label = new QLabel("1/16ths of tact");
    beats_label->setStyleSheet("color: #ebebeb");
    beats_label->setAlignment(Qt::AlignCenter);



    // ------ matrix

    m_16th_matrix->setShowGrid(false);
    m_16th_matrix->setFrameShape(QFrame::NoFrame);
    m_16th_matrix->horizontalHeader()->setVisible(false);
    m_16th_matrix->verticalHeader()->setVisible(false);
    m_16th_matrix->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_16th_matrix->setContentsMargins(0,0,0,0);
    m_16th_matrix->setFixedSize(200, 200);

    m_16th_matrix->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    m_16th_matrix->verticalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    QString label_16th_style("color: #ebebeb; font-size: 14px; text-align: center;");
    QVBoxLayout* current_16th_beat_layout = nullptr;
    QLabel* current_label_16th = nullptr;
    BeatCheckBox* current_16th_checkBox = nullptr;
    quint8 current_index_of_16th = 0;
    QWidget* container_for_16th_layout;
    for(quint8 i = 0; i < 4; ++i){
        for(quint8 j = 0; j < 4; ++j, ++current_index_of_16th){

            container_for_16th_layout = new QWidget();
            container_for_16th_layout->setContentsMargins(0,0,0,0);

            QVBoxLayout* current_16th_beat_layout = new QVBoxLayout(container_for_16th_layout);
            current_16th_beat_layout->setSpacing(0);
            current_16th_beat_layout->setContentsMargins(0,0,0,0);
            current_16th_beat_layout->setAlignment(Qt::AlignHCenter);
            current_16th_beat_layout->setObjectName(QString::number(current_index_of_16th));

            current_label_16th = new QLabel(QString::number(current_index_of_16th));
            current_label_16th->setContentsMargins(0,0,0,0);
            current_label_16th->setStyleSheet(label_16th_style);
            current_16th_beat_layout->addWidget(current_label_16th);

            current_16th_checkBox = new BeatCheckBox((m_beats_per_measure[current_index_of_16th]) ? true : false);
            current_16th_checkBox->setObjectName(QString::number(current_index_of_16th));
            current_16th_beat_layout->addWidget(current_16th_checkBox);

            m_16th_matrix->setCellWidget(i, j, container_for_16th_layout);
            connect(current_16th_checkBox, &QCheckBox::toggled, this, &TrackSettings::changedBeat);
        }
    }



    QVBoxLayout* beats_layout = new QVBoxLayout();
    beats_layout->setObjectName("beats_layout");
    beats_layout->setSpacing(10);
    beats_layout->setContentsMargins(0, 0, 0, 0);
    beats_layout->setAlignment(Qt::AlignHCenter);
    beats_layout->addWidget(beats_label);
    beats_layout->addWidget(m_16th_matrix);





    // ----------  Effects Switcher

    m_effects_switcher->setValue(0);

    ClickableLabel* effect_off_label = new ClickableLabel("0 off", this);
    ClickableLabel* effect_reverb_label = new ClickableLabel("1 Reverb", this);
    ClickableLabel* effect_delay_label = new ClickableLabel("2 Delay", this);
    ClickableLabel* effect_chorus_label = new ClickableLabel("3 Chorus", this);
    ClickableLabel* effect_distortion_label = new ClickableLabel("4 Distortion", this);

    effect_off_label->setStyleSheet("color: #ebebeb");
    effect_reverb_label->setStyleSheet("color: #ebebeb");
    effect_delay_label->setStyleSheet("color: #ebebeb");
    effect_chorus_label->setStyleSheet("color: #ebebeb");
    effect_distortion_label->setStyleSheet("color: #ebebeb");


    QWidget::connect(m_effects_switcher, &EffectsSwitcher::settedZeroPos, this, [=](){
        effect_off_label->setStyleSheet("color: #ff0000");
        effect_reverb_label->setStyleSheet("color: #ebebeb");
        effect_delay_label->setStyleSheet("color: #ebebeb");
        effect_chorus_label->setStyleSheet("color: #ebebeb");
        effect_distortion_label->setStyleSheet("color: #ebebeb");
        emit changedEffectType(EffectType::None);
        m_effects_settings_stack_widget->setCurrentIndex(static_cast<int>(EffectType::None));
    });

    QWidget::connect(m_effects_switcher, &EffectsSwitcher::settedFirstPos, this, [=](){
        effect_off_label->setStyleSheet("color: #ebebeb");
        effect_reverb_label->setStyleSheet("color: #ffef40");
        effect_delay_label->setStyleSheet("color: #ebebeb");
        effect_chorus_label->setStyleSheet("color: #ebebeb");
        effect_distortion_label->setStyleSheet("color: #ebebeb");
        emit changedEffectType(EffectType::Reverb);
        m_effects_settings_stack_widget->setCurrentIndex(static_cast<int>(EffectType::Reverb));
    });

    QWidget::connect(m_effects_switcher, &EffectsSwitcher::settedSecondPos, this, [=](){
        effect_off_label->setStyleSheet("color: #ebebeb");
        effect_reverb_label->setStyleSheet("color: #ebebeb");
        effect_delay_label->setStyleSheet("color: #ffef40");
        effect_chorus_label->setStyleSheet("color: #ebebeb");
        effect_distortion_label->setStyleSheet("color: #ebebeb");
        emit changedEffectType(EffectType::Delay);
        m_effects_settings_stack_widget->setCurrentIndex(static_cast<int>(EffectType::Delay));
    });

    QWidget::connect(m_effects_switcher, &EffectsSwitcher::settedThirdPos, this, [=](){
        effect_off_label->setStyleSheet("color: #ebebeb");
        effect_reverb_label->setStyleSheet("color: #ebebeb");
        effect_delay_label->setStyleSheet("color: #ebebeb");
        effect_chorus_label->setStyleSheet("color: #ffef40");
        effect_distortion_label->setStyleSheet("color: #ebebeb");
        emit changedEffectType(EffectType::Chorus);
        m_effects_settings_stack_widget->setCurrentIndex(static_cast<int>(EffectType::Chorus));
    });

    QWidget::connect(m_effects_switcher, &EffectsSwitcher::settedFourthPos, this, [=](){
        effect_off_label->setStyleSheet("color: #ebebeb");
        effect_reverb_label->setStyleSheet("color: #ebebeb");
        effect_delay_label->setStyleSheet("color: #ebebeb");
        effect_chorus_label->setStyleSheet("color: #ebebeb");
        effect_distortion_label->setStyleSheet("color: #ffef40");
        emit changedEffectType(EffectType::Distortion);
        m_effects_settings_stack_widget->setCurrentIndex(static_cast<int>(EffectType::Distortion));
    });


    QWidget::connect(effect_off_label, &ClickableLabel::clicked, this, [this](){
        m_effects_switcher->setZeroPos();
    });
    QWidget::connect(effect_reverb_label, &ClickableLabel::clicked, this, [this](){
        m_effects_switcher->setFirstPos();
    });
    QWidget::connect(effect_delay_label, &ClickableLabel::clicked, this, [this](){
        m_effects_switcher->setSecondPos();
    });
    QWidget::connect(effect_chorus_label, &ClickableLabel::clicked, this, [this](){
        m_effects_switcher->setThirdPos();
    });
    QWidget::connect(effect_distortion_label, &ClickableLabel::clicked, this, [this](){
        m_effects_switcher->setFourthPos();
    });


    QVBoxLayout* effects_labels_layout = new QVBoxLayout();
    effects_labels_layout->setObjectName("effects_labels_layout");
    effects_labels_layout->setAlignment(Qt::AlignCenter);
    effects_labels_layout->setSpacing(10);
    effects_labels_layout->addWidget(effect_off_label);
    effects_labels_layout->addWidget(effect_reverb_label);
    effects_labels_layout->addWidget(effect_delay_label);
    effects_labels_layout->addWidget(effect_chorus_label);
    effects_labels_layout->addWidget(effect_distortion_label);




    QVBoxLayout* effects_switcher_and_labels_layout = new QVBoxLayout();
    effects_switcher_and_labels_layout->setObjectName("effects_switcher_and_labels_layout");
    effects_switcher_and_labels_layout->addWidget(m_effects_switcher);
    effects_switcher_and_labels_layout->addLayout(effects_labels_layout);
    effects_switcher_and_labels_layout->setAlignment(Qt::AlignCenter);
    effects_switcher_and_labels_layout->setSpacing(20);






    // -------------------- (Loop_button + 16th_beats) layout

    QVBoxLayout* loopButton_16thBeats_effectSwitcher_emptyness_layout = new QVBoxLayout();
    loopButton_16thBeats_effectSwitcher_emptyness_layout->setObjectName("loopButton_16thBeats_effectSwitcher_emptyness_layout");
    loopButton_16thBeats_effectSwitcher_emptyness_layout->setAlignment(Qt::AlignHCenter);
    loopButton_16thBeats_effectSwitcher_emptyness_layout->setSpacing(10);
    loopButton_16thBeats_effectSwitcher_emptyness_layout->addLayout(loop_layout);
    loopButton_16thBeats_effectSwitcher_emptyness_layout->addLayout(beats_layout);
    loopButton_16thBeats_effectSwitcher_emptyness_layout->addLayout(effects_switcher_and_labels_layout);
    loopButton_16thBeats_effectSwitcher_emptyness_layout->addSpacing(50);





    // --------------------  Timers & Track colors button & REC button & effects settings & input connector -------------------------------------------------



    // -------------------- Timers
    int setter_height = 100;
    m_whole_tacts_lag_setter->setFixedHeight(setter_height);
    m_whole_tacts_duration_setter->setFixedHeight(setter_height);

    QHBoxLayout* lag_and_duration_setters_layout = new QHBoxLayout();
    lag_and_duration_setters_layout->addWidget(m_whole_tacts_lag_setter);
    lag_and_duration_setters_layout->addWidget(m_whole_tacts_duration_setter);


    connect(m_whole_tacts_lag_setter, &LCDCounter::changedValue, this, &TrackSettings::changedWholeTacktLag);
    connect(m_whole_tacts_duration_setter, &LCDCounter::changedValue, this, &TrackSettings::changedWholeTacktDuration);




    // ------------------------ Track color & REC

    // ------------ Track color

    // ---- Track color label

    QLabel* select_track_color_label = new QLabel("track colors");
    select_track_color_label->setStyleSheet("color: #ebebeb");
    select_track_color_label->setAlignment(Qt::AlignCenter);

    // ---- Track color button
    connect(m_select_track_colors_button, &TrackColorButtons::changedInnerColor, this, &TrackSettings::changedInnerActiveBackgroundColor);
    connect(m_select_track_colors_button, &TrackColorButtons::changedOuterColor, this, &TrackSettings::changedOuterBackgroundColor);

    QVBoxLayout* track_colors_layout = new QVBoxLayout();
    track_colors_layout->setObjectName("track_colors_layout");
    track_colors_layout->addWidget(select_track_color_label);
    track_colors_layout->addWidget(m_select_track_colors_button);
    track_colors_layout->setAlignment(Qt::AlignCenter);
    track_colors_layout->setSpacing(5);

    connect(m_loop_button, &RedButton::changedState, this, &TrackSettings::changedLoopState);


    // ----------- REC button

    QLabel* rec_button_label = new QLabel("Recording");
    rec_button_label->setStyleSheet("color: #ebebeb");
    rec_button_label->setAlignment(Qt::AlignCenter);

    QVBoxLayout* rec_button_and_label_layout = new QVBoxLayout();
    rec_button_and_label_layout->addWidget(rec_button_label);
    rec_button_and_label_layout->addWidget(m_recording_button);

    connect(m_recording_button, &RedButton::changedState, this, &TrackSettings::changedRecordingEnabled);



    // ----------- (Track color + REC button)  layout

    QHBoxLayout* track_colors_and_rec_button_layout = new QHBoxLayout();
    track_colors_and_rec_button_layout->addLayout(track_colors_layout);
    track_colors_and_rec_button_layout->addSpacing(20);
    track_colors_and_rec_button_layout->addLayout(rec_button_and_label_layout);



    // ---------------------- effects settings


    QLabel* effect_settings_label = new QLabel("Effect settings");
    effect_settings_label->setStyleSheet("color: #ebebeb");
    effect_settings_label->setAlignment(Qt::AlignCenter);

    // --------- LCD display
    m_effect_value_display->setRange(0, 100);
    m_effect_value_display->setFixedSize(140, 80);

    QHBoxLayout* effect_value_display_layout = new QHBoxLayout();
    effect_value_display_layout->addWidget(m_effect_value_display, Qt::AlignHCenter);


    // --------- potentiometers

    int spacing_between_potentiometers = 25;


    // -------- Off settings
    QWidget* off_settings_widget = new QWidget(this);


    // -------- Reverb settings

    m_reverb_room_size_potentiometer->setMaximum(100);
    m_reverb_damping_potentiometer->setMaximum(100);
    m_reverb_wet_level_potentiometer->setMaximum(100);
    m_reverb_dry_level_potentiometer->setMaximum(100);
    m_reverb_output_volume_potentiometer->setMaximum(100);


    // Reverb labels
    QLabel* reverb_room_size_label = new QLabel("room", this);
    reverb_room_size_label->setAlignment(Qt::AlignCenter);

    QLabel* reverb_damping_label = new QLabel("damping", this);
    reverb_damping_label->setAlignment(Qt::AlignCenter);

    QLabel* reverb_wet_level_label = new QLabel("wet", this);
    reverb_wet_level_label->setAlignment(Qt::AlignCenter);

    QLabel* reverb_dry_level_label = new QLabel("dry", this);
    reverb_dry_level_label->setAlignment(Qt::AlignCenter);

    QLabel* reverb_output_volume_label = new QLabel("volume", this);
    reverb_output_volume_label->setAlignment(Qt::AlignCenter);


    // Reverb potentiometers + labels
    QVBoxLayout* reverb_room_size_layout = new QVBoxLayout();
    reverb_room_size_layout->addWidget(m_reverb_room_size_potentiometer);
    reverb_room_size_layout->addWidget(reverb_room_size_label);
    reverb_room_size_layout->setAlignment(Qt::AlignHCenter);

    QVBoxLayout* reverb_damping_layout = new QVBoxLayout();
    reverb_damping_layout->addWidget(m_reverb_damping_potentiometer);
    reverb_damping_layout->addWidget(reverb_damping_label);
    reverb_damping_layout->setAlignment(Qt::AlignHCenter);

    QVBoxLayout* reverb_wet_level_layout = new QVBoxLayout();
    reverb_wet_level_layout->addWidget(m_reverb_wet_level_potentiometer);
    reverb_wet_level_layout->addWidget(reverb_wet_level_label );
    reverb_wet_level_layout->setAlignment(Qt::AlignHCenter);

    QVBoxLayout* reverb_dry_level_layout = new QVBoxLayout();
    reverb_dry_level_layout->addWidget(m_reverb_dry_level_potentiometer);
    reverb_dry_level_layout->addWidget(reverb_dry_level_label );
    reverb_dry_level_layout->setAlignment(Qt::AlignHCenter);

    QVBoxLayout* reverb_output_volume_layout = new QVBoxLayout();
    reverb_output_volume_layout->addWidget(m_reverb_output_volume_potentiometer);
    reverb_output_volume_layout->addWidget(reverb_output_volume_label);
    reverb_output_volume_layout->setAlignment(Qt::AlignHCenter);


    QHBoxLayout* reverb_volume_and_room_size_and_damping_potentiometers_layout = new QHBoxLayout();
    reverb_volume_and_room_size_and_damping_potentiometers_layout->addStretch();
    reverb_volume_and_room_size_and_damping_potentiometers_layout->addLayout(reverb_output_volume_layout);
    reverb_volume_and_room_size_and_damping_potentiometers_layout->addSpacing(spacing_between_potentiometers);
    reverb_volume_and_room_size_and_damping_potentiometers_layout->addLayout(reverb_room_size_layout);
    reverb_volume_and_room_size_and_damping_potentiometers_layout->addSpacing(spacing_between_potentiometers);
    reverb_volume_and_room_size_and_damping_potentiometers_layout->addLayout(reverb_damping_layout);
    reverb_volume_and_room_size_and_damping_potentiometers_layout->addStretch();

    QHBoxLayout* reverb_wet_and_dry_level_potentiometers_layout = new QHBoxLayout();
    reverb_wet_and_dry_level_potentiometers_layout->addStretch();
    reverb_wet_and_dry_level_potentiometers_layout->addLayout(reverb_wet_level_layout);
    reverb_wet_and_dry_level_potentiometers_layout->addSpacing(spacing_between_potentiometers);
    reverb_wet_and_dry_level_potentiometers_layout->addLayout(reverb_dry_level_layout);
    reverb_wet_and_dry_level_potentiometers_layout->addStretch();

    QWidget* reverb_settings_widget = new QWidget(this);
    QVBoxLayout* reverb_settings_layoput = new QVBoxLayout(reverb_settings_widget);
    reverb_settings_layoput->addLayout(reverb_volume_and_room_size_and_damping_potentiometers_layout);
    reverb_settings_layoput->addLayout(reverb_wet_and_dry_level_potentiometers_layout);





    // -------- Delay settings

    m_delay_time_potentiometer->setMaximum(100);
    m_delay_feedback_potentiometer->setMaximum(100);
    m_delay_mix_potentiometer->setMaximum(100);
    m_delay_output_volume_potentiometer->setMaximum(100);

    //  Delay labels

    QLabel* delay_time_label = new QLabel("delay time", this);
    delay_time_label->setAlignment(Qt::AlignCenter);

    QLabel* delay_feedback_label = new QLabel("feedback", this);
    delay_feedback_label->setAlignment(Qt::AlignCenter);

    QLabel* delay_mix_label = new QLabel("mix", this);
    delay_mix_label->setAlignment(Qt::AlignCenter);

    QLabel* delay_output_volume_label = new QLabel("volume", this);
    delay_output_volume_label->setAlignment(Qt::AlignCenter);


    // Delay potentiometers + labels
    QVBoxLayout* delay_time_layout = new QVBoxLayout();
    delay_time_layout->addWidget(m_delay_time_potentiometer);
    delay_time_layout->addWidget(delay_time_label);
    delay_time_layout->setAlignment(Qt::AlignHCenter);

    QVBoxLayout* delay_feedback_layout = new QVBoxLayout();
    delay_feedback_layout->addWidget(m_delay_feedback_potentiometer);
    delay_feedback_layout->addWidget(delay_feedback_label);
    delay_feedback_layout->setAlignment(Qt::AlignHCenter);

    QVBoxLayout* delay_mix_layout = new QVBoxLayout();
    delay_mix_layout->addWidget(m_delay_mix_potentiometer);
    delay_mix_layout->addWidget(delay_mix_label);
    delay_mix_layout->setAlignment(Qt::AlignHCenter);

    QVBoxLayout* delay_output_volume_layout = new QVBoxLayout();
    delay_output_volume_layout->addWidget(m_delay_output_volume_potentiometer);
    delay_output_volume_layout->addWidget(delay_output_volume_label);
    delay_output_volume_layout->setAlignment(Qt::AlignHCenter);



    QHBoxLayout* delay_volume_and_mix_potentiometers_layout = new QHBoxLayout();
    delay_volume_and_mix_potentiometers_layout->addStretch(1);
    delay_volume_and_mix_potentiometers_layout->addLayout(delay_output_volume_layout);
    delay_volume_and_mix_potentiometers_layout->addStretch(2);
    delay_volume_and_mix_potentiometers_layout->addLayout(delay_mix_layout);
    delay_volume_and_mix_potentiometers_layout->addStretch(1);

    QHBoxLayout* delay_feedback_and_delay_time_potentiometers_layout = new QHBoxLayout();
    delay_feedback_and_delay_time_potentiometers_layout->addStretch();
    delay_feedback_and_delay_time_potentiometers_layout->addLayout(delay_feedback_layout);
    delay_feedback_and_delay_time_potentiometers_layout->addSpacing(spacing_between_potentiometers);
    delay_feedback_and_delay_time_potentiometers_layout->addLayout(delay_time_layout);
    delay_feedback_and_delay_time_potentiometers_layout->addStretch();


    QWidget* delay_settings_widget = new QWidget(this);
    QVBoxLayout* delay_settings_layoput = new QVBoxLayout(delay_settings_widget);
    delay_settings_layoput->addLayout(delay_volume_and_mix_potentiometers_layout);
    delay_settings_layoput->addLayout(delay_feedback_and_delay_time_potentiometers_layout);



    // -------- Chorus settings

    m_chorus_rate_potentiometer->setMaximum(100);
    m_chorus_depth_potentiometer->setMaximum(100);
    m_chorus_center_delay_potentiometer->setMaximum(100);
    m_chorus_feedback_potentiometer->setMaximum(100);
    m_chorus_mix_potentiometer->setMaximum(100);
    m_chorus_output_volume_potentiometer->setMaximum(100);


    //  Chorus labels

    QLabel* chorus_rate_label = new QLabel("rate", this);
    chorus_rate_label->setAlignment(Qt::AlignCenter);

    QLabel* chorus_depth_label = new QLabel("depth", this);
    chorus_depth_label->setAlignment(Qt::AlignCenter);

    QLabel* chorus_center_delay_label = new QLabel("center delay", this);
    chorus_center_delay_label->setAlignment(Qt::AlignCenter);

    QLabel* chorus_feedback_label = new QLabel("feedback", this);
    chorus_feedback_label->setAlignment(Qt::AlignCenter);

    QLabel* chorus_mix_label = new QLabel("mix", this);
    chorus_mix_label->setAlignment(Qt::AlignCenter);

    QLabel* chorus_output_volume_label = new QLabel("volume", this);
    chorus_output_volume_label->setAlignment(Qt::AlignCenter);


    // Chorus potentiometers + labels

    QVBoxLayout* chorus_rate_layout = new QVBoxLayout();
    chorus_rate_layout->addWidget(m_chorus_rate_potentiometer);
    chorus_rate_layout->addWidget(chorus_rate_label);
    chorus_rate_layout->setAlignment(Qt::AlignHCenter);

    QVBoxLayout* chorus_depth_layout = new QVBoxLayout();
    chorus_depth_layout->addWidget(m_chorus_depth_potentiometer);
    chorus_depth_layout->addWidget(chorus_depth_label);
    chorus_depth_layout->setAlignment(Qt::AlignHCenter);

    QVBoxLayout* chorus_center_delay_layout = new QVBoxLayout();
    chorus_center_delay_layout->addWidget(m_chorus_center_delay_potentiometer);
    chorus_center_delay_layout->addWidget(chorus_center_delay_label);
    chorus_center_delay_layout->setAlignment(Qt::AlignHCenter);

    QVBoxLayout* chorus_feedback_layout = new QVBoxLayout();
    chorus_feedback_layout->addWidget(m_chorus_feedback_potentiometer);
    chorus_feedback_layout->addWidget(chorus_feedback_label);
    chorus_feedback_layout->setAlignment(Qt::AlignHCenter);

    QVBoxLayout* chorus_mix_layout = new QVBoxLayout();
    chorus_mix_layout->addWidget(m_chorus_mix_potentiometer);
    chorus_mix_layout->addWidget(chorus_mix_label);
    chorus_mix_layout->setAlignment(Qt::AlignHCenter);

    QVBoxLayout* chorus_output_volume_layout = new QVBoxLayout();
    chorus_output_volume_layout->addWidget(m_chorus_output_volume_potentiometer);
    chorus_output_volume_layout->addWidget(chorus_output_volume_label);
    chorus_output_volume_layout->setAlignment(Qt::AlignHCenter);


    QHBoxLayout* chorus_volume_and_rate_and_feedback_potentiometers_layout = new QHBoxLayout();
    chorus_volume_and_rate_and_feedback_potentiometers_layout->addStretch();
    chorus_volume_and_rate_and_feedback_potentiometers_layout->addLayout(chorus_output_volume_layout);
    chorus_volume_and_rate_and_feedback_potentiometers_layout->addSpacing(spacing_between_potentiometers);
    chorus_volume_and_rate_and_feedback_potentiometers_layout->addLayout(chorus_rate_layout);
    chorus_volume_and_rate_and_feedback_potentiometers_layout->addSpacing(spacing_between_potentiometers);
    chorus_volume_and_rate_and_feedback_potentiometers_layout->addLayout(chorus_feedback_layout);
    chorus_volume_and_rate_and_feedback_potentiometers_layout->addStretch();

    QHBoxLayout* chorus_depth_and_center_delay_and_mix_potentiometers_layout = new QHBoxLayout();
    chorus_depth_and_center_delay_and_mix_potentiometers_layout->addStretch();
    chorus_depth_and_center_delay_and_mix_potentiometers_layout->addLayout(chorus_depth_layout);
    chorus_depth_and_center_delay_and_mix_potentiometers_layout->addSpacing(spacing_between_potentiometers);
    chorus_depth_and_center_delay_and_mix_potentiometers_layout->addLayout(chorus_center_delay_layout);
    chorus_depth_and_center_delay_and_mix_potentiometers_layout->addSpacing(spacing_between_potentiometers);
    chorus_depth_and_center_delay_and_mix_potentiometers_layout->addLayout(chorus_mix_layout);
    chorus_depth_and_center_delay_and_mix_potentiometers_layout->addStretch();

    QWidget* chorus_settings_widget = new QWidget(this);
    QVBoxLayout* chorus_settings_layoput = new QVBoxLayout(chorus_settings_widget);
    chorus_settings_layoput->addLayout(chorus_volume_and_rate_and_feedback_potentiometers_layout);
    chorus_settings_layoput->addLayout(chorus_depth_and_center_delay_and_mix_potentiometers_layout);



    // -------- Distortion settings

    m_distortion_drive_potentiometer->setMaximum(100);
    m_distortion_mix_potentiometer->setMaximum(100);
    m_distortion_output_volume_potentiometer->setMaximum(100);


    //  Chorus labels

    QLabel* distortion_drive_label = new QLabel("drive", this);
    distortion_drive_label->setAlignment(Qt::AlignCenter);

    QLabel* distortion_mix_label = new QLabel("mix", this);
    distortion_mix_label->setAlignment(Qt::AlignCenter);

    QLabel* distortion_output_volume_label = new QLabel("volume", this);
    distortion_output_volume_label->setAlignment(Qt::AlignCenter);


    // Chorus potentiometers + labels

    QVBoxLayout* distortion_drive_layout = new QVBoxLayout();
    distortion_drive_layout->addWidget(m_distortion_drive_potentiometer);
    distortion_drive_layout->addWidget(distortion_drive_label);
    distortion_drive_layout->setAlignment(Qt::AlignHCenter);

    QVBoxLayout* distortion_mix_layout = new QVBoxLayout();
    distortion_mix_layout->addWidget(m_distortion_mix_potentiometer);
    distortion_mix_layout->addWidget(distortion_mix_label);
    distortion_mix_layout->setAlignment(Qt::AlignHCenter);

    QVBoxLayout* distortion_output_volume_layout = new QVBoxLayout();
    distortion_output_volume_layout->addWidget(m_distortion_output_volume_potentiometer);
    distortion_output_volume_layout->addWidget(distortion_output_volume_label);
    distortion_output_volume_layout->setAlignment(Qt::AlignHCenter);


    QHBoxLayout* distortion_volume_and_mix_potentiometers_layout = new QHBoxLayout();
    distortion_volume_and_mix_potentiometers_layout->addStretch(1);
    distortion_volume_and_mix_potentiometers_layout->addLayout(distortion_output_volume_layout);
    distortion_volume_and_mix_potentiometers_layout->addStretch(2);
    distortion_volume_and_mix_potentiometers_layout->addLayout(distortion_mix_layout);
    distortion_volume_and_mix_potentiometers_layout->addStretch(1);

    QHBoxLayout* distortion_drive_potentiometers_layout = new QHBoxLayout();
    distortion_drive_potentiometers_layout->addLayout(distortion_drive_layout);
    distortion_drive_potentiometers_layout->setAlignment(Qt::AlignHCenter);

    QWidget* distortion_settings_widget = new QWidget(this);
    QVBoxLayout* distortion_settings_layoput = new QVBoxLayout(distortion_settings_widget);
    distortion_settings_layoput->addLayout(distortion_volume_and_mix_potentiometers_layout);
    distortion_settings_layoput->addLayout(distortion_drive_potentiometers_layout);


    // -------- Effects settings stack

    m_effects_settings_stack_widget->setFixedSize(300, 200);
    m_effects_settings_stack_widget->insertWidget(0, off_settings_widget);
    m_effects_settings_stack_widget->insertWidget(1, reverb_settings_widget);
    m_effects_settings_stack_widget->insertWidget(2, delay_settings_widget);
    m_effects_settings_stack_widget->insertWidget(3, chorus_settings_widget);
    m_effects_settings_stack_widget->insertWidget(4, distortion_settings_widget);


    QVBoxLayout* effects_settings_layout = new QVBoxLayout();
    effects_settings_layout->setObjectName("effects_settings_layout");
    effects_settings_layout->addWidget(effect_settings_label);
    effects_settings_layout->addLayout(effect_value_display_layout);
    effects_settings_layout->addWidget(m_effects_settings_stack_widget);
    effects_settings_layout->addSpacing(20);



    // -------- Effects connects

    connectEffectsPotentiometersWithDisplay();



    // -------------------- input connector





    connect(m_audio_input_connector, &AudioSampleSelector::selectedAudioSample, this, &TrackSettings::changedAudioSamplePath);


    QVBoxLayout* audio_input_layout = new QVBoxLayout();
    audio_input_layout->setObjectName("audio_input_layout");
    audio_input_layout->setAlignment(Qt::AlignHCenter);
    audio_input_layout->addWidget(m_audio_input_connector);





    // -------------------- Layout (Timers + Track colors + REC + effects settings + input connector)

    QVBoxLayout* lag_duration_colors_rec_effectsSettings_inputConnector_layout = new QVBoxLayout();
    lag_duration_colors_rec_effectsSettings_inputConnector_layout->setObjectName("lag_duration_colors_rec_effectsSettings_inputConnector_layout");
    lag_duration_colors_rec_effectsSettings_inputConnector_layout->addLayout(lag_and_duration_setters_layout);
    lag_duration_colors_rec_effectsSettings_inputConnector_layout->addSpacing(20);
    lag_duration_colors_rec_effectsSettings_inputConnector_layout->addLayout(track_colors_and_rec_button_layout);
    lag_duration_colors_rec_effectsSettings_inputConnector_layout->addSpacing(20);
    lag_duration_colors_rec_effectsSettings_inputConnector_layout->addLayout(effects_settings_layout);
    lag_duration_colors_rec_effectsSettings_inputConnector_layout->addSpacing(20);
    lag_duration_colors_rec_effectsSettings_inputConnector_layout->addLayout(audio_input_layout);



    // -------------------- Main Layout --------------------

    QHBoxLayout* main_layout = new QHBoxLayout(this);
    main_layout->addLayout(volume_label_display_fader_layout);
    main_layout->addLayout(loopButton_16thBeats_effectSwitcher_emptyness_layout);
    main_layout->addLayout(lag_duration_colors_rec_effectsSettings_inputConnector_layout);
}












void TrackSettings::connectEffectsPotentiometersWithDisplay(){

    // Reverb

    connect(m_reverb_room_size_potentiometer, &QDial::valueChanged, m_effect_value_display, [this](int new_value){
        m_effect_value_display->setValue(new_value);
        emit changedReverbRoomSize(new_value);
    });

    connect(m_reverb_damping_potentiometer, &QDial::valueChanged, m_effect_value_display, [this](int new_value){
        m_effect_value_display->setValue(new_value);
        emit changedReverbDamping(new_value);
    });

    connect(m_reverb_wet_level_potentiometer, &QDial::valueChanged, m_effect_value_display, [this](int new_value){
        m_effect_value_display->setValue(new_value);
        emit changedReverbWetLevel(new_value);
    });

    connect(m_reverb_dry_level_potentiometer, &QDial::valueChanged, m_effect_value_display, [this](int new_value){
        m_effect_value_display->setValue(new_value);
        emit changedReverbDryLevel(new_value);
    });

    connect(m_reverb_output_volume_potentiometer, &QDial::valueChanged, m_effect_value_display, [this](int new_value){
        m_effect_value_display->setValue(new_value);
        emit changedReverbOutputVolume(new_value);
    });


    connect(m_reverb_room_size_potentiometer, &Potentiometer::entered, m_effect_value_display, &LCDDisplay::setValue);
    connect(m_reverb_damping_potentiometer, &Potentiometer::entered, m_effect_value_display, &LCDDisplay::setValue);
    connect(m_reverb_wet_level_potentiometer, &Potentiometer::entered, m_effect_value_display, &LCDDisplay::setValue);
    connect(m_reverb_dry_level_potentiometer, &Potentiometer::entered, m_effect_value_display, &LCDDisplay::setValue);
    connect(m_reverb_output_volume_potentiometer, &Potentiometer::entered, m_effect_value_display, &LCDDisplay::setValue);


    // Delay

    connect(m_delay_time_potentiometer, &QDial::valueChanged, m_effect_value_display, [this](int new_value){
        m_effect_value_display->setValue(new_value);
        emit changedDelayTime(new_value);
    });

    connect(m_delay_feedback_potentiometer, &QDial::valueChanged, m_effect_value_display, [this](int new_value){
        m_effect_value_display->setValue(new_value);
        emit changedDelayFeedback(new_value);
    });
    connect(m_delay_mix_potentiometer, &QDial::valueChanged, m_effect_value_display, [this](int new_value){
        m_effect_value_display->setValue(new_value);
        emit changedDelayMix(new_value);
    });
    connect(m_delay_output_volume_potentiometer, &QDial::valueChanged, m_effect_value_display, [this](int new_value){
        m_effect_value_display->setValue(new_value);
        emit changedDelayOutputVolume(new_value);
    });

    connect(m_delay_time_potentiometer, &Potentiometer::entered, m_effect_value_display, &LCDDisplay::setValue);
    connect(m_delay_feedback_potentiometer, &Potentiometer::entered, m_effect_value_display, &LCDDisplay::setValue);
    connect(m_delay_mix_potentiometer, &Potentiometer::entered, m_effect_value_display, &LCDDisplay::setValue);
    connect(m_delay_output_volume_potentiometer, &Potentiometer::entered, m_effect_value_display, &LCDDisplay::setValue);


    // Chorus

    connect(m_chorus_rate_potentiometer, &QDial::valueChanged, m_effect_value_display, [this](int new_value){
        m_effect_value_display->setValue(new_value);
        emit changedChorusRate(new_value);
    });

    connect(m_chorus_depth_potentiometer, &QDial::valueChanged, m_effect_value_display, [this](int new_value){
        m_effect_value_display->setValue(new_value);
        emit changedChorusDepth(new_value);
    });

    connect(m_chorus_center_delay_potentiometer, &QDial::valueChanged, m_effect_value_display, [this](int new_value){
        m_effect_value_display->setValue(new_value);
        emit changedChorusCenterDelay(new_value);
    });

    connect(m_chorus_feedback_potentiometer, &QDial::valueChanged, m_effect_value_display, [this](int new_value){
        m_effect_value_display->setValue(new_value);
        emit changedChorusFeedback(new_value);
    });

    connect(m_chorus_mix_potentiometer, &QDial::valueChanged, m_effect_value_display, [this](int new_value){
        m_effect_value_display->setValue(new_value);
        emit changedChorusMix(new_value);
    });

    connect(m_chorus_output_volume_potentiometer, &QDial::valueChanged, m_effect_value_display, [this](int new_value){
        m_effect_value_display->setValue(new_value);
        emit changedChorusOutputVolume(new_value);
    });

    connect(m_chorus_rate_potentiometer, &Potentiometer::entered, m_effect_value_display, &LCDDisplay::setValue);
    connect(m_chorus_depth_potentiometer, &Potentiometer::entered, m_effect_value_display, &LCDDisplay::setValue);
    connect(m_chorus_center_delay_potentiometer, &Potentiometer::entered, m_effect_value_display, &LCDDisplay::setValue);
    connect(m_chorus_feedback_potentiometer, &Potentiometer::entered, m_effect_value_display, &LCDDisplay::setValue);
    connect(m_chorus_mix_potentiometer, &Potentiometer::entered, m_effect_value_display, &LCDDisplay::setValue);
    connect(m_chorus_output_volume_potentiometer, &Potentiometer::entered, m_effect_value_display, &LCDDisplay::setValue);

    // Distortion

    connect(m_distortion_drive_potentiometer, &QDial::valueChanged, m_effect_value_display, [this](int new_value){
        m_effect_value_display->setValue(new_value);
        emit changedDistortionDrive(new_value);
    });

    connect(m_distortion_mix_potentiometer, &QDial::valueChanged, m_effect_value_display, [this](int new_value){
        m_effect_value_display->setValue(new_value);
        emit changedDistortionMix(new_value);
    });
    connect(m_distortion_output_volume_potentiometer, &QDial::valueChanged, m_effect_value_display, [this](int new_value){
        m_effect_value_display->setValue(new_value);
        emit changedDistortionOutputVolume(new_value);
    });

    connect(m_distortion_drive_potentiometer, &Potentiometer::entered, m_effect_value_display, &LCDDisplay::setValue);
    connect(m_distortion_mix_potentiometer, &Potentiometer::entered, m_effect_value_display, &LCDDisplay::setValue);
    connect(m_distortion_output_volume_potentiometer, &Potentiometer::entered, m_effect_value_display, &LCDDisplay::setValue);
}


void TrackSettings::closeEvent(QCloseEvent *event) {
    event->ignore();
    this->hide();
}




void TrackSettings::setVolume(float volume){
    m_volume_fader->setValue(volume*100);
}


void TrackSettings::setWholeTacktLag(qint16 value)
{
    m_whole_tacts_lag_setter->setValue(value);
}


void TrackSettings::setWholeTacktDuration(qint16 value)
{
    m_whole_tacts_duration_setter->setValue(value);
}



void TrackSettings::setLoopState(bool state){
    m_loop_button->setPressed(state);
}


void TrackSettings::setInnerActiveBackgroundColor(QColor color){
    m_select_track_colors_button->setInnerColor(color);
}
void TrackSettings::setOuterBackgroundColor(QColor color){
    m_select_track_colors_button->setOuterColor(color);
}

void TrackSettings::setBeats(std::array<bool, 16> beats_per_measure){
    m_beats_per_measure = beats_per_measure;
    for (int i = 0; i < 4 ; ++i){
        for (int j = 0; j < 4 ; ++j){
            qobject_cast<QCheckBox*>(m_16th_matrix->cellWidget(i, j)->layout()->itemAt(1)->widget())->setCheckState(m_beats_per_measure[i*4 + j] ? Qt::Checked : Qt::Unchecked);
        }
    }
}

void TrackSettings::setIsAudioSampleSelectedState(bool state){
    m_audio_input_connector->setIsAudioSampleSelectedState(state);
}


void TrackSettings::setRecordingEnabled(bool enabled)
{
    m_recording_button->setPressed(enabled);
}


void TrackSettings::changedBeat(){
    auto index = sender()->objectName().toUShort();
    auto state = qobject_cast<QCheckBox*>(sender())->isChecked();
    emit changedBeatState(index, state);
}



