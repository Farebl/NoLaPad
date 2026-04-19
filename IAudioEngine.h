#ifndef IAUDIOENGINE_H
#define IAUDIOENGINE_H

#include <QString>

class IPlayer;
class IRecorder;

class IAudioEngine
{
public:
    virtual void addPlayer(IPlayer* audio_player) = 0;
    virtual void removePlayer(IPlayer* audio_player) = 0;
    virtual void startRecording(const QString& outputPath) = 0;
    virtual void stopRecording() = 0;
    virtual bool isRecording() const = 0;
};

#endif // IAUDIOENGINE_H
