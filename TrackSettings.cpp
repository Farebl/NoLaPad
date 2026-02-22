#include "TrackSettings.h"
#include "BeatCheckBox.h"
#include "ClickableLabel.h"
#include <QStackedWidget>


TrackSettings::TrackSettings(uint8_t volume_percent, bool is_loop, std::array<std::array<bool, 4>, 4> beats_per_measure, const QColor& outer_background_color, const QColor& inner_background_color, QWidget* parent)
    : QDialog(parent)
    , m_volume_display (new QSpinBox())
    , m_effect_volume_display(new QSpinBox())
    , m_volume_fader(new Fader(this, "..//..//images//fader_track.png", "..//..//images//fader_handle.png", "..//..//images//fader_measures.png"))
    , m_effect_volume_fader(new Fader(this, "..//..//images//fader_track.png", "..//..//images//fader_handle.png", "..//..//images//fader_measures.png"))

    , m_select_track_colors_button(new TrackColorButtons(this, 80, Qt::gray, Qt::darkGray))
    , m_loop_button(new LoopButton(this, false, 80))
    , m_beats_matrix_layout(new QVBoxLayout())

    , m_lag_whole_takts_display(new QSpinBox())
    , m_lag_16th_takts_display(new QSpinBox())
    , m_duration_whole_takts_display(new QSpinBox())
    , m_duration_16th_takts_display(new QSpinBox())

    , m_effects_switcher(new Potentiometer(this, 140))
    , m_audio_input_connector(new AudioSampleSelector(this,  "..//..//images//audio_in_plugged.png", "..//..//images//audio_in_unplugged.png"))
    , m_beats_per_measure(beats_per_measure)
{
    setWindowTitle("Track settings");
    setWindowFlags(Qt::Dialog);
    setFixedSize(600, 770);
    setStyleSheet("background-color: #8e8e8e;");




    QString LCD_DisplayStyle =
        "QSpinBox {"
        "    background-color: #000000;"
        "    color: #FFFFFF;"
        "    border: 2px solid #333333;"
        "    border-radius: 5px;"
        "    padding: 2px 2px;"
        "    qproperty-alignment: AlignCenter;"
        "}";







    // -------------------- Volume (title & display & fader) -----------------------------

    // -------------------- Volume title

    QLabel* volume_title = new QLabel("volume");
    volume_title->setStyleSheet("color: #ebebeb");



    // -------------------- LCD display for volume fader

    int font_id = QFontDatabase::addApplicationFont("..//..//fonts//dseg7-classic-latin-400-italic.ttf");
    QFont lcd_font;
    QString font_family = QFontDatabase::applicationFontFamilies(font_id).at(0);
    lcd_font = QFont(font_family, 24);

    m_volume_display->setRange(0, 100);
    m_volume_display->setValue(120);
    m_volume_display->setFont(lcd_font);
    m_volume_display->setFixedSize(100, 70);
    m_volume_display->setButtonSymbols(QAbstractSpinBox::NoButtons);
    m_volume_display->setStyleSheet(LCD_DisplayStyle);



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







    // -------------------- Effect (title & display & fader) -----------------------------

    // -------------------- Effect title
    QLabel* effect_volume_title = new QLabel("volume");
    effect_volume_title->setStyleSheet("color: #ebebeb; ");



    // -------------------- LCD display for effect volume fader

    m_effect_volume_display->setRange(0, 100);
    m_effect_volume_display->setValue(120);
    m_effect_volume_display->setFont(lcd_font);
    m_effect_volume_display->setFixedSize(100, 70);
    m_effect_volume_display->setButtonSymbols(QAbstractSpinBox::NoButtons);
    m_effect_volume_display->setStyleSheet(LCD_DisplayStyle);


    // -------------------- Effect volume fader

    m_effect_volume_fader->setFixedSize(100, 640);
    connect(m_effect_volume_fader, &Fader::valueChanged, this, [this](int value) {
        m_effect_volume_display->setValue(static_cast<uint16_t>(value));
    });



    // -------------------- Effect volume layout (title + display + fader)

    QVBoxLayout* effect_volume_title_display_fader_layout = new QVBoxLayout();
    effect_volume_title_display_fader_layout->setObjectName("effect_volume_title_display_fader_layouti");
    effect_volume_title_display_fader_layout->addWidget(effect_volume_title);
    effect_volume_title_display_fader_layout->addWidget(m_effect_volume_display);
    effect_volume_title_display_fader_layout->addWidget(m_effect_volume_fader);
    effect_volume_title_display_fader_layout->setAlignment(Qt::AlignHCenter);
    effect_volume_title_display_fader_layout->setSpacing(5);





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

    connect(m_loop_button, &LoopButton::changedState, this, &TrackSettings::changedLoopState);



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

    QHBoxLayout* beats_line_0_layout = new QHBoxLayout();
    QString beat_label_style("color: #ebebeb; font-size: 14px; text-align: center;");

    QVBoxLayout* beat0_layout = new QVBoxLayout();
    beat0_layout->addWidget(new QLabel(QString("1")));
    qobject_cast<QLabel*>(beat0_layout->itemAt(0)->widget())->setStyleSheet(beat_label_style);
    beat0_layout->addWidget(new BeatCheckBox((m_beats_per_measure[0][0]) ? true : false));
    beat0_layout->setAlignment(Qt::AlignHCenter);
    beat0_layout->setSpacing(0);
    beat0_layout->setObjectName("0");


    QVBoxLayout* beat1_layout = new QVBoxLayout();
    beat1_layout->addWidget(new QLabel(QString("1")));
    qobject_cast<QLabel*>(beat1_layout->itemAt(0)->widget())->setStyleSheet(beat_label_style);
    beat1_layout->addWidget(new BeatCheckBox((m_beats_per_measure[0][1]) ? true : false));
    beat1_layout->setAlignment(Qt::AlignHCenter);
    beat1_layout->setSpacing(0);
    beat1_layout->setObjectName("1");

    QVBoxLayout* beat2_layout = new QVBoxLayout();
    beat2_layout->addWidget(new QLabel(QString("2")));
    qobject_cast<QLabel*>(beat2_layout->itemAt(0)->widget())->setStyleSheet(beat_label_style);
    beat2_layout->addWidget(new BeatCheckBox((m_beats_per_measure[0][2]) ? true : false));
    beat2_layout->setAlignment(Qt::AlignHCenter);
    beat2_layout->setSpacing(0);
    beat2_layout->setObjectName("2");

    QVBoxLayout* beat3_layout = new QVBoxLayout();
    beat3_layout->addWidget(new QLabel(QString("3")));
    qobject_cast<QLabel*>(beat3_layout->itemAt(0)->widget())->setStyleSheet(beat_label_style);
    beat3_layout->addWidget(new BeatCheckBox((m_beats_per_measure[0][3]) ? true : false));
    beat3_layout->setAlignment(Qt::AlignHCenter);
    beat3_layout->setSpacing(0);
    beat3_layout->setObjectName("3");


    beats_line_0_layout->addLayout(beat0_layout);
    beats_line_0_layout->addLayout(beat1_layout);
    beats_line_0_layout->addLayout(beat2_layout);
    beats_line_0_layout->addLayout(beat3_layout);
    beats_line_0_layout->setSpacing(25);


    connect(qobject_cast<QCheckBox*>(beat0_layout->itemAt(1)->widget()), &QCheckBox::toggled, this, &TrackSettings::changedBeat);
    connect(qobject_cast<QCheckBox*>(beat1_layout->itemAt(1)->widget()), &QCheckBox::toggled, this, &TrackSettings::changedBeat);
    connect(qobject_cast<QCheckBox*>(beat2_layout->itemAt(1)->widget()), &QCheckBox::toggled, this, &TrackSettings::changedBeat);
    connect(qobject_cast<QCheckBox*>(beat3_layout->itemAt(1)->widget()), &QCheckBox::toggled, this, &TrackSettings::changedBeat);


    QHBoxLayout* beats_line_1_layout = new QHBoxLayout();

    QVBoxLayout* beat4_layout = new QVBoxLayout();
    beat4_layout->addWidget(new QLabel(QString("8")));
    qobject_cast<QLabel*>(beat4_layout->itemAt(0)->widget())->setStyleSheet(beat_label_style);
    beat4_layout->addWidget(new BeatCheckBox((m_beats_per_measure[1][0]) ? true : false));
    beat4_layout->setAlignment(Qt::AlignHCenter);
    beat4_layout->setSpacing(0);

    QVBoxLayout* beat5_layout = new QVBoxLayout();
    beat5_layout->addWidget(new QLabel(QString("5")));
    qobject_cast<QLabel*>(beat5_layout->itemAt(0)->widget())->setStyleSheet(beat_label_style);
    beat5_layout->addWidget(new BeatCheckBox((m_beats_per_measure[1][1]) ? true : false));
    beat5_layout->setAlignment(Qt::AlignHCenter);
    beat5_layout->setSpacing(0);

    QVBoxLayout* beat6_layout = new QVBoxLayout();
    beat6_layout->addWidget(new QLabel(QString("6")));
    qobject_cast<QLabel*>(beat6_layout->itemAt(0)->widget())->setStyleSheet(beat_label_style);
    beat6_layout->addWidget(new BeatCheckBox((m_beats_per_measure[1][2]) ? true : false));
    beat6_layout->setAlignment(Qt::AlignHCenter);
    beat6_layout->setSpacing(0);

    QVBoxLayout* beat7_layout = new QVBoxLayout();
    beat7_layout->addWidget(new QLabel(QString("7")));
    qobject_cast<QLabel*>(beat7_layout->itemAt(0)->widget())->setStyleSheet(beat_label_style);
    beat7_layout->addWidget(new BeatCheckBox((m_beats_per_measure[1][3]) ? true : false));
    beat7_layout->setAlignment(Qt::AlignHCenter);
    beat7_layout->setSpacing(0);


    beats_line_1_layout->addLayout(beat4_layout);
    beats_line_1_layout->addLayout(beat5_layout);
    beats_line_1_layout->addLayout(beat6_layout);
    beats_line_1_layout->addLayout(beat7_layout);
    beats_line_1_layout->setSpacing(25);


    connect(qobject_cast<QCheckBox*>(beat4_layout->itemAt(1)->widget()), &QCheckBox::toggled, this, &TrackSettings::changedBeat);
    connect(qobject_cast<QCheckBox*>(beat5_layout->itemAt(1)->widget()), &QCheckBox::toggled, this, &TrackSettings::changedBeat);
    connect(qobject_cast<QCheckBox*>(beat6_layout->itemAt(1)->widget()), &QCheckBox::toggled, this, &TrackSettings::changedBeat);
    connect(qobject_cast<QCheckBox*>(beat7_layout->itemAt(1)->widget()), &QCheckBox::toggled, this, &TrackSettings::changedBeat);


    QHBoxLayout* beats_line_2_layout = new QHBoxLayout();

    QVBoxLayout* beat8_layout = new QVBoxLayout();
    beat8_layout->addWidget(new QLabel(QString("12")));
    qobject_cast<QLabel*>(beat8_layout->itemAt(0)->widget())->setStyleSheet(beat_label_style);
    beat8_layout->addWidget(new BeatCheckBox((m_beats_per_measure[2][0]) ? true : false));
    beat8_layout->setAlignment(Qt::AlignHCenter);
    beat8_layout->setSpacing(0);


    QVBoxLayout* beat9_layout = new QVBoxLayout();
    beat9_layout->addWidget(new QLabel(QString("9")));
    qobject_cast<QLabel*>(beat9_layout->itemAt(0)->widget())->setStyleSheet(beat_label_style);
    beat9_layout->addWidget(new BeatCheckBox((m_beats_per_measure[2][1]) ? true : false));
    beat9_layout->setAlignment(Qt::AlignHCenter);
    beat9_layout->setSpacing(0);

    QVBoxLayout* beat10_layout = new QVBoxLayout();
    beat10_layout->addWidget(new QLabel(QString("10")));
    qobject_cast<QLabel*>(beat10_layout->itemAt(0)->widget())->setStyleSheet(beat_label_style);
    beat10_layout->addWidget(new BeatCheckBox((m_beats_per_measure[2][2]) ? true : false));
    beat10_layout->setAlignment(Qt::AlignHCenter);
    beat10_layout->setSpacing(0);

    QVBoxLayout* beat11_layout = new QVBoxLayout();
    beat11_layout->addWidget(new QLabel(QString("11")));
    qobject_cast<QLabel*>(beat11_layout->itemAt(0)->widget())->setStyleSheet(beat_label_style);
    beat11_layout->addWidget(new BeatCheckBox((m_beats_per_measure[2][3]) ? true : false));
    beat11_layout->setAlignment(Qt::AlignHCenter);
    beat11_layout->setSpacing(0);


    beats_line_2_layout->addLayout(beat8_layout);
    beats_line_2_layout->addLayout(beat9_layout);
    beats_line_2_layout->addLayout(beat10_layout);
    beats_line_2_layout->addLayout(beat11_layout);

    beats_line_2_layout->setSpacing(25);


    connect(qobject_cast<QCheckBox*>(beat8_layout->itemAt(1)->widget()), &QCheckBox::toggled, this, &TrackSettings::changedBeat);
    connect(qobject_cast<QCheckBox*>(beat9_layout->itemAt(1)->widget()), &QCheckBox::toggled, this, &TrackSettings::changedBeat);
    connect(qobject_cast<QCheckBox*>(beat10_layout->itemAt(1)->widget()), &QCheckBox::toggled, this, &TrackSettings::changedBeat);
    connect(qobject_cast<QCheckBox*>(beat11_layout->itemAt(1)->widget()), &QCheckBox::toggled, this, &TrackSettings::changedBeat);



    QHBoxLayout* beats_line_3_layout = new QHBoxLayout();

    QVBoxLayout* beat12_layout = new QVBoxLayout();
    beat12_layout->addWidget(new QLabel(QString("12")));
    qobject_cast<QLabel*>(beat12_layout->itemAt(0)->widget())->setStyleSheet(beat_label_style);
    beat12_layout->addWidget(new BeatCheckBox((m_beats_per_measure[3][0]) ? true : false));
    beat12_layout->setAlignment(Qt::AlignHCenter);
    beat12_layout->setSpacing(0);

    QVBoxLayout* beat13_layout = new QVBoxLayout();
    beat13_layout->addWidget(new QLabel(QString("13")));
    qobject_cast<QLabel*>(beat13_layout->itemAt(0)->widget())->setStyleSheet(beat_label_style);
    beat13_layout->addWidget(new BeatCheckBox((m_beats_per_measure[3][1]) ? true : false));
    beat13_layout->setAlignment(Qt::AlignHCenter);
    beat13_layout->setSpacing(0);

    QVBoxLayout* beat14_layout = new QVBoxLayout();
    beat14_layout->addWidget(new QLabel(QString("14")));
    qobject_cast<QLabel*>(beat14_layout->itemAt(0)->widget())->setStyleSheet(beat_label_style);
    beat14_layout->addWidget(new BeatCheckBox((m_beats_per_measure[3][2]) ? true : false));
    beat14_layout->setAlignment(Qt::AlignHCenter);
    beat14_layout->setSpacing(0);

    QVBoxLayout* beat15_layout = new QVBoxLayout();
    beat15_layout->addWidget(new QLabel(QString("15")));
    qobject_cast<QLabel*>(beat15_layout->itemAt(0)->widget())->setStyleSheet(beat_label_style);
    beat15_layout->addWidget(new BeatCheckBox((m_beats_per_measure[3][3]) ? true : false));
    beat15_layout->setAlignment(Qt::AlignHCenter);
    beat15_layout->setSpacing(0);


    beats_line_3_layout->addLayout(beat12_layout);
    beats_line_3_layout->addLayout(beat13_layout);
    beats_line_3_layout->addLayout(beat14_layout);
    beats_line_3_layout->addLayout(beat15_layout);
    beats_line_3_layout->setSpacing(25);


    connect(qobject_cast<QCheckBox*>(beat12_layout->itemAt(1)->widget()), &QCheckBox::toggled, this, &TrackSettings::changedBeat);
    connect(qobject_cast<QCheckBox*>(beat13_layout->itemAt(1)->widget()), &QCheckBox::toggled, this, &TrackSettings::changedBeat);
    connect(qobject_cast<QCheckBox*>(beat14_layout->itemAt(1)->widget()), &QCheckBox::toggled, this, &TrackSettings::changedBeat);
    connect(qobject_cast<QCheckBox*>(beat15_layout->itemAt(1)->widget()), &QCheckBox::toggled, this, &TrackSettings::changedBeat);


    m_beats_matrix_layout->addLayout(beats_line_0_layout);
    m_beats_matrix_layout->addLayout(beats_line_1_layout);
    m_beats_matrix_layout->addLayout(beats_line_2_layout);
    m_beats_matrix_layout->addLayout(beats_line_3_layout);
    m_beats_matrix_layout->setSpacing(5);
    m_beats_matrix_layout->setContentsMargins(0, 0, 0, 0);


    QLabel* beats_title = new QLabel("1/16th of tact");
    beats_title->setStyleSheet("color: #ebebeb");

    QVBoxLayout* beats_layout = new QVBoxLayout();
    beats_layout->setObjectName("beats_layout");
    beats_layout->addWidget(beats_title);
    beats_layout->addLayout(m_beats_matrix_layout);
    beats_layout->setSpacing(10);
    beats_layout->setContentsMargins(0, 0, 0, 0);
    beats_layout->setAlignment(Qt::AlignHCenter);




    // --------------------  Effects Switcher

    m_effects_switcher->setValue(0);

    ClickableLabel* effect_title_1 = new ClickableLabel(this, "1 Reverb ");
    ClickableLabel* effect_title_2 = new ClickableLabel(this, "2 Chorus ");
    ClickableLabel* effect_title_3 = new ClickableLabel(this, "3 Delay ");
    ClickableLabel* effect_title_4 = new ClickableLabel(this, "4 Distortion");
    effect_title_1->setStyleSheet("color: #ebebeb");
    effect_title_2->setStyleSheet("color: #ebebeb");
    effect_title_3->setStyleSheet("color: #ebebeb");
    effect_title_4->setStyleSheet("color: #ebebeb");

    QWidget::connect(m_effects_switcher, &Potentiometer::settedFirstQuarter, this, [=](){
        effect_title_1->setStyleSheet("color: #ffef40");
        effect_title_2->setStyleSheet("color: #ebebeb");
        effect_title_3->setStyleSheet("color: #ebebeb");
        effect_title_4->setStyleSheet("color: #ebebeb");
    });

    QWidget::connect(m_effects_switcher, &Potentiometer::settedSecondQuarter, this, [=](){
        effect_title_1->setStyleSheet("color: #ebebeb");
        effect_title_2->setStyleSheet("color: #ffef40");
        effect_title_3->setStyleSheet("color: #ebebeb");
        effect_title_4->setStyleSheet("color: #ebebeb");
    });

    QWidget::connect(m_effects_switcher, &Potentiometer::settedThirdQuarter, this, [=](){
        effect_title_1->setStyleSheet("color: #ebebeb");
        effect_title_2->setStyleSheet("color: #ebebeb");
        effect_title_3->setStyleSheet("color: #ffef40");
        effect_title_4->setStyleSheet("color: #ebebeb");
    });

    QWidget::connect(m_effects_switcher, &Potentiometer::settedFourthQuarter, this, [=](){
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


    // ---------- lag whoole takts


    QLabel* lag_whole_takts_title = new QLabel("whole takts");
    lag_whole_takts_title->setStyleSheet("color: #ebebeb");

    //display
    m_lag_whole_takts_display->setRange(0, 999);
    m_lag_whole_takts_display->setValue(0);
    m_lag_whole_takts_display->setFont(lcd_font);
    m_lag_whole_takts_display->setFixedSize(100, 70);
    m_lag_whole_takts_display->setButtonSymbols(QAbstractSpinBox::NoButtons);
    m_lag_whole_takts_display->setStyleSheet(LCD_DisplayStyle);

    // lag whoole takts buttons
    const QString lag_and_duration_buttons_style =
        "QPushButton {"
        "    background-color: #E0E0E0;"
        "    color: #505050;"
        "    border: none;"
        "    border-radius: 2px;"
        "    font-size: 12px;"
        "    padding: 0px;"
        "}"
        "QPushButton:hover {"
        "    background-color: #FFFFFF;"
        "}";

    QPushButton* lag_whole_takts_down_button(new QPushButton("◀", this));
    QPushButton* lag_whole_takts_up_button(new QPushButton("▶", this));


    lag_whole_takts_up_button->setFixedSize(25, 16);
    lag_whole_takts_down_button->setStyleSheet(lag_and_duration_buttons_style);

    lag_whole_takts_down_button->setFixedSize(25, 16);
    lag_whole_takts_down_button->setStyleSheet(lag_and_duration_buttons_style);



    connect(lag_whole_takts_up_button, &QPushButton::clicked, this, [this]() {
        m_lag_whole_takts_display->setValue(m_lag_whole_takts_display->value() + 1);

    });
    connect(lag_whole_takts_down_button, &QPushButton::clicked, this, [this]() {
        if (m_lag_whole_takts_display->value() > 0) {
            m_lag_whole_takts_display->setValue(m_lag_whole_takts_display->value() - 1);
        }
    });

    QHBoxLayout* lag_whole_takts_buttons = new QHBoxLayout();
    lag_whole_takts_buttons->addWidget(lag_whole_takts_down_button);
    lag_whole_takts_buttons->addWidget(lag_whole_takts_up_button);


    QVBoxLayout* lag_whole_takts_with_title_display_and_buttons = new QVBoxLayout();
    lag_whole_takts_with_title_display_and_buttons->setObjectName("lag_whole_takts_with_title_display_and_buttons");
    lag_whole_takts_with_title_display_and_buttons->addWidget(lag_whole_takts_title);
    lag_whole_takts_with_title_display_and_buttons->addWidget(m_lag_whole_takts_display);
    lag_whole_takts_with_title_display_and_buttons->addLayout(lag_whole_takts_buttons); // buttons
    lag_whole_takts_with_title_display_and_buttons->setAlignment(Qt::AlignHCenter);
    lag_whole_takts_with_title_display_and_buttons->setSpacing(5);





    // ---------- lag 1/16 takts

    QLabel* lag_16th_takts_title = new QLabel("16th takts");
    lag_16th_takts_title->setStyleSheet("color: #ebebeb");

    //display
    m_lag_16th_takts_display->setRange(0, 999);
    m_lag_16th_takts_display->setValue(0);
    m_lag_16th_takts_display->setFont(lcd_font);
    m_lag_16th_takts_display->setFixedSize(100, 70);
    m_lag_16th_takts_display->setButtonSymbols(QAbstractSpinBox::NoButtons);
    m_lag_16th_takts_display->setStyleSheet(LCD_DisplayStyle);

    // lag whoole takts buttons
    QPushButton* lag_16th_takts_down_button(new QPushButton("◀", this));
    QPushButton* lag_16th_takts_up_button(new QPushButton("▶", this));


    lag_16th_takts_up_button->setFixedSize(25, 16);
    lag_16th_takts_down_button->setStyleSheet(lag_and_duration_buttons_style);

    lag_16th_takts_down_button->setFixedSize(25, 16);
    lag_16th_takts_down_button->setStyleSheet(lag_and_duration_buttons_style);



    connect(lag_16th_takts_up_button, &QPushButton::clicked, this, [this]() {
        m_lag_16th_takts_display->setValue(m_lag_16th_takts_display->value() + 1);

    });
    connect(lag_16th_takts_down_button, &QPushButton::clicked, this, [this]() {
        if (m_lag_16th_takts_display->value() > 0) {
            m_lag_16th_takts_display->setValue(m_lag_16th_takts_display->value() - 1);
        }
    });

    QHBoxLayout* lag_16th_takts_buttons = new QHBoxLayout();
    lag_16th_takts_buttons->addWidget(lag_16th_takts_down_button);
    lag_16th_takts_buttons->addWidget(lag_16th_takts_up_button);


    QVBoxLayout* lag_16th_takts_with_title_display_and_buttons = new QVBoxLayout();
    lag_16th_takts_with_title_display_and_buttons->setObjectName("lag_16th_takts_with_title_display_and_buttons");
    lag_16th_takts_with_title_display_and_buttons->addWidget(lag_16th_takts_title);
    lag_16th_takts_with_title_display_and_buttons->addWidget(m_lag_16th_takts_display);
    lag_16th_takts_with_title_display_and_buttons->addLayout(lag_16th_takts_buttons); // buttons
    lag_16th_takts_with_title_display_and_buttons->setAlignment(Qt::AlignHCenter);
    lag_16th_takts_with_title_display_and_buttons->setSpacing(5);





    // ---------- duration whoole takts

    QLabel* duration_whole_takts_title = new QLabel("whole takts");
    duration_whole_takts_title->setStyleSheet("color: #ebebeb");

    //display
    m_duration_whole_takts_display->setRange(0, 999);
    m_duration_whole_takts_display->setValue(0);
    m_duration_whole_takts_display->setFont(lcd_font);
    m_duration_whole_takts_display->setFixedSize(100, 70);
    m_duration_whole_takts_display->setButtonSymbols(QAbstractSpinBox::NoButtons);
    m_duration_whole_takts_display->setStyleSheet(LCD_DisplayStyle);


    // lag whoole takts buttons

    QPushButton* duration_whole_takts_down_button(new QPushButton("◀", this));
    QPushButton* duration_whole_takts_up_button(new QPushButton("▶", this));


    duration_whole_takts_up_button->setFixedSize(25, 16);
    duration_whole_takts_down_button->setStyleSheet(lag_and_duration_buttons_style);

    duration_whole_takts_down_button->setFixedSize(25, 16);
    duration_whole_takts_down_button->setStyleSheet(lag_and_duration_buttons_style);



    connect(duration_whole_takts_up_button, &QPushButton::clicked, this, [this]() {
        m_duration_whole_takts_display->setValue(m_duration_whole_takts_display->value() + 1);

    });
    connect(duration_whole_takts_down_button, &QPushButton::clicked, this, [this]() {
        if (m_duration_whole_takts_display->value() > 0) {
            m_duration_whole_takts_display->setValue(m_duration_whole_takts_display->value() - 1);
        }
    });

    QHBoxLayout* duration_whole_takts_buttons = new QHBoxLayout();
    duration_whole_takts_buttons->addWidget(duration_whole_takts_down_button);
    duration_whole_takts_buttons->addWidget(duration_whole_takts_up_button);


    QVBoxLayout* duration_whole_takts_with_title_display_and_buttons = new QVBoxLayout();
    duration_whole_takts_with_title_display_and_buttons->setObjectName("duration_whole_takts_with_title_display_and_buttons");
    duration_whole_takts_with_title_display_and_buttons->addWidget(duration_whole_takts_title);
    duration_whole_takts_with_title_display_and_buttons->addWidget(m_duration_whole_takts_display);
    duration_whole_takts_with_title_display_and_buttons->addLayout(duration_whole_takts_buttons); // buttons
    duration_whole_takts_with_title_display_and_buttons->setAlignment(Qt::AlignHCenter);
    duration_whole_takts_with_title_display_and_buttons->setSpacing(5);





    // ---------- duration 1/16 takts

    QLabel* duration_16th_takts_title = new QLabel("16th takts");
    duration_16th_takts_title->setStyleSheet("color: #ebebeb");

    //display
    m_duration_16th_takts_display->setValue(0);
    m_duration_16th_takts_display->setFont(lcd_font);
    m_duration_16th_takts_display->setFixedSize(100, 70);
    m_duration_16th_takts_display->setButtonSymbols(QAbstractSpinBox::NoButtons);
    m_duration_16th_takts_display->setStyleSheet(LCD_DisplayStyle);

    // duration whoole takts buttons
    QPushButton* duration_16th_takts_down_button(new QPushButton("◀", this));
    QPushButton* duration_16th_takts_up_button(new QPushButton("▶", this));


    duration_16th_takts_up_button->setFixedSize(25, 16);
    duration_16th_takts_down_button->setStyleSheet(lag_and_duration_buttons_style);

    duration_16th_takts_down_button->setFixedSize(25, 16);
    duration_16th_takts_down_button->setStyleSheet(lag_and_duration_buttons_style);



    connect(duration_16th_takts_up_button, &QPushButton::clicked, this, [this]() {
        m_duration_16th_takts_display->setValue(m_duration_16th_takts_display->value() + 1);

    });
    connect(duration_16th_takts_down_button, &QPushButton::clicked, this, [this]() {
        if (m_duration_16th_takts_display->value() > 0) {
            m_duration_16th_takts_display->setValue(m_duration_16th_takts_display->value() - 1);
        }
    });

    QHBoxLayout* duration_16th_takts_buttons = new QHBoxLayout();
    duration_16th_takts_buttons->addWidget(duration_16th_takts_down_button);
    duration_16th_takts_buttons->addWidget(duration_16th_takts_up_button);


    QVBoxLayout* duration_16th_takts_with_title_display_and_buttons = new QVBoxLayout();
    duration_16th_takts_with_title_display_and_buttons->setObjectName("duration_16th_takts_with_title_display_and_buttons");
    duration_16th_takts_with_title_display_and_buttons->addWidget(duration_16th_takts_title);
    duration_16th_takts_with_title_display_and_buttons->addWidget(m_duration_16th_takts_display);
    duration_16th_takts_with_title_display_and_buttons->addLayout(duration_16th_takts_buttons); // buttons
    duration_16th_takts_with_title_display_and_buttons->setAlignment(Qt::AlignHCenter);
    duration_16th_takts_with_title_display_and_buttons->setSpacing(5);



    QHBoxLayout* lag_section_layout = new QHBoxLayout();
    lag_section_layout->addLayout(lag_whole_takts_with_title_display_and_buttons);
    lag_section_layout->addLayout(lag_16th_takts_with_title_display_and_buttons);


    QHBoxLayout* duration_section_layout = new QHBoxLayout();
    duration_section_layout->addLayout(duration_whole_takts_with_title_display_and_buttons);
    duration_section_layout->addLayout(duration_16th_takts_with_title_display_and_buttons);





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
    main_layout->addLayout(effect_volume_title_display_fader_layout);
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
    m_effect_volume_fader->setValue(volum_percent);
}

void TrackSettings::setInnerActiveBackgroundColor(QColor color){
    m_select_track_colors_button->setInnerColor(color);
}
void TrackSettings::setOuterBackgroundColor(QColor color){
    m_select_track_colors_button->setOuterColor(color);
}

void TrackSettings::setBeats(std::array<std::array<bool, 4>, 4> beats_per_measure){
    m_beats_per_measure = beats_per_measure;

    for (int i = 0; i<4 ; ++i){
        for (int j = 0; j<4 ; ++j){
            if(m_beats_per_measure[i][j]){
                qobject_cast<QCheckBox*>(m_beats_matrix_layout->itemAt(i)->layout()->itemAt(j)->layout()->itemAt(1)->widget())->setCheckState(Qt::Checked);
            }
            else{
                qobject_cast<QCheckBox*>(m_beats_matrix_layout->itemAt(i)->layout()->itemAt(j)->layout()->itemAt(1)->widget())->setCheckState(Qt::Unchecked);
            }
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
    auto index = sender()->objectName().toUInt();
    auto state = qobject_cast<QCheckBox*>(sender())->isChecked();
    emit changedBeatState(index, state);
}
