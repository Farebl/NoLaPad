#include <QtTest>
#include <QTemporaryDir>
#include "JSONStorage.h"
#include "ProjectView.h"

extern const qsizetype ICON_SIZE = 450;

class TestJSONStorage : public QObject {
    Q_OBJECT
private:
    QTemporaryDir tmp;
private slots:
    void initTestCase();
    void testSaveAndLoadProject();
    void testSaveViewCreatesFile();
    void testLoadEmptyDirReturnsEmpty();
};

void TestJSONStorage::initTestCase() {
    QVERIFY(tmp.isValid());
}

void TestJSONStorage::testSaveAndLoadProject() {
    JSONStorage st(tmp.path(), "views.json", "PNG");
    ProjectSaveParameters data;
    data.name = "TestProj";
    data.description = "desc";
    data.project_save_dir_path = tmp.path();
    data.records_save_dir_path = tmp.path();
    data.rows_count = 2; data.columns_count = 2;
    data.tracks_info.resize(4);
    for (auto& t : data.tracks_info) {
        t.volume = 0.5; t.is_loop = true; t.is_recording = false;
        t._16th_beats = {1,0,1,0, 0,0,0,0, 0,0,0,0, 0,0,0,0};
        t.audio_sample_path = "/some/path.wav";
        t.outer_color = Qt::gray;                     
        t.inner_color = Qt::red;
    }
    data.metronome_info.bpm = 120;
    data.metronome_info.volume = 1.0;

    QString path = st.saveProject(data);
    QVERIFY(!path.isEmpty());
    QVERIFY(QFile::exists(path));

    auto loaded = st.getProjectData(path);
    QCOMPARE(loaded.name, QStringLiteral("TestProj"));
    QCOMPARE(loaded.rows_count, quint8(2));
    QCOMPARE(loaded.tracks_info.size(), 4);
    QCOMPARE(loaded.tracks_info[0]._16th_beats[0], 1);
    QCOMPARE(loaded.tracks_info[0]._16th_beats[2], 1);
}

void TestJSONStorage::testLoadEmptyDirReturnsEmpty() {
    QTemporaryDir empty;
    JSONStorage st(empty.path(), "views.json", "PNG");
    QCOMPARE(st.loadProjectsViews().size(), 0);
}


void TestJSONStorage::testSaveViewCreatesFile() {
    JSONStorage st(tmp.path(), "views.json", "PNG");

    // Створюємо тестову іконку
    QPixmap* icon = new QPixmap(32, 32);
    icon->fill(Qt::red);

    // Створюємо ProjectView
    ProjectView view(
        64,
        "MyProject",
        "/tmp/MyProject.json",
        "2026-04-28",
        "Test description",
        nullptr
    );

    view.setPreviewIcon(icon);

    // Зберігаємо
    QVERIFY(st.saveProjectView(&view));

    // Перевіряємо, що створився JSON файл
    QString jsonPath = tmp.path() + "/views.json";
    QVERIFY(QFile::exists(jsonPath));

    // Перевіряємо, що створилась preview-іконка
    QString iconPath = tmp.path() + "/MyProject.png";
    QVERIFY(QFile::exists(iconPath));

    // Перевіряємо, що запис можна прочитати назад
    auto views = st.loadProjectsViews();
    QCOMPARE(views.size(), 1);
    QCOMPARE(views[0]->getProjectName(), QString("MyProject"));
    QCOMPARE(views[0]->getPathToProject(), QString("/tmp/MyProject.json"));

    qDeleteAll(views);
}


QTEST_MAIN(TestJSONStorage)
#include "test_jsonstorage.moc"
