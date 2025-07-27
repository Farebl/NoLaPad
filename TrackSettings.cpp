#include "TrackSettings.h"


TrackSettings::TrackSettings(QWidget* parent, uint8_t volume_percent, bool is_recording, bool is_loop, std::vector<std::vector<bool>> beats_per_measure, const QColor& outer_background_color, const QColor& inner_background_color)
    : QDialog(parent)
    , m_volume_display (new QSpinBox())
    , m_effect_volume_display(new QSpinBox())
    , m_volume_fader(new Fader(this, "..//..//images//fader_track.png", "..//..//images//fader_handle.png", "..//..//images//fader_measures.png"))
    , m_effect_volume_fader(new Fader(this, "..//..//images//fader_track.png", "..//..//images//fader_handle.png", "..//..//images//fader_measures.png"))
    , m_rec_button(new TrackRECButton(this, false, 50, 30))
    , m_select_track_colors_button(new TrackColorButtons(this, 80, Qt::gray, Qt::darkGray))
    , m_loop_button(new LoopButton(this, false, 80))
    , m_effects_switch(new Potentiometer(this, 140))
    , m_beats_matrix_layout(new QVBoxLayout())
    , m_audio_input_connector(new AudioSampleSelector(this,  "..//..//images//audio_in_plugged.png", "..//..//images//audio_in_unplugged.png"))
    , m_beats_per_measure(beats_per_measure)
{
    setWindowTitle("Track settings");
    setWindowFlags(Qt::Dialog);
    setFixedSize(470, 810);
    setStyleSheet("background-color: #8e8e8e;");

    int font_id = QFontDatabase::addApplicationFont("..//..//fonts//dseg7-classic-latin-400-italic.ttf");
    QFont lcd_font;
    QString font_family = QFontDatabase::applicationFontFamilies(font_id).at(0);
    lcd_font = QFont(font_family, 24);


    m_volume_display->setRange(0, 100);
    m_volume_display->setValue(120);
    m_volume_display->setFont(lcd_font);
    m_volume_display->setFixedSize(100, 70);
    m_volume_display->setButtonSymbols(QAbstractSpinBox::NoButtons);

    m_effect_volume_display->setRange(0, 100);
    m_effect_volume_display->setValue(120);
    m_effect_volume_display->setFont(lcd_font);
    m_effect_volume_display->setFixedSize(100, 70);
    m_effect_volume_display->setButtonSymbols(QAbstractSpinBox::NoButtons);

    // Один і той же стиль для обох
    QString spinBoxStyle =
        "QSpinBox {"
        "    background-color: #000000;"
        "    color: #FFFFFF;"
        "    border: 2px solid #333333;"
        "    border-radius: 5px;"
        "    padding: 2px 2px;"
        "    qproperty-alignment: AlignCenter;"
        "}";

    m_volume_display->setStyleSheet(spinBoxStyle);
    m_effect_volume_display->setStyleSheet(spinBoxStyle);

    QLabel* volume_title = new QLabel("volume");
    volume_title->setStyleSheet("color: #ebebeb");

    QVBoxLayout* volume_display_layout = new QVBoxLayout();
    volume_display_layout->addWidget(volume_title);
    volume_display_layout->addWidget(m_volume_display);
    volume_display_layout->setAlignment(Qt::AlignHCenter);
    volume_display_layout->setSpacing(5);

    QLabel* effect_volume_title = new QLabel("volume");
    effect_volume_title->setStyleSheet("color: #ebebeb; ");

    QVBoxLayout* effect_volume_display_layout = new QVBoxLayout();
    effect_volume_display_layout->addWidget(effect_volume_title);
    effect_volume_display_layout->addWidget(m_effect_volume_display);
    effect_volume_display_layout->setAlignment(Qt::AlignHCenter);
    effect_volume_display_layout->setSpacing(5);

    QHBoxLayout* l0 = new QHBoxLayout(); // lcd displays
    l0->addSpacing(30);
    l0->addLayout(volume_display_layout);
    l0->addLayout(effect_volume_display_layout);
    l0->setSpacing(30);


    m_rec_button->setRECState(is_recording);

    QLabel* rec_button_title = new QLabel("REC");
    rec_button_title->setStyleSheet("color: #ebebeb;");

    QVBoxLayout* rec_button_layout = new QVBoxLayout();
    rec_button_layout->setAlignment(Qt::AlignCenter);
    rec_button_layout->addWidget(rec_button_title);
    rec_button_layout->addWidget(m_rec_button);

    connect(m_rec_button, &TrackRECButton::changedState, this, &TrackSettings::changeRECState);

    QHBoxLayout* l1 = new QHBoxLayout();
    l1->setAlignment(Qt::AlignCenter);
    l1->addSpacing(40);
    l1->addLayout(rec_button_layout);



    m_volume_fader->setFixedSize(100, 640);
    connect(m_volume_fader, &Fader::valueChanged, this, [this](int value) {
        m_volume_display->setValue(static_cast<uint16_t>(value));
    });
    connect(m_volume_fader, &Fader::valueChanged, this, &TrackSettings::changedVolume);


    m_effect_volume_fader->setFixedSize(100, 640);
    connect(m_effect_volume_fader, &Fader::valueChanged, this, [this](int value) {
        m_effect_volume_display->setValue(static_cast<uint16_t>(value));
    });


    QHBoxLayout* l2 = new QHBoxLayout();
    l2->addWidget(m_volume_fader);
    l2->addWidget(m_effect_volume_fader);


    QVBoxLayout* l3 = new QVBoxLayout(); // displays & faders
    l3->setAlignment(Qt::AlignHCenter);
    l3->addLayout(l0); // lcd displays
    l3->addLayout(l1);
    l3->addLayout(l2); // faders
    l3->setSpacing(10);


    connect(m_select_track_colors_button, &TrackColorButtons::changedInnerColor, this, &TrackSettings::changedInnerActiveBackgroundColor);
    connect(m_select_track_colors_button, &TrackColorButtons::changedOuterColor, this, &TrackSettings::changedOuterBackgroundColor);

    QLabel* select_track_color_title = new QLabel("track colors");
    select_track_color_title->setStyleSheet("color: #ebebeb");

    QVBoxLayout* select_track_colors_layout = new QVBoxLayout();
    select_track_colors_layout->addWidget(select_track_color_title);
    select_track_colors_layout->addWidget(m_select_track_colors_button);
    select_track_colors_layout->setAlignment(Qt::AlignCenter);
    select_track_colors_layout->setSpacing(5);

    connect(m_loop_button, &LoopButton::changedState, this, &TrackSettings::changedLoopState);

    QLabel* loop_title = new QLabel("loop");
    loop_title->setStyleSheet("color: #ebebeb");

    QVBoxLayout* loop_layout = new QVBoxLayout();
    loop_layout->addWidget(loop_title);
    loop_layout->addWidget(m_loop_button);
    loop_layout->setAlignment(Qt::AlignCenter);
    loop_layout->setSpacing(5);


    QHBoxLayout* l4 = new QHBoxLayout();
    l4->addLayout(select_track_colors_layout);
    l4->addLayout(loop_layout);
    l4->setSpacing(10);
    l4->setContentsMargins(0, 0, 0, 0);


    if (m_beats_per_measure[0].size() < 4){
        m_beats_per_measure[0].resize(4, false);
    }
    if (m_beats_per_measure[1].size() < 4){
        m_beats_per_measure[1].resize(4, false);
    }
    if (m_beats_per_measure[2].size() < 4){
        m_beats_per_measure[2].resize(4, false);
    }
    if (m_beats_per_measure[3].size() < 4){
        m_beats_per_measure[3].resize(4, false);
    }



    QHBoxLayout* beats_line_1_layout = new QHBoxLayout();
    QString beat_label_style("color: #ebebeb; font-size: 14px; text-align: center;");

    QVBoxLayout* beat1_layout = new QVBoxLayout();
    beat1_layout->addWidget(new QLabel(QString("1")));
    qobject_cast<QLabel*>(beat1_layout->itemAt(0)->widget())->setStyleSheet(beat_label_style);
    beat1_layout->addWidget(new BeatCheckBox((m_beats_per_measure[0][0]) ? true : false));
    beat1_layout->setAlignment(Qt::AlignHCenter);
    beat1_layout->setSpacing(0);

    QVBoxLayout* beat2_layout = new QVBoxLayout();
    beat2_layout->addWidget(new QLabel(QString("2")));
    qobject_cast<QLabel*>(beat2_layout->itemAt(0)->widget())->setStyleSheet(beat_label_style);
    beat2_layout->addWidget(new BeatCheckBox((m_beats_per_measure[0][1]) ? true : false));
    beat2_layout->setAlignment(Qt::AlignHCenter);
    beat2_layout->setSpacing(0);

    QVBoxLayout* beat3_layout = new QVBoxLayout();
    beat3_layout->addWidget(new QLabel(QString("3")));
    qobject_cast<QLabel*>(beat3_layout->itemAt(0)->widget())->setStyleSheet(beat_label_style);
    beat3_layout->addWidget(new BeatCheckBox((m_beats_per_measure[0][2]) ? true : false));
    beat3_layout->setAlignment(Qt::AlignHCenter);
    beat3_layout->setSpacing(0);

    QVBoxLayout* beat4_layout = new QVBoxLayout();
    beat4_layout->addWidget(new QLabel(QString("4")));
    qobject_cast<QLabel*>(beat4_layout->itemAt(0)->widget())->setStyleSheet(beat_label_style);
    beat4_layout->addWidget(new BeatCheckBox((m_beats_per_measure[0][3]) ? true : false));
    beat4_layout->setAlignment(Qt::AlignHCenter);
    beat4_layout->setSpacing(0);

    beats_line_1_layout->addLayout(beat1_layout); // beat1
    beats_line_1_layout->addLayout(beat2_layout); // beat2
    beats_line_1_layout->addLayout(beat3_layout); // beat3
    beats_line_1_layout->addLayout(beat4_layout); // beat4
    beats_line_1_layout->setSpacing(25);

    // Підключення сигналів
    connect(qobject_cast<QCheckBox*>(beat1_layout->itemAt(1)->widget()), &QCheckBox::toggled, this, &TrackSettings::changedBeat1);
    connect(qobject_cast<QCheckBox*>(beat2_layout->itemAt(1)->widget()), &QCheckBox::toggled, this, &TrackSettings::changedBeat2);
    connect(qobject_cast<QCheckBox*>(beat3_layout->itemAt(1)->widget()), &QCheckBox::toggled, this, &TrackSettings::changedBeat3);
    connect(qobject_cast<QCheckBox*>(beat4_layout->itemAt(1)->widget()), &QCheckBox::toggled, this, &TrackSettings::changedBeat4);


    QHBoxLayout* beats_line_2_layout = new QHBoxLayout();

    QVBoxLayout* beat5_layout = new QVBoxLayout();
    beat5_layout->addWidget(new QLabel(QString("5")));
    qobject_cast<QLabel*>(beat5_layout->itemAt(0)->widget())->setStyleSheet(beat_label_style);
    beat5_layout->addWidget(new BeatCheckBox((m_beats_per_measure[1][0]) ? true : false));
    beat5_layout->setAlignment(Qt::AlignHCenter);
    beat5_layout->setSpacing(0);

    QVBoxLayout* beat6_layout = new QVBoxLayout();
    beat6_layout->addWidget(new QLabel(QString("6")));
    qobject_cast<QLabel*>(beat6_layout->itemAt(0)->widget())->setStyleSheet(beat_label_style);
    beat6_layout->addWidget(new BeatCheckBox((m_beats_per_measure[1][1]) ? true : false));
    beat6_layout->setAlignment(Qt::AlignHCenter);
    beat6_layout->setSpacing(0);

    QVBoxLayout* beat7_layout = new QVBoxLayout();
    beat7_layout->addWidget(new QLabel(QString("7")));
    qobject_cast<QLabel*>(beat7_layout->itemAt(0)->widget())->setStyleSheet(beat_label_style);
    beat7_layout->addWidget(new BeatCheckBox((m_beats_per_measure[1][2]) ? true : false));
    beat7_layout->setAlignment(Qt::AlignHCenter);
    beat7_layout->setSpacing(0);

    QVBoxLayout* beat8_layout = new QVBoxLayout();
    beat8_layout->addWidget(new QLabel(QString("8")));
    qobject_cast<QLabel*>(beat8_layout->itemAt(0)->widget())->setStyleSheet(beat_label_style);
    beat8_layout->addWidget(new BeatCheckBox((m_beats_per_measure[1][3]) ? true : false));
    beat8_layout->setAlignment(Qt::AlignHCenter);
    beat8_layout->setSpacing(0);

    beats_line_2_layout->addLayout(beat5_layout); // beat1
    beats_line_2_layout->addLayout(beat6_layout); // beat2
    beats_line_2_layout->addLayout(beat7_layout); // beat3
    beats_line_2_layout->addLayout(beat8_layout); // beat4
    beats_line_2_layout->setSpacing(25);

    // Підключення сигналів
    connect(qobject_cast<QCheckBox*>(beat5_layout->itemAt(1)->widget()), &QCheckBox::toggled, this, &TrackSettings::changedBeat5);
    connect(qobject_cast<QCheckBox*>(beat6_layout->itemAt(1)->widget()), &QCheckBox::toggled, this, &TrackSettings::changedBeat6);
    connect(qobject_cast<QCheckBox*>(beat7_layout->itemAt(1)->widget()), &QCheckBox::toggled, this, &TrackSettings::changedBeat7);
    connect(qobject_cast<QCheckBox*>(beat8_layout->itemAt(1)->widget()), &QCheckBox::toggled, this, &TrackSettings::changedBeat8);



    QHBoxLayout* beats_line_3_layout = new QHBoxLayout();

    QVBoxLayout* beat9_layout = new QVBoxLayout();
    beat9_layout->addWidget(new QLabel(QString("9")));
    qobject_cast<QLabel*>(beat9_layout->itemAt(0)->widget())->setStyleSheet(beat_label_style);
    beat9_layout->addWidget(new BeatCheckBox((m_beats_per_measure[2][0]) ? true : false));
    beat9_layout->setAlignment(Qt::AlignHCenter);
    beat9_layout->setSpacing(0);

    QVBoxLayout* beat10_layout = new QVBoxLayout();
    beat10_layout->addWidget(new QLabel(QString("10")));
    qobject_cast<QLabel*>(beat10_layout->itemAt(0)->widget())->setStyleSheet(beat_label_style);
    beat10_layout->addWidget(new BeatCheckBox((m_beats_per_measure[2][1]) ? true : false));
    beat10_layout->setAlignment(Qt::AlignHCenter);
    beat10_layout->setSpacing(0);

    QVBoxLayout* beat11_layout = new QVBoxLayout();
    beat11_layout->addWidget(new QLabel(QString("11")));
    qobject_cast<QLabel*>(beat11_layout->itemAt(0)->widget())->setStyleSheet(beat_label_style);
    beat11_layout->addWidget(new BeatCheckBox((m_beats_per_measure[2][2]) ? true : false));
    beat11_layout->setAlignment(Qt::AlignHCenter);
    beat11_layout->setSpacing(0);

    QVBoxLayout* beat12_layout = new QVBoxLayout();
    beat12_layout->addWidget(new QLabel(QString("12")));
    qobject_cast<QLabel*>(beat12_layout->itemAt(0)->widget())->setStyleSheet(beat_label_style);
    beat12_layout->addWidget(new BeatCheckBox((m_beats_per_measure[2][3]) ? true : false));
    beat12_layout->setAlignment(Qt::AlignHCenter);
    beat12_layout->setSpacing(0);

    beats_line_3_layout->addLayout(beat9_layout); // beat1
    beats_line_3_layout->addLayout(beat10_layout); // beat2
    beats_line_3_layout->addLayout(beat11_layout); // beat3
    beats_line_3_layout->addLayout(beat12_layout); // beat4
    beats_line_3_layout->setSpacing(25);



    // Підключення сигналів
    connect(qobject_cast<QCheckBox*>(beat9_layout->itemAt(1)->widget()), &QCheckBox::toggled, this, &TrackSettings::changedBeat9);
    connect(qobject_cast<QCheckBox*>(beat10_layout->itemAt(1)->widget()), &QCheckBox::toggled, this, &TrackSettings::changedBeat10);
    connect(qobject_cast<QCheckBox*>(beat11_layout->itemAt(1)->widget()), &QCheckBox::toggled, this, &TrackSettings::changedBeat11);
    connect(qobject_cast<QCheckBox*>(beat12_layout->itemAt(1)->widget()), &QCheckBox::toggled, this, &TrackSettings::changedBeat12);



    QHBoxLayout* beats_line_4_layout = new QHBoxLayout();


    QVBoxLayout* beat13_layout = new QVBoxLayout();
    beat13_layout->addWidget(new QLabel(QString("13")));
    qobject_cast<QLabel*>(beat13_layout->itemAt(0)->widget())->setStyleSheet(beat_label_style);
    beat13_layout->addWidget(new BeatCheckBox((m_beats_per_measure[3][0]) ? true : false));
    beat13_layout->setAlignment(Qt::AlignHCenter);
    beat13_layout->setSpacing(0);

    QVBoxLayout* beat14_layout = new QVBoxLayout();
    beat14_layout->addWidget(new QLabel(QString("14")));
    qobject_cast<QLabel*>(beat14_layout->itemAt(0)->widget())->setStyleSheet(beat_label_style);
    beat14_layout->addWidget(new BeatCheckBox((m_beats_per_measure[3][1]) ? true : false));
    beat14_layout->setAlignment(Qt::AlignHCenter);
    beat14_layout->setSpacing(0);

    QVBoxLayout* beat15_layout = new QVBoxLayout();
    beat15_layout->addWidget(new QLabel(QString("15")));
    qobject_cast<QLabel*>(beat15_layout->itemAt(0)->widget())->setStyleSheet(beat_label_style);
    beat15_layout->addWidget(new BeatCheckBox((m_beats_per_measure[3][2]) ? true : false));
    beat15_layout->setAlignment(Qt::AlignHCenter);
    beat15_layout->setSpacing(0);

    QVBoxLayout* beat16_layout = new QVBoxLayout();
    beat16_layout->addWidget(new QLabel(QString("16")));
    qobject_cast<QLabel*>(beat16_layout->itemAt(0)->widget())->setStyleSheet(beat_label_style);
    beat16_layout->addWidget(new BeatCheckBox((m_beats_per_measure[3][3]) ? true : false));
    beat16_layout->setAlignment(Qt::AlignHCenter);
    beat16_layout->setSpacing(0);

    beats_line_4_layout->addLayout(beat13_layout); // beat1
    beats_line_4_layout->addLayout(beat14_layout); // beat2
    beats_line_4_layout->addLayout(beat15_layout); // beat3
    beats_line_4_layout->addLayout(beat16_layout); // beat4
    beats_line_4_layout->setSpacing(25);

    // Підключення сигналів
    connect(qobject_cast<QCheckBox*>(beat13_layout->itemAt(1)->widget()), &QCheckBox::toggled, this, &TrackSettings::changedBeat13);
    connect(qobject_cast<QCheckBox*>(beat14_layout->itemAt(1)->widget()), &QCheckBox::toggled, this, &TrackSettings::changedBeat14);
    connect(qobject_cast<QCheckBox*>(beat15_layout->itemAt(1)->widget()), &QCheckBox::toggled, this, &TrackSettings::changedBeat15);
    connect(qobject_cast<QCheckBox*>(beat16_layout->itemAt(1)->widget()), &QCheckBox::toggled, this, &TrackSettings::changedBeat16);


    m_beats_matrix_layout->addLayout(beats_line_1_layout);
    m_beats_matrix_layout->addLayout(beats_line_2_layout);
    m_beats_matrix_layout->addLayout(beats_line_3_layout);
    m_beats_matrix_layout->addLayout(beats_line_4_layout);
    m_beats_matrix_layout->setSpacing(5); // Явно задаємо проміжок між дочірніми layout
    m_beats_matrix_layout->setContentsMargins(0, 0, 0, 0);

    QVBoxLayout* beats_layout = new QVBoxLayout();
    QLabel* beats_title = new QLabel("1/16th of tact");
    beats_title->setStyleSheet("color: #ebebeb");
    beats_layout->addWidget(beats_title);
    beats_layout->addLayout(m_beats_matrix_layout);
    beats_layout->setSpacing(10); // Прибираємо проміжки між елементами beats_layout
    beats_layout->setContentsMargins(0, 0, 0, 0);
    beats_layout->setAlignment(Qt::AlignHCenter);


    m_effects_switch->setValue(0);

    ClickableLabel* effect_title_1 = new ClickableLabel(this, "Reverb ");
    ClickableLabel* effect_title_2 = new ClickableLabel(this, "Chorus ");
    ClickableLabel* effect_title_3 = new ClickableLabel(this, "Delay ");
    ClickableLabel* effect_title_4 = new ClickableLabel(this, "Distortion");
    effect_title_1->setStyleSheet("color: #ebebeb");
    effect_title_2->setStyleSheet("color: #ebebeb");
    effect_title_3->setStyleSheet("color: #ebebeb");
    effect_title_4->setStyleSheet("color: #ebebeb");

    QWidget::connect(m_effects_switch, &Potentiometer::settedFirstQuarter, this, [=](){
        effect_title_1->setStyleSheet("color: #ffef40");
        effect_title_2->setStyleSheet("color: #ebebeb");
        effect_title_3->setStyleSheet("color: #ebebeb");
        effect_title_4->setStyleSheet("color: #ebebeb");
    });

    QWidget::connect(m_effects_switch, &Potentiometer::settedSecondQuarter, this, [=](){
        effect_title_1->setStyleSheet("color: #ebebeb");
        effect_title_2->setStyleSheet("color: #ffef40");
        effect_title_3->setStyleSheet("color: #ebebeb");
        effect_title_4->setStyleSheet("color: #ebebeb");
    });

    QWidget::connect(m_effects_switch, &Potentiometer::settedThirdQuarter, this, [=](){
        effect_title_1->setStyleSheet("color: #ebebeb");
        effect_title_2->setStyleSheet("color: #ebebeb");
        effect_title_3->setStyleSheet("color: #ffef40");
        effect_title_4->setStyleSheet("color: #ebebeb");
    });

    QWidget::connect(m_effects_switch, &Potentiometer::settedFourthQuarter, this, [=](){
        effect_title_1->setStyleSheet("color: #ebebeb");
        effect_title_2->setStyleSheet("color: #ebebeb");
        effect_title_3->setStyleSheet("color: #ebebeb");
        effect_title_4->setStyleSheet("color: #ffef40");
    });


    QWidget::connect(effect_title_1, &ClickableLabel::clicked, this, [=](){
        m_effects_switch->setValue(50);
        effect_title_1->setStyleSheet("color: #ffef40");
        effect_title_2->setStyleSheet("color: #ebebeb");
        effect_title_3->setStyleSheet("color: #ebebeb");
        effect_title_4->setStyleSheet("color: #ebebeb");
    });
    QWidget::connect(effect_title_2, &ClickableLabel::clicked, this, [=](){
        m_effects_switch->setValue(75);
        effect_title_1->setStyleSheet("color: #ebebeb");
        effect_title_2->setStyleSheet("color: #ffef40");
        effect_title_3->setStyleSheet("color: #ebebeb");
        effect_title_4->setStyleSheet("color: #ebebeb");
    });
    QWidget::connect(effect_title_3, &ClickableLabel::clicked, this, [=](){
        m_effects_switch->setValue(0);
        effect_title_1->setStyleSheet("color: #ebebeb");
        effect_title_2->setStyleSheet("color: #ebebeb");
        effect_title_3->setStyleSheet("color: #ffef40");
        effect_title_4->setStyleSheet("color: #ebebeb");
    });
    QWidget::connect(effect_title_4, &ClickableLabel::clicked, this, [=](){
        m_effects_switch->setValue(25);
        effect_title_1->setStyleSheet("color: #ebebeb");
        effect_title_2->setStyleSheet("color: #ebebeb");
        effect_title_3->setStyleSheet("color: #ebebeb");
        effect_title_4->setStyleSheet("color: #ffef40");
    });


    QVBoxLayout* effects_titles_layout= new QVBoxLayout();
    effects_titles_layout->addWidget(effect_title_1);
    effects_titles_layout->addWidget(effect_title_2);
    effects_titles_layout->addWidget(effect_title_3);
    effects_titles_layout->addWidget(effect_title_4);
    effects_titles_layout->setAlignment(Qt::AlignCenter);
    effects_titles_layout->setSpacing(0);
    effects_titles_layout->setContentsMargins(0, -20, 0 ,-20); // Додаємо відступи

    QVBoxLayout* effects_layout= new QVBoxLayout();
    effects_layout->addWidget(m_effects_switch);
    effects_layout->addLayout(effects_titles_layout);
    effects_layout->setAlignment(Qt::AlignCenter);
    effects_layout->setSpacing(90);


    QLabel* audio_input_connector_title = new QLabel("audio in");
    audio_input_connector_title->setStyleSheet("color: #ebebeb");

    connect(m_audio_input_connector, &AudioSampleSelector::selectedAudioSample, this, &TrackSettings::changedAudioSamplePath);


    QVBoxLayout* audio_input_layout = new QVBoxLayout();
    audio_input_layout->addWidget(audio_input_connector_title);
    audio_input_layout->addWidget(m_audio_input_connector);
    audio_input_layout->setAlignment(Qt::AlignCenter);
    audio_input_layout->setSpacing(5);


    QVBoxLayout* l5= new QVBoxLayout();
    l5->addLayout(l4); //colors & loop
    l5->addLayout(beats_layout);
    l5->addLayout(effects_layout);
    l5->addLayout(audio_input_layout);
    l5->setAlignment(Qt::AlignTop);
    l5->setSpacing(20);


    QHBoxLayout* l6= new QHBoxLayout(this);
    l6->addLayout(l3); // displays & faders
    l6->addLayout(l5); //colors & loop & beats & effects & plug_in
    l6->setSpacing(10);
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
    m_volume_display->setValue(volum_percent);
}


void TrackSettings::setEffectVolume(int volum_percent){
    m_effect_volume_fader->setValue(volum_percent);
    m_effect_volume_display->setValue(volum_percent);
}

void TrackSettings::setRECState(bool state){
    m_rec_button->setRECState(state);
}


void TrackSettings::setInnerActiveBackgroundColor(QColor color){
    m_select_track_colors_button->setInnerColor(color);
}
void TrackSettings::setOuterBackgroundColor(QColor color){
    m_select_track_colors_button->setOuterColor(color);
}


void TrackSettings::setBeats(std::vector<std::vector<bool>> beats_per_measure){
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
