#ifndef AUDIOSAMPLESELECTOR_H
#define AUDIOSAMPLESELECTOR_H

#include <QWidget>
#include <QPixmap>
#include <QMouseEvent>
#include <QPainter>
#include <QTimer>
#include <QApplication>

class AudioSampleSelector : public QWidget
{
    Q_OBJECT
private:
    QPixmap m_active_state_image;
    QPixmap m_inactive_state_image;
    bool m_is_active; // Ініціалізуємо як false

    static AudioSampleSelector* m_instance;
    explicit AudioSampleSelector(QString active_state_image_path, QString inactive_state_image_path, QWidget *parent);

protected:
    void mousePressEvent(QMouseEvent *event) override;
    void paintEvent(QPaintEvent *event) override;

public:
    static AudioSampleSelector* getInstance(QString active_state_image_path = "..//..//images//audio_in_plugged.png", QString inactive_state_image_path  = "..//..//images//audio_in_unplugged.png", QWidget *parent = nullptr);
    void setIsAudioSampleSelectedState(bool state);

signals:
    void selectedAudioSample(const QString &audioSamplePath);
};

#endif // AUDIOSAMPLESELECTOR_H
