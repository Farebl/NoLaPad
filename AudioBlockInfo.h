#ifndef AUDIOBLOCKINFO_H
#define AUDIOBLOCKINFO_H

struct AudioBlockInfo {
    float** data;       // масив вказівників на канали: data[0]=L, data[1]=R
    int     numChannels;
    int     numSamples;
};

#endif // AUDIOBLOCKINFO_H
