#include <QtTest>
#include <QTemporaryDir>
#include <QFileInfo>
#include <vector>
#include <cmath>

#include "JUCERecorder.h"
#include "AudioBlockInfo.h"

class TestJUCERecorder : public QObject {
    Q_OBJECT
private:
    QTemporaryDir tmp;

private slots:
    void initTestCase() {
        QVERIFY(tmp.isValid());
    }

    // ---- Стан "isRecording" ----

    void testInitiallyNotRecording() {
        JUCERecorder rec;
        QVERIFY(!rec.isRecording());
    }

    void testStartSetsRecordingFlag() {
        JUCERecorder rec;
        rec.startRecording(tmp.path() + "/start.wav");
        QVERIFY(rec.isRecording());
        rec.stopRecording();
    }

    void testStopClearsRecordingFlag() {
        JUCERecorder rec;
        rec.startRecording(tmp.path() + "/stop.wav");
        rec.stopRecording();
        QVERIFY(!rec.isRecording());
    }

    // ---- Захист від некоректного використання ----

    void testStopWithoutStartIsSafe() {
        JUCERecorder rec;
        rec.stopRecording();
        QVERIFY(!rec.isRecording());
    }

    void testWriteChunkBeforeStartIsSafe() {
        JUCERecorder rec;
        const int N = 256;
        std::vector<float> ch0(N, 0.5f), ch1(N, -0.5f);
        float* channels[2] = { ch0.data(), ch1.data() };
        AudioBlockInfo block;
        block.data = channels;
        block.numSamples = N;
        rec.writeChunk(&block);
        QVERIFY(!rec.isRecording());
    }

    void testStartWithInvalidPathDoesNotCrash() {
        JUCERecorder rec;
        // неіснуюча директорія → createOutputStream поверне null/failed
        rec.startRecording("/this/path/definitely/does/not/exist/x.wav");
        // не падаємо; isRecording лишається false
        QVERIFY(!rec.isRecording());
    }

    // ---- Ключовий тест: запис реального WAV-файлу ----

    void testRecordingProducesValidWavFile() {
        const QString path = tmp.path() + "/record.wav";
        {
            JUCERecorder rec;
            rec.startRecording(path);
            QVERIFY(rec.isRecording());

            const int N = 512;
            const int blocksToWrite = 200; // ~2.32 с @ 44100 Hz, 2 ch
            std::vector<float> ch0(N), ch1(N);
            float* channels[2] = { ch0.data(), ch1.data() };
            AudioBlockInfo block;
            block.data = channels;
            block.numSamples = N;

            // Генеруємо синусоїду 440 Hz і подаємо у recorder
            for (int b = 0; b < blocksToWrite; ++b) {
                for (int i = 0; i < N; ++i) {
                    const float t = float(b * N + i) / 44100.0f;
                    const float s = 0.3f * std::sin(2.0f * 3.14159265f * 440.0f * t);
                    ch0[i] = s;
                    ch1[i] = s;
                }
                rec.writeChunk(&block);
            }

            rec.stopRecording();
            QVERIFY(!rec.isRecording());
        } // ~JUCERecorder() гарантує дочекання writer-thread

        QFileInfo fi(path);
        QVERIFY2(fi.exists(),
                 qPrintable("WAV file not created: " + path));

        // 200 блоків × 512 семплів × 2 канали × 2 байти (16-bit) = 409600 байт даних
        // + WAV header ~44 байти. Перевіряємо хоча б половину очікуваного,
        // щоб уникнути флакі-тестів через round-trip FIFO.
        const qint64 expectedMin = qint64(200) * 512 * 2 * 2 / 2;
        QVERIFY2(fi.size() >= expectedMin,
                 qPrintable(QString("WAV file too small: %1 bytes (expected >= %2)")
                                .arg(fi.size()).arg(expectedMin)));
    }

    // ---- Restart має закривати попередній файл і відкривати новий ----

    void testRestartCreatesBothFiles() {
        JUCERecorder rec;
        const QString path1 = tmp.path() + "/first.wav";
        const QString path2 = tmp.path() + "/second.wav";

        rec.startRecording(path1);
        QVERIFY(rec.isRecording());
        rec.startRecording(path2);   // повинен викликати stopRecording для first
        QVERIFY(rec.isRecording());
        rec.stopRecording();
        QVERIFY(!rec.isRecording());

        QVERIFY(QFileInfo::exists(path1));
        QVERIFY(QFileInfo::exists(path2));
    }
};

QTEST_MAIN(TestJUCERecorder)
#include "test_jucerecorder.moc"
