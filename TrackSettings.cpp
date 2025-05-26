#include "TrackSettings.h"


TrackSettings::TrackSettings(uint8_t volume_percent, bool is_loop, std::vector<std::vector<bool>> beats_per_measure, const QColor& outer_background_color, const QColor& inner_background_color, QWidget* parent)
    : QDialog(parent)
    , m_volume_display (new QSpinBox())
    , m_effect_volume_display(new QSpinBox())
    , m_volume_fader(new Fader(this, "..//..//images//fader_track.png", "..//..//images//fader_handle.png", "..//..//images//fader_measures.png"))
    , m_effect_volume_fader(new Fader(this, "..//..//images//fader_track.png", "..//..//images//fader_handle.png", "..//..//images//fader_measures.png"))
    , m_select_track_colors_button(new TrackColorButtons(this, 80, Qt::gray, Qt::darkGray))
    , m_loop_button(new LoopButton())
    , m_effects_switch(new Potentiometer(this, 60))
    , m_beats_matrix_layout(new QVBoxLayout())
    , m_audio_input_connector(new AudioSampleSelector(this,  "..//..//images//audio_in_plugged.png", "..//..//images//audio_in_unplugged.png"))
    , m_beats_per_measure(beats_per_measure)
{
    setWindowTitle("Track settings");
    setWindowFlags(Qt::Dialog);
    setFixedSize(410, 770);
    setStyleSheet("background-color: #8e8e8e;");

    int fontId = QFontDatabase::addApplicationFont("..//..//fonts//dseg7-classic-latin-400-italic.ttf");
    QFont lcdFont;
    QString fontFamily = QFontDatabase::applicationFontFamilies(fontId).at(0);
    lcdFont = QFont(fontFamily, 24);


    m_volume_display->setRange(0, 100);
    m_volume_display->setValue(120);
    m_volume_display->setFont(lcdFont);
    m_volume_display->setFixedSize(100, 70);
    m_volume_display->setButtonSymbols(QAbstractSpinBox::NoButtons);

    m_effect_volume_display->setRange(0, 100);
    m_effect_volume_display->setValue(120);
    m_effect_volume_display->setFont(lcdFont);
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

    QHBoxLayout* l1 = new QHBoxLayout();
    l1->addLayout(volume_display_layout);
    l1->addLayout(effect_volume_display_layout);


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


    QVBoxLayout* l3 = new QVBoxLayout(); // displays faders
    l3->addLayout(l1); // displays
    l3->addLayout(l2); // faders
    l3->setSpacing(10);


    m_select_track_colors_button->setFixedSize(80, 80);
    connect(m_select_track_colors_button, &TrackColorButtons::changedInnerColor, this, &TrackSettings::changedInnerBackgroundColor);
    connect(m_select_track_colors_button, &TrackColorButtons::changedOuterColor, this, &TrackSettings::changedOuterBackgroundColor);

    QLabel* select_track_color_title = new QLabel("track colors");
    select_track_color_title->setStyleSheet("color: #ebebeb");

    QVBoxLayout* select_track_colors_layout = new QVBoxLayout();
    select_track_colors_layout->addWidget(select_track_color_title);
    select_track_colors_layout->addWidget(m_select_track_colors_button);
    select_track_colors_layout->setAlignment(Qt::AlignCenter);
    select_track_colors_layout->setSpacing(5);


    m_loop_button->setFixedSize(80, 80);

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
    beats_line_1_layout->addWidget(new BeatCheckBox((m_beats_per_measure[0][0])?true:false)); //beat1
    beats_line_1_layout->addWidget(new BeatCheckBox((m_beats_per_measure[0][1])?true:false)); //beat2
    beats_line_1_layout->addWidget(new BeatCheckBox((m_beats_per_measure[0][2])?true:false)); //beat3
    beats_line_1_layout->addWidget(new BeatCheckBox((m_beats_per_measure[0][3])?true:false)); //beat4

    connect(qobject_cast<QCheckBox*>(beats_line_1_layout->itemAt(0)->widget()), &QCheckBox::toggled, this, &TrackSettings::changedBeat1);
    connect(qobject_cast<QCheckBox*>(beats_line_1_layout->itemAt(1)->widget()), &QCheckBox::toggled, this, &TrackSettings::changedBeat2);
    connect(qobject_cast<QCheckBox*>(beats_line_1_layout->itemAt(2)->widget()), &QCheckBox::toggled, this, &TrackSettings::changedBeat3);
    connect(qobject_cast<QCheckBox*>(beats_line_1_layout->itemAt(3)->widget()), &QCheckBox::toggled, this, &TrackSettings::changedBeat4);


    QHBoxLayout* beats_line_2_layout = new QHBoxLayout();
    beats_line_2_layout->addWidget(new BeatCheckBox((m_beats_per_measure[1][0])?true:false)); //beat5
    beats_line_2_layout->addWidget(new BeatCheckBox((m_beats_per_measure[1][1])?true:false)); //beat6
    beats_line_2_layout->addWidget(new BeatCheckBox((m_beats_per_measure[1][2])?true:false)); //beat7
    beats_line_2_layout->addWidget(new BeatCheckBox((m_beats_per_measure[1][3])?true:false)); //beat8


    connect(qobject_cast<QCheckBox*>(beats_line_2_layout->itemAt(0)->widget()), &QCheckBox::toggled, this, &TrackSettings::changedBeat5);
    connect(qobject_cast<QCheckBox*>(beats_line_2_layout->itemAt(1)->widget()), &QCheckBox::toggled, this, &TrackSettings::changedBeat6);
    connect(qobject_cast<QCheckBox*>(beats_line_2_layout->itemAt(2)->widget()), &QCheckBox::toggled, this, &TrackSettings::changedBeat7);
    connect(qobject_cast<QCheckBox*>(beats_line_2_layout->itemAt(3)->widget()), &QCheckBox::toggled, this, &TrackSettings::changedBeat8);


    QHBoxLayout* beats_line_3_layout = new QHBoxLayout();
    beats_line_3_layout->addWidget(new BeatCheckBox((m_beats_per_measure[2][0])?true:false)); //beat9
    beats_line_3_layout->addWidget(new BeatCheckBox((m_beats_per_measure[2][1])?true:false)); //beat10
    beats_line_3_layout->addWidget(new BeatCheckBox((m_beats_per_measure[2][2])?true:false)); //beat11
    beats_line_3_layout->addWidget(new BeatCheckBox((m_beats_per_measure[2][3])?true:false)); //beat12


    connect(qobject_cast<QCheckBox*>(beats_line_3_layout->itemAt(0)->widget()), &QCheckBox::toggled, this, &TrackSettings::changedBeat9);
    connect(qobject_cast<QCheckBox*>(beats_line_3_layout->itemAt(1)->widget()), &QCheckBox::toggled, this, &TrackSettings::changedBeat10);
    connect(qobject_cast<QCheckBox*>(beats_line_3_layout->itemAt(2)->widget()), &QCheckBox::toggled, this, &TrackSettings::changedBeat11);
    connect(qobject_cast<QCheckBox*>(beats_line_3_layout->itemAt(3)->widget()), &QCheckBox::toggled, this, &TrackSettings::changedBeat12);


    QHBoxLayout* beats_line_4_layout = new QHBoxLayout();
    beats_line_4_layout->addWidget(new BeatCheckBox((m_beats_per_measure[3][0])?true:false)); //beat13
    beats_line_4_layout->addWidget(new BeatCheckBox((m_beats_per_measure[3][1])?true:false)); //beat14
    beats_line_4_layout->addWidget(new BeatCheckBox((m_beats_per_measure[3][2])?true:false)); //beat15
    beats_line_4_layout->addWidget(new BeatCheckBox((m_beats_per_measure[3][3])?true:false)); //beat16


    connect(qobject_cast<QCheckBox*>(beats_line_4_layout->itemAt(0)->widget()), &QCheckBox::toggled, this, &TrackSettings::changedBeat13);
    connect(qobject_cast<QCheckBox*>(beats_line_4_layout->itemAt(1)->widget()), &QCheckBox::toggled, this, &TrackSettings::changedBeat14);
    connect(qobject_cast<QCheckBox*>(beats_line_4_layout->itemAt(2)->widget()), &QCheckBox::toggled, this, &TrackSettings::changedBeat15);
    connect(qobject_cast<QCheckBox*>(beats_line_4_layout->itemAt(3)->widget()), &QCheckBox::toggled, this, &TrackSettings::changedBeat16);



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



    m_effects_switch->setFixedSize(80, 80);
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

    QVBoxLayout* effects_layout= new QVBoxLayout();
    effects_layout->addWidget(m_effects_switch);
    effects_layout->addLayout(effects_titles_layout);
    effects_layout->setAlignment(Qt::AlignCenter);
    effects_layout->setSpacing(5);


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
    l5->setSpacing(45);


    QHBoxLayout* l6= new QHBoxLayout(this);
    l6->addLayout(l3); // displays faders
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
}
void TrackSettings::setEffectVolume(int volum_percent){
    m_effect_volume_fader->setValue(volum_percent);
}

void TrackSettings::setInnerBackgroundColor(QColor color){
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
                qobject_cast<QCheckBox*>(m_beats_matrix_layout->itemAt(i)->layout()->itemAt(j)->widget())->setCheckState(Qt::Checked);
            }
            else{
                qobject_cast<QCheckBox*>(m_beats_matrix_layout->itemAt(i)->layout()->itemAt(j)->widget())->setCheckState(Qt::Unchecked);
            }
        }
    }
}

void TrackSettings::setIsAudioSampleSelectedState(bool state){
    m_audio_input_connector->setIsAudioSampleSelectedState(state);
}
