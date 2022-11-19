//
//  audio.cpp
//  tinypiano
//
//  Created by zq on 2022/11/8.
//

#include "audio.hpp"
#include "midievent.hpp"

#define DEFAULT_VST_PATH "/Library/Audio/Plug-Ins/VST/"
#define MAX_EVENT_NUM    256

OSStatus au__process(void *inRefCon,
                     AudioUnitRenderActionFlags *ioActionFlags,
                     const AudioTimeStamp *inTimeStamp,
                     UInt32 inBusNumber,
                     UInt32 inNumberFrames,
                     AudioBufferList *__nullable ioData) {
    auto *client = static_cast<AudioClient *>(inRefCon);
    return client->process(ioActionFlags, inTimeStamp, inBusNumber, inNumberFrames, ioData);
}

AudioClient::AudioClient() {
}

AudioClient::~AudioClient() {
    close();
}

void AudioClient::open() {
    std::string vst_path = DEFAULT_VST_PATH;
    vst_path += "TruePianos.vst";
    
    vst_host_.setup(44100, 512);
    vst_host_.load(vst_path);
    
    if (NewAUGraph(&au_graph_) != noErr) {
        printf("AudioClient::open > AUGraph failed.\n");
        return;
    }

    AudioComponentDescription component_desc;
    bzero(&component_desc, sizeof(component_desc));
    component_desc.componentType = kAudioUnitType_Output;
    component_desc.componentSubType = kAudioUnitSubType_HALOutput;
    component_desc.componentManufacturer = kAudioUnitManufacturer_Apple;

    if (AUGraphAddNode(au_graph_, &component_desc, &au_node_) != noErr) {
        printf("AudioClient::open > GraphAddNode failed.\n");
        return;
    }
    if (AUGraphOpen(au_graph_) != noErr) {
        printf("AudioClient::open > GraphOpen failed.\n");
        return;
    }
    if (AUGraphNodeInfo(au_graph_, au_node_, NULL, &au_) != noErr) {
        printf("AudioClient::open > GraphNodeInfo failed.\n");
        return;
    }

    AudioStreamBasicDescription streambasic_desc;
    streambasic_desc.mSampleRate = 44100.0;
    streambasic_desc.mFormatID = kAudioFormatLinearPCM;
    streambasic_desc.mFormatFlags = kAudioFormatFlagIsFloat | kAudioFormatFlagIsNonInterleaved;
    streambasic_desc.mBytesPerPacket = 4;
    streambasic_desc.mFramesPerPacket = 1;
    streambasic_desc.mBytesPerFrame = 4;
    streambasic_desc.mChannelsPerFrame = 2;
    streambasic_desc.mBitsPerChannel = 32;
    streambasic_desc.mReserved = 0;
    if (AudioUnitSetProperty(au_, kAudioUnitProperty_StreamFormat,
                             kAudioUnitScope_Input, 0, &streambasic_desc,
                             sizeof(AudioStreamBasicDescription)) != noErr) {
        printf("AudioClient::open > SetProperty(kAudioUnitProperty_StreamFormat) failed.\n");
        return;
    }

    AURenderCallbackStruct callback{
        au__process,
        static_cast<void *>(this),
    };
    if(AudioUnitSetProperty(au_, kAudioUnitProperty_SetRenderCallback,
                            kAudioUnitScope_Input, 0, &callback,
                            sizeof(AURenderCallbackStruct)) != noErr) {
        printf("AudioClient::open > SetProperty(kAudioUnitProperty_SetRenderCallback) failed.\n");
        return;
    }

    if(AUGraphInitialize(au_graph_) != noErr) {
        printf("AudioClient::open > GraphInitialize failed.\n");
        return;
    }

    if(AUGraphStart(au_graph_) != noErr) {
        printf("AudioClient::open > GraphStart failed.\n");
        return;
    }
}

void AudioClient::close() {
    AUGraphUninitialize(au_graph_);
    AUGraphClose(au_graph_);
    
    vst_host_.closeEffect(0);
    vst_host_.closePlugin(0);
}


int AudioClient::process(AudioUnitRenderActionFlags *ioActionFlags,
                         const AudioTimeStamp *inTimeStamp,
                         UInt32 inBusNumber,
                         UInt32 inNumberFrames,
                         AudioBufferList *ioData) {
    
    static struct MoreEvents : VstEvents {
      VstEvent *data[MAX_EVENT_NUM-2];
    } events;
    events.numEvents = 0;
    events.reserved = 0;
    
    midievent_read(&events, MAX_EVENT_NUM);
    vst_host_.procEvents(&events);
    
    float_t *left = static_cast<float_t *> (ioData->mBuffers[0].mData);
    float_t *right = static_cast<float_t *> (ioData->mBuffers[1].mData);
    vst_host_.process(0, left, right, inNumberFrames);
    
    return 0;
}
