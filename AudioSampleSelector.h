#ifndef AUDIOSAMPLESELECTOR_H
#define AUDIOSAMPLESELECTOR_H

#include <QWidget>
#include <QPixmap>

class AudioSampleSelector : public QWidget
{
    Q_OBJECT
private:
    bool m_is_active; // Ініціалізуємо як false
    QString m_path_to_music;
    QPixmap m_active_state_image;
    QPixmap m_inactive_state_image;

protected:
    void mousePressEvent(QMouseEvent *event) override;
    void paintEvent(QPaintEvent *event) override;

public:
    explicit AudioSampleSelector(QString active_state_image_path, QString inactive_state_image_path, QWidget *parent = nullptr);
    void setIsAudioSampleSelectedState(bool state);

signals:
    void selectedAudioSample(const QString &audioSamplePath);
};

#endif // AUDIOSAMPLESELECTOR_H
