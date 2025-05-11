#include "Project.h"
#include <QApplication>

#include <QMediaPlayer>
#include <QAudioOutput>
#include <QVBoxLayout>


#include "Track.h"
int main(int argc, char *argv[]) {
    QApplication a(argc, argv);
	// в классе TrackSettings храни beats_per_measure (кол-во красных квадратиков) что бы и было всегда больше чем 0
    Project w(8, 8, 60);
    w.show();

    return a.exec();
}
