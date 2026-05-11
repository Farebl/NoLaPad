#include <QtTest>
#include <QSignalSpy>
#include "Track.h"
#include "MicroTimer.h"
#include "MockTrackPlayer.h"


// =============================================================================
// TestTrack — unit-тести класу Track з використанням MockTrackPlayer.
//
// Усі тести використовують тестовий конструктор Track(ITrackPlayer*, ...),
// тому JUCETrackPlayer і JUCE-аудіо-пристрої НЕ задіяні.
// =============================================================================
class TestTrack : public QObject {
    Q_OBJECT

private slots:

    // ── Ініціалізація та стан за замовчуванням ──────────────────────────────
    void testConstructorSetsVolume();
    void testConstructorSetsLoopFalse();
    void testConstructorSetsRecordingFalse();
    void testGetPlayerReturnsNonNull();

    // ── Гучність ────────────────────────────────────────────────────────────
    void testSetVolumePropagatedToPlayer();
    void testVolumeBoundaryZero();
    void testVolumeBoundaryOne();
    void testGetVolumeReturnsPlayerValue();

    // ── Аудіо-шлях ──────────────────────────────────────────────────────────
    void testSetAudioPathPropagatedToPlayer();
    void testSetAudioPathMarksFileLoaded();
    void testEmptyPathDoesNotMarkFileLoaded();
    void testGetAudioSamplePath();
    void testGetSoundPath();

    // ── Луп ─────────────────────────────────────────────────────────────────
    void testSetLoopStateTrue();
    void testSetLoopStateFalse();
    void testGetLoopStateDefault();

    // ── Запис ───────────────────────────────────────────────────────────────
    void testSetRecordingStatePropagatedToPlayer();
    void testGetRecordingStateDefault();
    void testRecordingStateToggle();

    // ── Кольори ─────────────────────────────────────────────────────────────
    void testSetOuterColor();
    void testSetInnerColor();

    // ── Біти ────────────────────────────────────────────────────────────────
    void testGetBeatsStatesReturnsInitialPattern();
    void testSetBeatStateTrue();
    void testSetBeatStateFalse();
    void testAllBeatsCanBeSet();

    // ── Сигнал rightClicked ─────────────────────────────────────────────────
    void testRightClickEmitsSignalWithPointer();

    // ── Ефекти — тип ────────────────────────────────────────────────────────
    void testSetEffectTypeReverb();
    void testSetEffectTypeDelay();
    void testSetEffectTypeChorus();
    void testSetEffectTypeDistortion();
    void testSetEffectTypeNone();

    // ── Reverb — граничні значення ──────────────────────────────────────────
    void testReverbRoomSizeMinBoundary();
    void testReverbRoomSizeMaxBoundary();
    void testReverbRoomSizeOutOfRangeIgnored();
    void testReverbDampingBoundaries();
    void testReverbWetLevelBoundaries();
    void testReverbDryLevelBoundaries();
    void testReverbOutputVolumeBoundaries();

    // ── Delay — граничні значення ───────────────────────────────────────────
    void testDelayTimeBoundaries();
    void testDelayFeedbackMaxBoundary();
    void testDelayFeedbackOutOfRangeIgnored();
    void testDelayMixLevelBoundaries();
    void testDelayOutputVolumeBoundaries();

    // ── Chorus — граничні значення ──────────────────────────────────────────
    void testChorusRateMinBoundary();
    void testChorusRateMaxBoundary();
    void testChorusRateOutOfRangeIgnored();
    void testChorusDepthBoundaries();
    void testChorusCenterDelayBoundaries();
    void testChorusFeedbackMaxBoundary();
    void testChorusFeedbackOutOfRangeIgnored();
    void testChorusMixBoundaries();
    void testChorusOutputVolumeBoundaries();

    // ── Distortion — граничні значення ──────────────────────────────────────
    void testDistortionDriveMinBoundary();
    void testDistortionDriveMaxBoundary();
    void testDistortionDriveOutOfRangeIgnored();
    void testDistortionMixBoundaries();
    void testDistortionOutputVolumeBoundaries();
};

