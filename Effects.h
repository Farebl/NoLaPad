#ifndef EFFECTS_H
#define EFFECTS_H

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


#endif // EFFECTS_H
