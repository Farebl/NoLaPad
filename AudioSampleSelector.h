#ifndef AUDIOSAMPLESELECTOR_H
#define AUDIOSAMPLESELECTOR_H

#include <QWidget>
#include <QPixmap>

class QLabel;

class AudioSampleSelector : public QWidget
{
    Q_OBJECT
private:

    bool m_is_active;
    QString m_title;
    QString m_path_to_music;
    QPixmap m_active_state_image;
    QPixmap m_inactive_state_image;


protected:
    void mousePressEvent(QMouseEvent *event) override;
    void paintEvent(QPaintEvent *event) override;

public:
    explicit AudioSampleSelector(const QString& active_state_image_path, const QString& inactive_state_image_path, const QString& title, QWidget *parent = nullptr);
    void setIsAudioSampleSelectedState(bool state);

signals:
    void selectedAudioSample(const QString& audioSamplePath);
    void setTitle(const QString& title);
};

#endif // AUDIOSAMPLESELECTOR_H
