//
//  audio.hpp
//  tinypiano
//
//  Created by zq on 2022/11/8.
//

#ifndef audio_hpp
#define audio_hpp

#include "ofxvsthost/ofxVstHost.h"

#include <stdio.h>
#include <AudioToolbox/AudioToolbox.h>

class AudioClient {
public:
    AudioClient();
    virtual ~AudioClient();
    
    void open();
    void close();
    
    int process(AudioUnitRenderActionFlags *ioActionFlags,
                const AudioTimeStamp *inTimeStamp,
                UInt32 inBusNumber,
                UInt32 inNumberFrames,
                AudioBufferList *ioData);

private:
    AudioUnit au_;
    AUGraph   au_graph_;
    AUNode    au_node_;
    
    ofxVstHost vst_host_;
    VstEvents *events_;
};

#endif /* audio_hpp */
