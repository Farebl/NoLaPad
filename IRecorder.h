#ifndef IRECORDER_H
#define IRECORDER_H

#include <QString>


struct AudioBlockInfo;

class IRecorder {
public:
    virtual ~IRecorder() = default;
    virtual void startRecording(const QString& path) = 0;
    virtual void stopRecording() = 0;
    virtual void writeChunk(AudioBlockInfo* block) = 0;
    virtual bool isRecording() const = 0;
};

#endif // IRECORDER_H