// =============================================================================
// Допоміжна функція — створює Track з ін'єктованим MockTrackPlayer.
// out_player отримує сирий вказівник на mock для перевірки стану у тестах.
// ВАЖЛИВО: не передавай стековий MockTrackPlayer — Track знищить його через
//          unique_ptr при деструкції.
// =============================================================================
static Track* makeTrack(
    MockTrackPlayer*& out_player,
    MicroTimer* timer = nullptr,
    float volume = 0.8f,
    bool is_loop = false,
    bool is_recording = false,
    std::array<bool, 16> beats = {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    const QString& path = ""
    ) {
    // 1. Створюємо відразу як MockTrackPlayer
    out_player = new MockTrackPlayer();

    // 2. Передаємо в Track (тут конвертація Mock* -> ITrackPlayer* безпечна і автоматична)
    return new Track(
        out_player, timer, volume, is_loop, is_recording, 0, 0,
        beats, path, Qt::gray, Qt::red, nullptr
    );
}



// =============================================================================
// ── Ініціалізація ────────────────────────────────────────────────────────────
// =============================================================================

void TestTrack::testConstructorSetsVolume() {
    MockTrackPlayer* player;
    QScopedPointer<Track> t(makeTrack(player, nullptr, 0.75f));
    QCOMPARE(player->volume, 0.75f);
}

void TestTrack::testConstructorSetsLoopFalse() {
    MockTrackPlayer* player;
    QScopedPointer<Track> t(makeTrack(player));
    QVERIFY(!t->getLoopState());
}

void TestTrack::testConstructorSetsRecordingFalse() {
    MockTrackPlayer* player;
    QScopedPointer<Track> t(makeTrack(player));
    QVERIFY(!t->getRecordingState());
}

void TestTrack::testGetPlayerReturnsNonNull() {
    MockTrackPlayer* player;
    QScopedPointer<Track> t(makeTrack(player));
    QVERIFY(t->getPlayer() != nullptr);
    // переконуємось, що getPlayer повертає саме той самий об'єкт
    QCOMPARE(t->getPlayer(), static_cast<ITrackPlayer*>(player));
}

// =============================================================================
// ── Гучність ─────────────────────────────────────────────────────────────────
// =============================================================================

void TestTrack::testSetVolumePropagatedToPlayer() {
    MockTrackPlayer* player;
    QScopedPointer<Track> t(makeTrack(player));
    t->setVolume(0.5f);
    QCOMPARE(player->volume, 0.5f);
}

void TestTrack::testVolumeBoundaryZero() {
    MockTrackPlayer* player;
    QScopedPointer<Track> t(makeTrack(player));
    t->setVolume(0.0f);
    QCOMPARE(player->volume, 0.0f);
}

void TestTrack::testVolumeBoundaryOne() {
    MockTrackPlayer* player;
    QScopedPointer<Track> t(makeTrack(player));
    t->setVolume(1.0f);
    QCOMPARE(player->volume, 1.0f);
}

void TestTrack::testGetVolumeReturnsPlayerValue() {
    MockTrackPlayer* player;
    QScopedPointer<Track> t(makeTrack(player));
    player->volume = 0.33f;             // встановлюємо напряму у mock
    QCOMPARE(t->getVolume(), 0.33f);    // Track::getVolume() читає з плеєра
}

// =============================================================================
// ── Аудіо-шлях ───────────────────────────────────────────────────────────────
// =============================================================================

void TestTrack::testSetAudioPathPropagatedToPlayer() {
    MockTrackPlayer* player;
    QScopedPointer<Track> t(makeTrack(player));
    t->setAudioSamplePath("/drums/kick.wav");
    QCOMPARE(player->loaded_path, QString("/drums/kick.wav"));
}

void TestTrack::testSetAudioPathMarksFileLoaded() {
    MockTrackPlayer* player;
    QScopedPointer<Track> t(makeTrack(player));
    t->setAudioSamplePath("/drums/snare.wav");
    QVERIFY(player->file_loaded);
}

void TestTrack::testEmptyPathDoesNotMarkFileLoaded() {
    MockTrackPlayer* player;
    QScopedPointer<Track> t(makeTrack(player));
    // порожній шлях — Track::setAudioSamplePath() має не викликати loadAudioFileForPlaying
    t->setAudioSamplePath("");
    QVERIFY(!player->file_loaded);
}

void TestTrack::testGetAudioSamplePath() {
    MockTrackPlayer* player;
    QScopedPointer<Track> t(makeTrack(player));
    t->setAudioSamplePath("/hi-hat.wav");
    QCOMPARE(t->getAudioSamplePath(), QString("/hi-hat.wav"));
}

void TestTrack::testGetSoundPath() {
    MockTrackPlayer* player;
    QScopedPointer<Track> t(makeTrack(player));
    t->setAudioSamplePath("/clap.wav");
    // getSoundPath() — синонім getAudioSamplePath()
    QCOMPARE(t->getSoundPath(), QString("/clap.wav"));
}

// =============================================================================
// ── Луп ──────────────────────────────────────────────────────────────────────
// =============================================================================

void TestTrack::testSetLoopStateTrue() {
    MockTrackPlayer* player;
    QScopedPointer<Track> t(makeTrack(player));
    t->setLoopState(true);
    QVERIFY(t->getLoopState());
}

void TestTrack::testSetLoopStateFalse() {
    MockTrackPlayer* player;
    QScopedPointer<Track> t(makeTrack(player, nullptr, 1.0f, true));
    t->setLoopState(false);
    QVERIFY(!t->getLoopState());
}

void TestTrack::testGetLoopStateDefault() {
    MockTrackPlayer* player;
    QScopedPointer<Track> t(makeTrack(player));
    QVERIFY(!t->getLoopState());
}

// =============================================================================
// ── Запис ─────────────────────────────────────────────────────────────────────
// =============================================================================

void TestTrack::testSetRecordingStatePropagatedToPlayer() {
    MockTrackPlayer* player;
    QScopedPointer<Track> t(makeTrack(player));
    t->setRecordingState(true);
    QVERIFY(player->recording_enabled);
}

void TestTrack::testGetRecordingStateDefault() {
    MockTrackPlayer* player;
    QScopedPointer<Track> t(makeTrack(player));
    QVERIFY(!t->getRecordingState());
}

void TestTrack::testRecordingStateToggle() {
    MockTrackPlayer* player;
    QScopedPointer<Track> t(makeTrack(player));
    t->setRecordingState(true);
    QVERIFY(t->getRecordingState());
    QVERIFY(player->recording_enabled);
    t->setRecordingState(false);
    QVERIFY(!t->getRecordingState());
    QVERIFY(!player->recording_enabled);
}

// =============================================================================
// ── Кольори ───────────────────────────────────────────────────────────────────
// =============================================================================

void TestTrack::testSetOuterColor() {
    MockTrackPlayer* player;
    QScopedPointer<Track> t(makeTrack(player));
    t->setOuterBackgroundColor(Qt::blue);
    QCOMPARE(t->getOuterBackgroundColor(), QColor(Qt::blue));
}

void TestTrack::testSetInnerColor() {
    MockTrackPlayer* player;
    QScopedPointer<Track> t(makeTrack(player));
    t->setInnerActiveBackgroundColor(Qt::green);
    QCOMPARE(t->getInnerActiveBackgroundColor(), QColor(Qt::green));
}

// =============================================================================
// ── Біти ──────────────────────────────────────────────────────────────────────
// =============================================================================

void TestTrack::testGetBeatsStatesReturnsInitialPattern() {
    MockTrackPlayer* player;
    std::array<bool,16> pattern = {1,0,1,0, 1,0,1,0, 0,0,0,0, 0,0,0,0};
    QScopedPointer<Track> t(makeTrack(player, nullptr, 1.0f, false, false, pattern));
    auto beats = t->getBeatsStates();
    for (int i = 0; i < 16; ++i)
        QCOMPARE(beats[i], pattern[i]);
}

void TestTrack::testSetBeatStateTrue() {
    MockTrackPlayer* player;
    QScopedPointer<Track> t(makeTrack(player));
    t->setBeatState(5, true);
    QVERIFY(t->getBeatsStates()[5]);
}

void TestTrack::testSetBeatStateFalse() {
    MockTrackPlayer* player;
    std::array<bool,16> all_on;
    all_on.fill(true);
    QScopedPointer<Track> t(makeTrack(player, nullptr, 1.0f, false, false, all_on));
    t->setBeatState(5, false);
    QVERIFY(!t->getBeatsStates()[5]);
}

void TestTrack::testAllBeatsCanBeSet() {
    MockTrackPlayer* player;
    QScopedPointer<Track> t(makeTrack(player));
    for (quint8 i = 0; i < 16; ++i)
        t->setBeatState(i, true);
    auto beats = t->getBeatsStates();
    for (int i = 0; i < 16; ++i)
        QVERIFY(beats[i]);
}

// =============================================================================
// ── Сигнал rightClicked ───────────────────────────────────────────────────────
// =============================================================================

void TestTrack::testRightClickEmitsSignalWithPointer() {
    MockTrackPlayer* player;
    QScopedPointer<Track> t(makeTrack(player));
    t->resize(100, 50);
    t->show();   // потрібно для mouse events
    QSignalSpy spy(t.data(), &Track::rightClicked);
    QTest::mouseClick(t.data(), Qt::RightButton);
    QCOMPARE(spy.count(), 1);
    // перевіряємо, що сигнал містить вказівник на сам трек
    QCOMPARE(spy.takeFirst().at(0).value<Track*>(), t.data());
}

// =============================================================================
// ── Ефекти — тип ──────────────────────────────────────────────────────────────
// =============================================================================

void TestTrack::testSetEffectTypeReverb() {
    MockTrackPlayer* player;
    QScopedPointer<Track> t(makeTrack(player));
    t->setCurrentEffectType(EffectType::Reverb);
    QCOMPARE(player->effect_type, EffectType::Reverb);
    QCOMPARE(t->getEffectType(),  EffectType::Reverb);
}

void TestTrack::testSetEffectTypeDelay() {
    MockTrackPlayer* player;
    QScopedPointer<Track> t(makeTrack(player));
    t->setCurrentEffectType(EffectType::Delay);
    QCOMPARE(player->effect_type, EffectType::Delay);
}

void TestTrack::testSetEffectTypeChorus() {
    MockTrackPlayer* player;
    QScopedPointer<Track> t(makeTrack(player));
    t->setCurrentEffectType(EffectType::Chorus);
    QCOMPARE(player->effect_type, EffectType::Chorus);
}

void TestTrack::testSetEffectTypeDistortion() {
    MockTrackPlayer* player;
    QScopedPointer<Track> t(makeTrack(player));
    t->setCurrentEffectType(EffectType::Distortion);
    QCOMPARE(player->effect_type, EffectType::Distortion);
}

void TestTrack::testSetEffectTypeNone() {
    MockTrackPlayer* player;
    QScopedPointer<Track> t(makeTrack(player));
    t->setCurrentEffectType(EffectType::Reverb);
    t->setCurrentEffectType(EffectType::None);
    QCOMPARE(player->effect_type, EffectType::None);
}

// =============================================================================
// ── Reverb ────────────────────────────────────────────────────────────────────
// =============================================================================

void TestTrack::testReverbRoomSizeMinBoundary() {
    MockTrackPlayer* player;
    QScopedPointer<Track> t(makeTrack(player));
    t->setReverbRoomSize(0.0f);
    QCOMPARE(player->reverb_settings.roomSize, 0.0f);
}

void TestTrack::testReverbRoomSizeMaxBoundary() {
    MockTrackPlayer* player;
    QScopedPointer<Track> t(makeTrack(player));
    t->setReverbRoomSize(1.0f);
    QCOMPARE(player->reverb_settings.roomSize, 1.0f);
}

void TestTrack::testReverbRoomSizeOutOfRangeIgnored() {
    MockTrackPlayer* player;
    QScopedPointer<Track> t(makeTrack(player));
    t->setReverbRoomSize(0.5f);
    t->setReverbRoomSize(1.5f);   // поза діапазоном [0,1] — Track ігнорує
    QCOMPARE(player->reverb_settings.roomSize, 0.5f);
}

void TestTrack::testReverbDampingBoundaries() {
    MockTrackPlayer* player;
    QScopedPointer<Track> t(makeTrack(player));
    t->setReverbDamping(0.0f);
    QCOMPARE(player->reverb_settings.damping, 0.0f);
    t->setReverbDamping(1.0f);
    QCOMPARE(player->reverb_settings.damping, 1.0f);
}

void TestTrack::testReverbWetLevelBoundaries() {
    MockTrackPlayer* player;
    QScopedPointer<Track> t(makeTrack(player));
    t->setReverbWetLevel(0.0f);
    QCOMPARE(player->reverb_settings.wetLevel, 0.0f);
    t->setReverbWetLevel(1.0f);
    QCOMPARE(player->reverb_settings.wetLevel, 1.0f);
}

void TestTrack::testReverbDryLevelBoundaries() {
    MockTrackPlayer* player;
    QScopedPointer<Track> t(makeTrack(player));
    t->setReverbDryLevel(0.0f);
    QCOMPARE(player->reverb_settings.dryLevel, 0.0f);
    t->setReverbDryLevel(1.0f);
    QCOMPARE(player->reverb_settings.dryLevel, 1.0f);
}

void TestTrack::testReverbOutputVolumeBoundaries() {
    MockTrackPlayer* player;
    QScopedPointer<Track> t(makeTrack(player));
    t->setReverbOutputVolume(0.0f);
    QCOMPARE(player->reverb_settings.outputVolume, 0.0f);
    t->setReverbOutputVolume(1.0f);
    QCOMPARE(player->reverb_settings.outputVolume, 1.0f);
}

// =============================================================================
// ── Delay ─────────────────────────────────────────────────────────────────────
// =============================================================================

void TestTrack::testDelayTimeBoundaries() {
    MockTrackPlayer* player;
    QScopedPointer<Track> t(makeTrack(player));
    t->setDelayTime(0.0f);
    QCOMPARE(player->delay_settings.delayTime, 0.0f);
    t->setDelayTime(1.0f);
    QCOMPARE(player->delay_settings.delayTime, 1.0f);
}

void TestTrack::testDelayFeedbackMaxBoundary() {
    MockTrackPlayer* player;
    QScopedPointer<Track> t(makeTrack(player));
    t->setDelayFeedback(0.9f);   // максимум дозволений Track.cpp
    QCOMPARE(player->delay_settings.feedback, 0.9f);
}

void TestTrack::testDelayFeedbackOutOfRangeIgnored() {
    MockTrackPlayer* player;
    QScopedPointer<Track> t(makeTrack(player));
    t->setDelayFeedback(0.5f);
    t->setDelayFeedback(0.95f);  // > 0.9 — Track ігнорує
    QCOMPARE(player->delay_settings.feedback, 0.5f);
}

void TestTrack::testDelayMixLevelBoundaries() {
    MockTrackPlayer* player;
    QScopedPointer<Track> t(makeTrack(player));
    t->setDelayMixLevel(0.0f);
    QCOMPARE(player->delay_settings.mix, 0.0f);
    t->setDelayMixLevel(1.0f);
    QCOMPARE(player->delay_settings.mix, 1.0f);
}

void TestTrack::testDelayOutputVolumeBoundaries() {
    MockTrackPlayer* player;
    QScopedPointer<Track> t(makeTrack(player));
    t->setDelayOutputVolume(0.0f);
    QCOMPARE(player->delay_settings.outputVolume, 0.0f);
    t->setDelayOutputVolume(1.0f);
    QCOMPARE(player->delay_settings.outputVolume, 1.0f);
}

// =============================================================================
// ── Chorus ────────────────────────────────────────────────────────────────────
// =============================================================================

void TestTrack::testChorusRateMinBoundary() {
    MockTrackPlayer* player;
    QScopedPointer<Track> t(makeTrack(player));
    t->setChorusRate(0.1f);   // мінімум Track.cpp
    QCOMPARE(player->chorus_settings.rate, 0.1f);
}

void TestTrack::testChorusRateMaxBoundary() {
    MockTrackPlayer* player;
    QScopedPointer<Track> t(makeTrack(player));
    t->setChorusRate(10.0f);
    QCOMPARE(player->chorus_settings.rate, 10.0f);
}

void TestTrack::testChorusRateOutOfRangeIgnored() {
    MockTrackPlayer* player;
    QScopedPointer<Track> t(makeTrack(player));
    t->setChorusRate(5.0f);
    t->setChorusRate(0.0f);   // < 0.1 — Track ігнорує
    QCOMPARE(player->chorus_settings.rate, 5.0f);
    t->setChorusRate(11.0f);  // > 10.0 — Track ігнорує
    QCOMPARE(player->chorus_settings.rate, 5.0f);
}

void TestTrack::testChorusDepthBoundaries() {
    MockTrackPlayer* player;
    QScopedPointer<Track> t(makeTrack(player));
    t->setChorusDepth(0.0f);
    QCOMPARE(player->chorus_settings.depth, 0.0f);
    t->setChorusDepth(1.0f);
    QCOMPARE(player->chorus_settings.depth, 1.0f);
}

void TestTrack::testChorusCenterDelayBoundaries() {
    MockTrackPlayer* player;
    QScopedPointer<Track> t(makeTrack(player));
    t->setChorusCenterDelay(1.0f);
    QCOMPARE(player->chorus_settings.centerDelay, 1.0f);
    t->setChorusCenterDelay(50.0f);
    QCOMPARE(player->chorus_settings.centerDelay, 50.0f);
}

void TestTrack::testChorusFeedbackMaxBoundary() {
    MockTrackPlayer* player;
    QScopedPointer<Track> t(makeTrack(player));
    t->setChorusFeedback(0.5f);
    QCOMPARE(player->chorus_settings.feedback, 0.5f);
}

void TestTrack::testChorusFeedbackOutOfRangeIgnored() {
    MockTrackPlayer* player;
    QScopedPointer<Track> t(makeTrack(player));
    t->setChorusFeedback(0.3f);
    t->setChorusFeedback(0.6f);   // > 0.5 — Track ігнорує
    QCOMPARE(player->chorus_settings.feedback, 0.3f);
}

void TestTrack::testChorusMixBoundaries() {
    MockTrackPlayer* player;
    QScopedPointer<Track> t(makeTrack(player));
    t->setChorusMix(0.0f);
    QCOMPARE(player->chorus_settings.mix, 0.0f);
    t->setChorusMix(1.0f);
    QCOMPARE(player->chorus_settings.mix, 1.0f);
}

void TestTrack::testChorusOutputVolumeBoundaries() {
    MockTrackPlayer* player;
    QScopedPointer<Track> t(makeTrack(player));
    t->setChorusOutputVolume(0.0f);
    QCOMPARE(player->chorus_settings.outputVolume, 0.0f);
    t->setChorusOutputVolume(1.0f);
    QCOMPARE(player->chorus_settings.outputVolume, 1.0f);
}

// =============================================================================
// ── Distortion ────────────────────────────────────────────────────────────────
// =============================================================================

void TestTrack::testDistortionDriveMinBoundary() {
    MockTrackPlayer* player;
    QScopedPointer<Track> t(makeTrack(player));
    t->setDistortionDrive(1.0f);   // мінімум Track.cpp
    QCOMPARE(player->distortion_settings.drive, 1.0f);
}

void TestTrack::testDistortionDriveMaxBoundary() {
    MockTrackPlayer* player;
    QScopedPointer<Track> t(makeTrack(player));
    t->setDistortionDrive(100.0f);
    QCOMPARE(player->distortion_settings.drive, 100.0f);
}

void TestTrack::testDistortionDriveOutOfRangeIgnored() {
    MockTrackPlayer* player;
    QScopedPointer<Track> t(makeTrack(player));
    t->setDistortionDrive(50.0f);
    t->setDistortionDrive(0.5f);    // < 1.0 — Track ігнорує
    QCOMPARE(player->distortion_settings.drive, 50.0f);
    t->setDistortionDrive(101.0f);  // > 100 — Track ігнорує
    QCOMPARE(player->distortion_settings.drive, 50.0f);
}

void TestTrack::testDistortionMixBoundaries() {
    MockTrackPlayer* player;
    QScopedPointer<Track> t(makeTrack(player));
    t->setDistortionMix(0.0f);
    QCOMPARE(player->distortion_settings.mix, 0.0f);
    t->setDistortionMix(1.0f);
    QCOMPARE(player->distortion_settings.mix, 1.0f);
}

void TestTrack::testDistortionOutputVolumeBoundaries() {
    MockTrackPlayer* player;
    QScopedPointer<Track> t(makeTrack(player));
    t->setDistortionOutputVolume(0.0f);
    QCOMPARE(player->distortion_settings.outputVolume, 0.0f);
    t->setDistortionOutputVolume(1.0f);
    QCOMPARE(player->distortion_settings.outputVolume, 1.0f);
}

// =============================================================================
QTEST_MAIN(TestTrack)
#include "test_track.moc"
