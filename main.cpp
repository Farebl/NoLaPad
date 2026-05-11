
#include "ProjectManager.h"
#include <QApplication>

int main(int argc, char *argv[]) {
    QApplication a(argc, argv);

    ProjectManager* project_manager = new ProjectManager();
    project_manager->show();

    int result = a.exec();
    delete project_manager;
    return result;
}

// RedButton:
    // RedButton тепер приймає ширину
    // connect-и з перемикачем ефектів --> тепр вониз захватують посилання зіамість значень
    // з назв методів змінено

// TrackSettings
    // рпибрано метод getLoopState
    // recording button type was changed to RedButton instead of QCheckBox

// ProjectSettings
    // було випарвлено помилку, при якій доводилось змінбвати ім'я проєкту для збережння

//LCDDispaly
    // прибрано FixedSize

//LCDCounter
    // тепер він не є QSpinBox. Тепер він наслідує QWidget
    // перероблено кнопки

// виправлено метроном
    //(він був зміщений на 1/16 вперед) Тепер норм
    // він тепер синхорізована с таймеро потійно

// track:
    //випарвлено встановлення 16-тих
    // тепер після сбросу аудфосесплу трек зупиняється
    // реалізовано затримку та тривалістьтреку







// додай обмежувачі в сетери і QSpinBox-и










