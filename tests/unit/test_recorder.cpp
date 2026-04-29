#include <QtTest>
#include <QTemporaryDir>
#include <QFileInfo>
#include "MockAudioEngine.h"

class TestRecorder : public QObject {
    Q_OBJECT
private:
    QTemporaryDir tmp;
private slots:
    void testStartRecordingMarksFlag();
    void testStopRecordingClearsFlag();
    void testRecordPathPropagated();
    void testStartStopCounters();
};

void TestRecorder::testStartRecordingMarksFlag() {
    MockAudioEngine eng;
    QString p = tmp.path() + "/r.wav";
    eng.startRecording(p);
    QVERIFY(eng.is_recording);
}

void TestRecorder::testStopRecordingClearsFlag() {
    MockAudioEngine eng;
    eng.startRecording(tmp.path() + "/r.wav");
    eng.stopRecording();
    QVERIFY(!eng.is_recording);
}

void TestRecorder::testRecordPathPropagated() {
    MockAudioEngine eng;
    QString p = tmp.path() + "/MyTrack.wav";
    eng.startRecording(p);
    QCOMPARE(eng.last_record_path, p);
}

void TestRecorder::testStartStopCounters() {
    MockAudioEngine eng;
    for (int i = 0; i < 5; ++i) {
        eng.startRecording(tmp.path() + QString("/r%1.wav").arg(i));
        eng.stopRecording();
    }
    QCOMPARE(eng.start_record_calls, 5);
    QCOMPARE(eng.stop_record_calls, 5);
}

// Інтеграційний тест із реальним JUCERecorder:
// - startRecording → 200 мс симуляції аудіо-callback → stopRecording
// - перевірити, що файл існує і має ненульовий розмір
QTEST_MAIN(TestRecorder)
#include "test_recorder.moc"
