#ifndef IAUDIOENGINE_H
#define IAUDIOENGINE_H

class IPlayer;
class IRecorder;

class IAudioEngine
{
public:
    virtual bool initialize() = 0;
    virtual void shutdown() = 0;
    virtual void addTrack(IPlayer* audio_player) = 0;
    virtual void removeTrack(IPlayer* audio_player) = 0;
    virtual void setRecorder(IRecorder* audio_recorder) = 0;
    virtual void startRecording() = 0;
    virtual void stopRecording() = 0;
    virtual bool isRecording() = 0;
};

#endif // IAUDIOENGINE_H
