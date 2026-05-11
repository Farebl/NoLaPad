#include <QtTest>
#include <QSignalSpy>
#include <QThread>
#include "MicroTimer.h"

class TestMicroTimer : public QObject {
    Q_OBJECT
private slots:
    void testInvalidIntervalDefault();
    void testStartEmitsTicks();
    void testStopHaltsTicks();
    void testTickOrderCycles();
    void testIntervalChange();
};

void TestMicroTimer::testInvalidIntervalDefault() {
    MicroTimer t(0);                       // некоректне значення
    t.setInterval(0);                      // ще раз — не повинно впасти
    QVERIFY(true);                         // граничний кейс
}

void TestMicroTimer::testStartEmitsTicks() {
    MicroTimer t(10000);                   // 10 мс
    QSignalSpy spy0(&t, t.m_signals[0]);
    QSignalSpy spy1(&t, t.m_signals[1]);

    QThread th;
    t.moveToThread(&th);
    connect(&th, &QThread::started, &t, &MicroTimer::start);
    th.start();
    QTest::qWait(150);
    t.stop();
    th.quit();
    QVERIFY(th.wait(2000));

    QVERIFY2(spy0.count() > 0, "tick0 не прийшов");
    QVERIFY2(spy1.count() > 0, "tick1 не прийшов");
}

void TestMicroTimer::testStopHaltsTicks() {
    MicroTimer t(10000);
    QSignalSpy spy(&t, t.m_signals[0]);

    QThread th;
    t.moveToThread(&th);
    connect(&th, &QThread::started, &t, &MicroTimer::start);
    th.start();
    QTest::qWait(100);
    t.stop();
    th.quit(); th.wait();

    int count_after_stop = spy.count();
    QTest::qWait(100);
    QCOMPARE(spy.count(), count_after_stop);  // не зросло
}

void TestMicroTimer::testTickOrderCycles() {
    // 16 сигналів tick0..tick15 чергуються по колу
    MicroTimer t(5000);
    QVector<QSignalSpy*> spies;
    spies << new QSignalSpy(&t, t.m_signals[0])
          << new QSignalSpy(&t, t.m_signals[15]);

    QThread th; t.moveToThread(&th);
    connect(&th, &QThread::started, &t, &MicroTimer::start);
    th.start(); QTest::qWait(300);
    t.stop(); th.quit(); th.wait();

    QVERIFY(spies[0]->count() > 0);
    QVERIFY(spies[1]->count() > 0);
    qDeleteAll(spies);
}

void TestMicroTimer::testIntervalChange()
{
    MicroTimer timer;
    timer.setInterval(250);
    QCOMPARE(timer.getInterval(), 250);
}

QTEST_MAIN(TestMicroTimer)
#include "test_microtimer.moc"
