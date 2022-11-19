//
//  ofxVstHost.h
//
//
//  Created by Mathieu Hingant on 10/04/17.
//
//

#pragma once

//#include "ofMain.h"
#include "ofxVstHostPluginLoader.h"

#include <vector>

//TODO:
// - Close library function


//TODO: try to use this to know which is which!
struct effect_data {
    AEffect* effect;
    std::string label;
};


class ofxVstHost
{
public:
    ofxVstHost();
    ~ofxVstHost();
    
    
    // Main controls
    void setup(float sampleRate = 48000.f, int blockSize = 512);
    bool load(std::string path, std::string nameInfo = "");
    void closeEffect(int index);
    void closePlugin(int index);
    
    void process(int indexEffect, float *left, float *right, int frames);
    
    int getNumPlugins(void) const;
    int getNumEffects(void) const;
    float getSampleRate(void) const;
    VstInt32 getBlockSize(void) const;
    
    
    // Effect info
    int getNumInputs(int indexEffect) const;
    int getNumOutputs(int indexEffect) const;
    int getNumParameters(int indexEffect) const;
    int getNumPrograms(int indexEffect) const;
    std::string getVendorString(int indexEffect) const;
    std::string getProductString(int indexEffect) const;
    std::string getEffectName(int indexEffect) const;
    
    // Effect parameters
    std::string listParameterValues(int indexEffect);
    float getParameterValue(int indexEffect, int indexParam) const;
    void setParameterValue(int indexEffect, int indexParam, float value);
    std::string getParameterName(int indexEffect, int indexParam) const;
    std::string getParameterLabel(int indexEffect, int indexParam) const;

    void procEvents(VstEvents *events);
    
    
private:
    // Module is where to load the library
    std::vector<PluginLoader> _plugins;
    // Effect is one instance of this loaded library
    std::vector<AEffect* > _effects;
    // Each effect can be given a name to know which effect is which
    std::vector<std::string> _names;
    
    
    //--------------------------------------------------------------
    VstInt32 _blockSize;
    float _sampleRate;
    VstInt32 _numProcessCycles;
    
    // Out audio array used to process the audio
    float*  _outputs[64];
    float*  _inputs[64];
    
    float*  _empty;
    
    // Our sound buffer when processing the audio
//    ofSoundBuffer _rBuffer;
//    ofSoundBuffer _lBuffer;
    
    static bool checkPlatform();
    
    
    //--------------------------------------------------------------
    intptr_t dispatcher(AEffect* effect, int32_t opcode, int32_t index = 0, intptr_t value = 0, void *ptr = nullptr, float opt = 0.0f) const;
    
    static VstIntPtr VSTCALLBACK HostCallback (AEffect* effect, VstInt32 opcode, VstInt32 index, VstIntPtr value, void* ptr, float opt);
};
