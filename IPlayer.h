#ifndef IPLAYER_H
#define IPLAYER_H

#include <QString>

struct AudioBlockInfo {
    float** data;       // масив вказівників на канали: data[0]=L, data[1]=R
    int     numChannels;
    int     numSamples;
};

enum class EffectType { None, Reverb, Chorus, Delay, Distortion };
struct ReverbSettings {
    float roomSize    = 0.5f;
    float damping     = 0.5f;
    float wetLevel    = 0.33f;
    float dryLevel    = 0.4f;
    float outputVolume = 1.0f;
};
struct DelaySettings {
    float delayTime   = 0.2f;
    float feedback    = 0.2f;
    float mix         = 0.5f;
    float outputVolume = 1.0f;
};
struct ChorusSettings {
    float rate         = 1.0f;
    float depth        = 0.25f;
    float centerDelay  = 7.0f;
    float feedback     = 0.0f;
    float mix          = 0.5f;
    float outputVolume = 1.0f;
};
struct DistortionSettings {
    float drive        = 10.0f;
    float mix          = 0.5f;
    float outputVolume = 0.5f;
};


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

    virtual void loadFile(const QString& path) = 0;
    virtual void setVolume(float volume) = 0;
    virtual void setLoopEnabled(bool loop) = 0;

    virtual void setRecordingEnabled(bool enabled) = 0;
    virtual bool isRecordingEnabled() const = 0;

    virtual void setEffectType(EffectType type) = 0;
    virtual void setReverbSettings(const ReverbSettings& s) = 0;
    virtual void setDelaySettings(const DelaySettings& s) = 0;
    virtual void setChorusSettings(const ChorusSettings& s) = 0;
    virtual void setDistortionSettings(const DistortionSettings& s) = 0;
};

#endif // IPLAYER_H
