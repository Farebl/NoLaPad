#include <QtTest>
#include <QSignalSpy>
#include "ProjectManager.h"
#include "MockStorage.h"
#include "MockAudioEngine.h"
#include "MockTrackPlayer.h"

class TestProjectManager : public QObject {
    Q_OBJECT
private slots:
    void testCreateNewProjectAddsView();
    void testSaveProjectUpdatesExistingView();
    void testSaveFailureShowsError();
    void testReopenProjectDoesNotLeakPlayers();
};

static auto makeMockFactory() {
    return []() -> ITrackPlayer* { return new MockTrackPlayer(); };
}

void TestProjectManager::testCreateNewProjectAddsView() {

    auto* storage = new MockStorage();
    auto* engine  = new MockAudioEngine();

    ProjectManager pm(storage, engine, makeMockFactory(), nullptr);
    int initial = storage->stored_views.size();

    pm.testHook_createProject("NewOne");
    pm.testHook_save();

    QCOMPARE(storage->save_project_calls, 1);
    QCOMPARE(storage->save_view_calls,    1);
    QCOMPARE(storage->stored_views.size(), initial + 1);
}

void TestProjectManager::testSaveProjectUpdatesExistingView() {
    auto* storage = new MockStorage();
    auto* engine  = new MockAudioEngine();
    ProjectManager pm(storage, engine, makeMockFactory(), nullptr);

    pm.testHook_createProject("X");
    pm.testHook_save();
    int views_after_first = storage->stored_views.size();
    pm.testHook_save();                            // повторне збереження ТОГО Ж проєкту
    QCOMPARE(storage->stored_views.size(), views_after_first);  // не додало нового
    QCOMPARE(storage->save_project_calls, 2);
}

void TestProjectManager::testSaveFailureShowsError() {
    auto* storage = new MockStorage();
    storage->fail_next_save = true;
    auto* engine = new MockAudioEngine();
    ProjectManager pm(storage, engine, makeMockFactory(), nullptr);
    pm.testHook_createProject("X");
    pm.testHook_save();                            // має не впасти, не додати view
    QCOMPARE(storage->stored_views.size(), 0);
}

void TestProjectManager::testReopenProjectDoesNotLeakPlayers() {
    auto* storage = new MockStorage();
    auto* engine  = new MockAudioEngine();
    ProjectManager pm(storage, engine, makeMockFactory(), nullptr);

    for (int i = 0; i < 5; ++i) {
        pm.testHook_loadProject("/fake/path");     // використовує MockStorage::getProjectData
        pm.testHook_closeProject();
    }
    // Метроном — 1 плеєр; усі треки після закриття мають бути зняті
    QCOMPARE(engine->players.size(), 1);
}


QTEST_MAIN(TestProjectManager)
#include "test_projectmanager.moc"
