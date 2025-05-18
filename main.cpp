#include "Project.h"
#include <QApplication>

#include <QMediaPlayer>
#include <QAudioOutput>
#include <QVBoxLayout>


#include "Track.h"
int main(int argc, char *argv[]) {
    QApplication a(argc, argv);
    Project w(8, 8, 60);
    w.show();
    // есть проблемы с настройкой тактов трека

    return a.exec();
}
