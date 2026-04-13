#include "TrackSettings.h"
#include "BeatCheckBox.h"
#include "ClickableLabel.h"
#include <QStackedWidget>
#include "LCDCounter.h"
#include <QHeaderView>

TrackSettings* TrackSettings::m_instance = nullptr;

TrackSettings* TrackSettings::getInstance(quint8 volume_percent, bool is_loop, std::array<bool, 16> beats_per_measure, const QColor& outer_background_color, const QColor& inner_background_color, QWidget* parent)
{
    if (m_instance == nullptr)
        m_instance = new TrackSettings(volume_percent, is_loop, beats_per_measure, outer_background_color, inner_background_color, parent);

    return m_instance;
}


TrackSettings::TrackSettings(quint8 volume_percent, bool is_loop, const std::array<bool, 16>& beats_per_measure, const QColor& outer_background_color, const QColor& inner_background_color, QWidget* parent)
    : QDialog(parent)
    , m_volume_display (new LCDDisplay())
    , m_volume_fader(new Fader("..//..//images//fader_track.png", "..//..//images//fader_handle.png", "..//..//images//fader_measures.png", this))

    , m_select_track_colors_button(TrackColorButtons::getInstance(80, Qt::gray, Qt::darkGray, this))
    , m_loop_button(new RedButton(false, 80, this))
    , m_16th_matrix(new QTableWidget(4, 4, this))

    , m_lag_whole_takts_setter(new LCDCounter("whole takts"))
    , m_lag_16_th_setter(new LCDCounter("1/16 of takt"))
    , m_duration_whole_takts_setter(new LCDCounter("whole takts"))
    , m_duration_16_th_setter(new LCDCounter("1/16 of takt"))

    , m_effects_switcher(EffectsSwitcher::getInstance(140, this))
    , m_audio_input_connector(AudioSampleSelector::getInstance("..//..//images//audio_in_plugged.png", "..//..//images//audio_in_unplugged.png", this))
    , m_beats_per_measure(beats_per_measure)
{
    setWindowTitle("Track settings");
    setWindowFlags(Qt::Dialog);
    setFixedSize(600, 770);
    setStyleSheet("background-color: #8e8e8e;");










    // -------------------- Volume (title & display & fader) -----------------------------


    // -------------------- Volume title

    QLabel* volume_title = new QLabel("volume");
    volume_title->setStyleSheet("color: #ebebeb");

    // -------------------- Volume fader

    m_volume_fader->setFixedSize(100, 640);
    connect(m_volume_fader, &Fader::valueChanged, this, [this](int value) {
        m_volume_display->setValue(static_cast<uint16_t>(value));
    });
    connect(m_volume_fader, &Fader::valueChanged, this, &TrackSettings::changedVolume);



    // -------------------- Volume layout (title + display + fader)

    QVBoxLayout* volume_title_display_fader_layout = new QVBoxLayout();
    volume_title_display_fader_layout->setObjectName("volume_title_display_fader_layout");
    volume_title_display_fader_layout->addWidget(volume_title);
    volume_title_display_fader_layout->addWidget(m_volume_display);
    volume_title_display_fader_layout->addWidget(m_volume_fader);
    volume_title_display_fader_layout->setAlignment(Qt::AlignHCenter);
    volume_title_display_fader_layout->setSpacing(5);





    // --------------------  (Set track color button & Loop button) & 16th beats & Effects Switcher & REC button -----------------------------


    // -------------------- (Set track color button & Loop button) layout


    // --------- Track color title

    QLabel* select_track_color_title = new QLabel("track colors");
    select_track_color_title->setStyleSheet("color: #ebebeb");


    // --------- Track color button
    connect(m_select_track_colors_button, &TrackColorButtons::changedInnerColor, this, &TrackSettings::changedInnerActiveBackgroundColor);
    connect(m_select_track_colors_button, &TrackColorButtons::changedOuterColor, this, &TrackSettings::changedOuterBackgroundColor);

    QVBoxLayout* select_track_colors_layout = new QVBoxLayout();
    select_track_colors_layout->setObjectName("select_track_colors_layout");
    select_track_colors_layout->addWidget(select_track_color_title);
    select_track_colors_layout->addWidget(m_select_track_colors_button);
    select_track_colors_layout->setAlignment(Qt::AlignCenter);
    select_track_colors_layout->setSpacing(5);

    connect(m_loop_button, &RedButton::changedState, this, &TrackSettings::changedLoopState);



    // --------- Loop button

    QLabel* loop_title = new QLabel("loop");
    loop_title->setStyleSheet("color: #ebebeb");

    QVBoxLayout* loop_layout = new QVBoxLayout();
    loop_layout->setObjectName("loop_layout");
    loop_layout->addWidget(loop_title);
    loop_layout->addWidget(m_loop_button);
    loop_layout->setAlignment(Qt::AlignCenter);
    loop_layout->setSpacing(5);



    // --------- Layout (Track color button + Loop button)

    QHBoxLayout* color_button_and_loop_layout = new QHBoxLayout();
    color_button_and_loop_layout->addLayout(select_track_colors_layout);
    color_button_and_loop_layout->addLayout(loop_layout);




    // --------------------  16th beats


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
            current_16th_beat_layout->addWidget(current_16th_checkBox);

            m_16th_matrix->setCellWidget(i, j, container_for_16th_layout);
            connect(current_16th_checkBox, &QCheckBox::toggled, this, &TrackSettings::changedBeat);
        }
    }



    //beats_line_0_layout->setSpacing(25);




    QLabel* beats_title = new QLabel("1/16ths of takt");
    beats_title->setStyleSheet("color: #ebebeb");

    QVBoxLayout* beats_layout = new QVBoxLayout();
    beats_layout->setObjectName("beats_layout");
    beats_layout->addWidget(beats_title);
    beats_layout->addWidget(m_16th_matrix);
    beats_layout->setSpacing(10);
    beats_layout->setContentsMargins(0, 0, 0, 0);
    beats_layout->setAlignment(Qt::AlignHCenter);




    // --------------------  Effects Switcher

    m_effects_switcher->setValue(0);

    ClickableLabel* effect_title_1 = new ClickableLabel("1 Reverb", this);
    ClickableLabel* effect_title_2 = new ClickableLabel("2 Chorus", this);
    ClickableLabel* effect_title_3 = new ClickableLabel("3 Delay", this);
    ClickableLabel* effect_title_4 = new ClickableLabel("4 Distortion", this);
    effect_title_1->setStyleSheet("color: #ebebeb");
    effect_title_2->setStyleSheet("color: #ebebeb");
    effect_title_3->setStyleSheet("color: #ebebeb");
    effect_title_4->setStyleSheet("color: #ebebeb");

    QWidget::connect(m_effects_switcher, &EffectsSwitcher::settedFirstQuarter, this, [=](){
        effect_title_1->setStyleSheet("color: #ffef40");
        effect_title_2->setStyleSheet("color: #ebebeb");
        effect_title_3->setStyleSheet("color: #ebebeb");
        effect_title_4->setStyleSheet("color: #ebebeb");
    });

    QWidget::connect(m_effects_switcher, &EffectsSwitcher::settedSecondQuarter, this, [=](){
        effect_title_1->setStyleSheet("color: #ebebeb");
        effect_title_2->setStyleSheet("color: #ffef40");
        effect_title_3->setStyleSheet("color: #ebebeb");
        effect_title_4->setStyleSheet("color: #ebebeb");
    });

    QWidget::connect(m_effects_switcher, &EffectsSwitcher::settedThirdQuarter, this, [=](){
        effect_title_1->setStyleSheet("color: #ebebeb");
        effect_title_2->setStyleSheet("color: #ebebeb");
        effect_title_3->setStyleSheet("color: #ffef40");
        effect_title_4->setStyleSheet("color: #ebebeb");
    });

    QWidget::connect(m_effects_switcher, &EffectsSwitcher::settedFourthQuarter, this, [=](){
        effect_title_1->setStyleSheet("color: #ebebeb");
        effect_title_2->setStyleSheet("color: #ebebeb");
        effect_title_3->setStyleSheet("color: #ebebeb");
        effect_title_4->setStyleSheet("color: #ffef40");
    });


    QWidget::connect(effect_title_1, &ClickableLabel::clicked, this, [=](){
        m_effects_switcher->setValue(50);
        effect_title_1->setStyleSheet("color: #ffef40");
        effect_title_2->setStyleSheet("color: #ebebeb");
        effect_title_3->setStyleSheet("color: #ebebeb");
        effect_title_4->setStyleSheet("color: #ebebeb");
    });
    QWidget::connect(effect_title_2, &ClickableLabel::clicked, this, [=](){
        m_effects_switcher->setValue(75);
        effect_title_1->setStyleSheet("color: #ebebeb");
        effect_title_2->setStyleSheet("color: #ffef40");
        effect_title_3->setStyleSheet("color: #ebebeb");
        effect_title_4->setStyleSheet("color: #ebebeb");
    });
    QWidget::connect(effect_title_3, &ClickableLabel::clicked, this, [=](){
        m_effects_switcher->setValue(0);
        effect_title_1->setStyleSheet("color: #ebebeb");
        effect_title_2->setStyleSheet("color: #ebebeb");
        effect_title_3->setStyleSheet("color: #ffef40");
        effect_title_4->setStyleSheet("color: #ebebeb");
    });
    QWidget::connect(effect_title_4, &ClickableLabel::clicked, this, [=](){
        m_effects_switcher->setValue(25);
        effect_title_1->setStyleSheet("color: #ebebeb");
        effect_title_2->setStyleSheet("color: #ebebeb");
        effect_title_3->setStyleSheet("color: #ebebeb");
        effect_title_4->setStyleSheet("color: #ffef40");
    });


    QVBoxLayout* effects_titles_layout = new QVBoxLayout();
    effects_titles_layout->setObjectName("effects_titles_layout");
    effects_titles_layout->addWidget(effect_title_1);
    effects_titles_layout->addWidget(effect_title_2);
    effects_titles_layout->addWidget(effect_title_3);
    effects_titles_layout->addWidget(effect_title_4);
    effects_titles_layout->setAlignment(Qt::AlignCenter);
    effects_titles_layout->setSpacing(0);
    effects_titles_layout->setContentsMargins(0, -20, 0 ,-20);


    QVBoxLayout* effects_switcher_and_titles_layout = new QVBoxLayout();
    effects_switcher_and_titles_layout->setObjectName("effects_switcher_and_titles_layout");
    effects_switcher_and_titles_layout->addWidget(m_effects_switcher);
    effects_switcher_and_titles_layout->addLayout(effects_titles_layout);
    effects_switcher_and_titles_layout->setAlignment(Qt::AlignCenter);
    effects_switcher_and_titles_layout->setSpacing(90);




    // -------------------- REC button

    m_recording_checkbox = new QCheckBox("Enable Recording", this);
    m_recording_checkbox->setChecked(false);

    connect(m_recording_checkbox, &QCheckBox::toggled, this, [this](bool checked) {
        emit changedRecordingEnabled(checked);
    });





    // -------------------- (Track_color_button + Loop_button + 16th_beats) layout

    QVBoxLayout* trackColors_loopButton_16thBeats_effectSwitcher_RECButton_layout = new QVBoxLayout();
    trackColors_loopButton_16thBeats_effectSwitcher_RECButton_layout->setObjectName("trackColors_loopButton_16thBeats_effectSwitcher_RECButton_layout");
    trackColors_loopButton_16thBeats_effectSwitcher_RECButton_layout->addLayout(color_button_and_loop_layout);
    trackColors_loopButton_16thBeats_effectSwitcher_RECButton_layout->addLayout(beats_layout);
    trackColors_loopButton_16thBeats_effectSwitcher_RECButton_layout->addLayout(effects_switcher_and_titles_layout);
    trackColors_loopButton_16thBeats_effectSwitcher_RECButton_layout->addWidget(m_recording_checkbox);
    trackColors_loopButton_16thBeats_effectSwitcher_RECButton_layout->setAlignment(Qt::AlignHCenter);
    trackColors_loopButton_16thBeats_effectSwitcher_RECButton_layout->setSpacing(5);






    // --------------------  Timers & effects settings & input connector -------------------------------------------------



    // -------------------- Timers


    QHBoxLayout* lag_section_layout = new QHBoxLayout();
    lag_section_layout->addWidget(m_lag_whole_takts_setter);
    lag_section_layout->addWidget(m_lag_16_th_setter);


    QHBoxLayout* duration_section_layout = new QHBoxLayout();
    duration_section_layout->addWidget(m_duration_whole_takts_setter);
    duration_section_layout->addWidget(m_duration_16_th_setter);




    // -------------------- effects settings


    QLabel* effect_settings_title = new QLabel("Effect settings");
    effect_settings_title->setStyleSheet("color: #ebebeb");

    QStackedWidget* effects_settings = new QStackedWidget();
    effects_settings->setFixedSize(210, 210);

    QVBoxLayout*  effects_settings_layout = new QVBoxLayout();
    effects_settings_layout->setObjectName("effects_settings_layout");
    effects_settings_layout->addWidget(effect_settings_title);
    effects_settings_layout->addWidget(effects_settings);




    // -------------------- input connector


    QLabel* audio_input_connector_title = new QLabel("audio in");
    audio_input_connector_title->setStyleSheet("color: #ebebeb");

    connect(m_audio_input_connector, &AudioSampleSelector::selectedAudioSample, this, &TrackSettings::changedAudioSamplePath);


    QVBoxLayout* audio_input_layout = new QVBoxLayout();
    audio_input_layout->setObjectName("audio_input_layout");
    audio_input_layout->addWidget(audio_input_connector_title);
    audio_input_layout->addWidget(m_audio_input_connector);

    audio_input_layout->setAlignment(Qt::AlignCenter);
    audio_input_layout->setSpacing(5);




    // -------------------- Layout (Timers + effects settings + input connector)

    QVBoxLayout* lag_duration_effectsSettings_inputConnector_layout = new QVBoxLayout();
    lag_duration_effectsSettings_inputConnector_layout->setObjectName("lag_duration_effectsSettings_inputConnector_layout");
    lag_duration_effectsSettings_inputConnector_layout->addLayout(lag_section_layout);
    lag_duration_effectsSettings_inputConnector_layout->addLayout(duration_section_layout);
    lag_duration_effectsSettings_inputConnector_layout->addLayout(effects_settings_layout);
    lag_duration_effectsSettings_inputConnector_layout->addLayout(audio_input_layout);
    lag_duration_effectsSettings_inputConnector_layout->setAlignment(Qt::AlignCenter);
    lag_duration_effectsSettings_inputConnector_layout->setSpacing(5);







    // -------------------- Main Layout --------------------

    QHBoxLayout* main_layout = new QHBoxLayout(this);
    main_layout->addLayout(volume_title_display_fader_layout);
    main_layout->addLayout(trackColors_loopButton_16thBeats_effectSwitcher_RECButton_layout);
    main_layout->addLayout(lag_duration_effectsSettings_inputConnector_layout);
    main_layout->setSpacing(10);
}

void TrackSettings::closeEvent(QCloseEvent *event) {
    event->ignore();
    this->hide();
}


void TrackSettings::setLoopState(bool state){
    m_loop_button->setLooptState(state);
}

void TrackSettings::setVolume(int volum_percent){
    m_volume_fader->setValue(volum_percent);
}
void TrackSettings::setEffectVolume(int volum_percent){
    //m_effect_volume_fader->setValue(volum_percent);
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
    m_recording_checkbox->setChecked(enabled);
}

bool TrackSettings::getRecordingEnabled() const
{
    return m_recording_checkbox->isChecked();
}


void TrackSettings::changedBeat(){
    auto index = sender()->objectName().toUShort();
    auto state = qobject_cast<QCheckBox*>(sender())->isChecked();
    emit changedBeatState(index, state);
}



