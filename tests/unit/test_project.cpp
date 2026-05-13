#include <QtTest>
#include <QSignalSpy>
#include "Project.h"
#include "MockAudioEngine.h"
#include "MockTrackPlayer.h"
#include "MicroTimer.h"
#include "Metronome.h"
#include "TrackSettings.h"
#include "RecorderButton.h"

class TestProject : public QObject {
    Q_OBJECT
private slots:
    void testCreationFromSaveData_AddsAllTracks();
    void testTracksRegisteredOnce();      // регрес-тест на подвійне addPlayer
    void testDestructorRemovesPlayers();
    void testRecordingButtonStartsEngine();
};

static auto makeMockFactory() {
    return []() -> ITrackPlayer* { return new MockTrackPlayer(); };
}

static ProjectSaveParameters make4x4() {
    ProjectSaveParameters d;
    d.name                  = "P";
    d.description           = "test";
    d.project_save_dir_path = QDir::tempPath();
    d.records_save_dir_path = QDir::tempPath();
    d.rows_count            = 4;
    d.columns_count         = 4;
    
    d.metronome_info.bpm    = 120;
    d.metronome_info.volume = 1.0;
    // якщо у MetronomeInfo є шляхи до семплів — задай порожні рядки явно
    
    d.tracks_info.resize(16);
    for (auto& t : d.tracks_info) {
        t.volume            = 0.7f;
        t.is_loop           = false;
        t.is_recording      = false;
        t._16th_beats       = {1,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0};
        t.audio_sample_path = "";          // явно
        t.outer_color       = Qt::gray;    // ← валідний колір
        t.inner_color       = Qt::red;     // ← валідний колір
    }
    return d;
}

void TestProject::testCreationFromSaveData_AddsAllTracks() {
    MockAudioEngine eng;
    MicroTimer timer(1000);
    Metronome m(&timer, "", "", "", "", 1.0, 60, nullptr);
    TrackSettings ts(100, false, {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}, Qt::gray, Qt::darkGray);

    Project p(&eng, makeMockFactory(), &timer, &m, &ts, make4x4(), nullptr);
    QCOMPARE(eng.players.size(), 16);     // саме 16, не 32
}

void TestProject::testTracksRegisteredOnce() {
    // Регресія для багу подвійного addPlayer у конструкторі з ProjectSaveParameters
    MockAudioEngine eng;
    MicroTimer timer(1000);
    Metronome m(&timer, "", "", "", "", 1.0, 60, nullptr);
    TrackSettings ts(100, false, {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}, Qt::gray, Qt::darkGray);

    Project p(&eng, makeMockFactory(), &timer, &m, &ts, make4x4(), nullptr);
    // унікальних плеєрів = 16
    QSet<IPlayer*> unique(eng.players.begin(), eng.players.end());
    QCOMPARE(unique.size(), 16);
    QCOMPARE(eng.players.size(), unique.size());  // дублікатів немає
}

void TestProject::testDestructorRemovesPlayers() {
    MockAudioEngine eng;
    MicroTimer timer(1000);
    Metronome m(&timer, "", "", "", "", 1.0, 60, nullptr);
    TrackSettings ts(100, false, {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}, Qt::gray, Qt::darkGray);

    {
        Project p(&eng, makeMockFactory(), &timer, &m, &ts, make4x4(), nullptr);
        QCOMPARE(eng.players.size(), 16);
    }
    QCOMPARE(eng.removed.size(), 16);     // усі 16 знято
    QCOMPARE(eng.players.size(), 0);
}

void TestProject::testRecordingButtonStartsEngine() {
    MockAudioEngine eng;
    MicroTimer timer(1000);
    Metronome m(&timer, "", "", "", "", 1.0, 60, nullptr);
    TrackSettings ts(100, false, {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}, Qt::gray, Qt::darkGray);

    Project p(&eng, makeMockFactory(), &timer, &m, &ts, make4x4(), nullptr);
    RecorderButton* REC_button = p.getRecordingButton();
    REC_button->toggled(true);
    QCOMPARE(eng.isRecording(), true);
    REC_button->toggled(false);
    QCOMPARE(eng.isRecording(), false);
}


QTEST_MAIN(TestProject)
#include "test_project.moc"
