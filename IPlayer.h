#ifndef IPLAYER_H
#define IPLAYER_H

#include <QStringList>


struct AudioBlockInfo;


class IPlayer {
public:
    virtual ~IPlayer() = default;

    virtual void prepareToPlay(int samplesPerBlock, double sampleRate) = 0;
    virtual void getNextAudioBlock(AudioBlockInfo* block) = 0;
    virtual void releaseResources() = 0;

    virtual void play() = 0;
    virtual void stop() = 0;
    virtual bool isPlaying() const = 0;
    virtual bool hasFileLoaded() const = 0;

    virtual void setVolume(float volume) = 0;
    virtual float getVolume() const = 0;

    virtual void setRecordingEnabled(bool enabled) = 0;
    virtual bool isRecordingEnabled() const = 0;
};

#endif // IPLAYER_H
