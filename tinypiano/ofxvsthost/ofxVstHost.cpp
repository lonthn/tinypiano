//
//  ofxVstHost.cpp
//  
//
//  Created by Mathieu Hingant on 10/04/17.
//
//

#include "ofxVstHost.h"

#include <iostream>

//--------------------------------------------------------------
ofxVstHost::ofxVstHost()
{
    _plugins.clear();
    _effects.clear();
    _names.clear();
    
    _blockSize = 512;
    _sampleRate = 44100.f;
    _numProcessCycles = 5;
}


//--------------------------------------------------------------
ofxVstHost::~ofxVstHost()
{
    // Close all effects
    for(int i = 0; i < _effects.size(); i++) {
        closeEffect(i);
    }
    
    // Close all plugins
    for(int i = 0; i < _plugins.size(); i++) {
        closePlugin(i);
    }
    
}


//--------------------------------------------------------------
void ofxVstHost::setup(float sampleRate, int blockSize)
{
    // Initialise our variables
    _sampleRate = sampleRate;
    _blockSize = blockSize;
    _numProcessCycles = 5;
    
    _empty = (float *) malloc(_blockSize * sizeof(float));
    for (int i = 0; i < 64; i++) {
        _outputs[i] = nullptr;
        _inputs[i] = nullptr;
    }
}


//--------------------------------------------------------------
bool ofxVstHost::load(std::string path, std::string nameInfo)
{
    std::cout << "ofxVstHost: " << "load(" << path << ")" << std::endl;
    
    if (!checkPlatform ())
    {
        std::cout << "ofxVstHost: " << "Platform verification failed! Please check your Compiler Settings!" << std::endl;
        return false;
    }
    
    // Check that this module has not been already loaded
    int indexExisting = -1;
    for(int i = 0; i < _plugins.size(); i++) {
        // If we find the same name in our vector means we already loaded this library
        if(_plugins[i].path == path) {
            indexExisting = i;
            std::cout << "ofxVstHost: " << "Library " << path << " already exist at index " << i << ". Not loading it again." << std::endl;
        }
    }
    
    
    // Load new library if it is not in our list already
    if(indexExisting == -1){
        PluginLoader loader;
        if (!loader.loadLibrary (path.c_str()))
        {
            std::cerr << "ofxVstHost: " << "Failed to load VST Plugin library!" << std::endl;
            return false;
        }
        
        // Add our plugin loader to our vector
        _plugins.push_back(loader);
        
        // Overwrite index as it will be used below to access proper plugin
        indexExisting = (int) _plugins.size() - 1;
    }
    
    
    // Get main entry from our plugin vector
    PluginEntryProc mainEntry = _plugins[indexExisting].getMainEntry ();
    if (!mainEntry)
    {
        std::cerr << "ofxVstHost: " << "VST Plugin main entry not found!" << std::endl;
        return false;
    }

    
    // Create the effect from this main entry
    AEffect* effect = mainEntry (HostCallback);
    
    if (!effect)
    {
        std::cerr << "ofxVstHost: " << "Failed to create effect instance!" << std::endl;
        return false;
    }
    
    // Add this effect to our vector
    _effects.push_back(effect);
    _names.push_back(nameInfo);
    
    // Get index of the effect in our vector
    int indexEffect = (int) _effects.size() - 1;
    
    // Initialize the effect
    dispatcher(_effects[indexEffect], effOpen);
    dispatcher(_effects[indexEffect], effBeginSetProgram, 0, NULL, 0, 0);
    dispatcher(_effects[indexEffect], effSetProgram, 0, 0, 0, 0);
    dispatcher(_effects[indexEffect], effEndSetProgram, 0, NULL, 0, 0);
    
    dispatcher(_effects[indexEffect], effMainsChanged, 0, 0);
    dispatcher(_effects[indexEffect], effSetSampleRate, 0, 0, 0, getSampleRate());
    dispatcher(_effects[indexEffect], effSetBlockSize, 0, getBlockSize());
    //dispatcher(_effects[indexEffect], effSetProcessPrecision, 0, kVstProcessPrecision32);
    dispatcher(_effects[indexEffect], effMainsChanged, 0, 1);
    //dispatcher(_effects[indexEffect], effStartProcess);
    return true;
}


//--------------------------------------------------------------
void ofxVstHost::closeEffect(int index)
{
    // Make sure index does not go out of bound
    if(index < _effects.size()) {
        // Close effect
        //dispatcher(_effects[index], effStopProcess);
        dispatcher(_effects[index], effMainsChanged, 0, 0);
        dispatcher(_effects[index], effClose);
        
        // Remove this effect from the vector
        _effects.erase(_effects.begin() + index);
        _names.erase(_names.begin() + index);
    }
    else {
        std::cerr << "ofxVstHost: " << "closeEffect(...), index out of bound. Expected " << _effects.size() - 1 << " max, got " << index << std::endl;
    }
}

//TODO: Make sure the plugin is not used in other currently open effects
//--------------------------------------------------------------
void ofxVstHost::closePlugin(int index)
{
    // Make sure index does not go out of bound
    if(index < _plugins.size()) {
        // Close library
        _plugins[index].closeLibrary();
        
        // Remove this effect from the vector
        _plugins.erase(_plugins.begin() + index);
    }
    else {
        std::cerr << "ofxVstHost: " << "closePlugin(...), index out of bound. Expected " << _plugins.size() - 1 << " max, got " << index << std::endl;
    }
}


//--------------------------------------------------------------
void ofxVstHost::process(int indexEffect, float *left, float *right, int frames)
{
    // Make sure indexEffect does not go out of bound
    if(indexEffect < _effects.size()) {
        // Tie the pointer array to our sound buffer
        
        _outputs[0] = left;
        _outputs[1] = right;
        for (int i = 2; i < _effects[indexEffect]->numOutputs; i++) {
            _outputs[i] = _empty;
        }
        for (int i = 0; i < _effects[indexEffect]->numInputs; i++) {
            _inputs[i] = _empty;
        }
        
        // Process the effect
        dispatcher(_effects[indexEffect], effStartProcess, 0, 0, nullptr, 0);
        _effects[indexEffect]->processReplacing(_effects[indexEffect], _inputs, _outputs, frames);
        dispatcher(_effects[indexEffect], effStopProcess, 0, 0, nullptr, 0);
    }
    else {
        std::cerr << "ofxVstHost: " << "process(...), indexEffect out of bound. Expected " << _effects.size() - 1 << "max, got " << indexEffect << std::endl;
    }
}

// Return the number of plugins (libraries) loaded currently
//--------------------------------------------------------------
int ofxVstHost::getNumPlugins(void) const
{
    return (int) _plugins.size();
}

// Return the number of effects loaded currently
//--------------------------------------------------------------
int ofxVstHost::getNumEffects(void) const
{
    return (int) _effects.size();
}

//--------------------------------------------------------------
float ofxVstHost::getSampleRate() const
{
    return _sampleRate;
}

//--------------------------------------------------------------
VstInt32 ofxVstHost::getBlockSize() const
{
    return _blockSize;
}


//--------------------------------------------------------------
int ofxVstHost::getNumInputs(int indexEffect) const {
    // Make sure indexEffect does not go out of bound
    if(indexEffect < _effects.size()) {
        return _effects[indexEffect]->numInputs;
    }
    else {
        std::cerr << "ofxVstHost: " << "getNumInputs(...), indexEffect out of bound. Expected " << _effects.size() - 1 << " max, got " << indexEffect << std::endl;
        return -1;
    }
}
//--------------------------------------------------------------
int ofxVstHost::getNumOutputs(int indexEffect) const {
    // Make sure indexEffect does not go out of bound
    if(indexEffect < _effects.size()) {
        return _effects[indexEffect]->numOutputs;
    }
    else {
        std::cerr << "ofxVstHost: " << "getNumOutputs(...), indexEffect out of bound. Expected " << _effects.size() - 1 << " max, got " << indexEffect << std::endl;
        return -1;
    }
}

//--------------------------------------------------------------
int ofxVstHost::getNumParameters(int indexEffect) const {
    // Make sure indexEffect does not go out of bound
    if(indexEffect < _effects.size()) {
        return _effects[indexEffect]->numParams;
    }
    else {
        std::cerr << "ofxVstHost: " << "getNumParameters(...), indexEffect out of bound. Expected " << _effects.size() - 1 << " max, got " << indexEffect << std::endl;
        return -1;
    }
}

//--------------------------------------------------------------
int ofxVstHost::getNumPrograms(int indexEffect) const {
    // Make sure indexEffect does not go out of bound
    if(indexEffect < _effects.size()) {
        return _effects[indexEffect]->numPrograms;
    }
    else {
        std::cerr << "ofxVstHost: " << "getNumPrograms(...), indexEffect out of bound. Expected " << _effects.size() - 1 << " max, got " << indexEffect << std::endl;
        return -1;
    }
}

//--------------------------------------------------------------
std::string ofxVstHost::getVendorString(int indexEffect) const
{
    char vendorString[256] = {0};
    
    // Make sure indexEffect does not go out of bound
    if(indexEffect < _effects.size()) {
        dispatcher (_effects[indexEffect], effGetVendorString, 0, 0, vendorString);
    }
    else {
        std::cerr << "ofxVstHost: " << "getVendorstd::string(...), indexEffect out of bound. Expected " << _effects.size() - 1 << " max, got " << indexEffect << std::endl;
    }
    
    return std::string(vendorString);
}

//--------------------------------------------------------------
std::string ofxVstHost::getProductString(int indexEffect) const
{
    char productString[256] = {0};
    
    // Make sure indexEffect does not go out of bound
    if(indexEffect < _effects.size()) {
        dispatcher (_effects[indexEffect], effGetProductString, 0, 0, productString);
    }
    else {
        std::cerr << "ofxVstHost: " << "getProductstd::string(...), indexEffect out of bound. Expected " << _effects.size() - 1 << " max, got " << indexEffect << std::endl;
    }
    
    return std::string(productString);
}

//--------------------------------------------------------------
std::string ofxVstHost::getEffectName(int indexEffect) const
{
    // Make sure indexEffect does not go out of bound
    if(indexEffect < _names.size()) {
        return _names[indexEffect];
    }
    else {
        std::cerr << "ofxVstHost: " << "getEffectName(...), indexEffect out of bound. Expected " << _names.size() - 1 << " max, got " << indexEffect << std::endl;
    }
    return "";
}

//--------------------------------------------------------------
std::string ofxVstHost::listParameterValues(int indexEffect)
{
    std::string str = "";
    
    // Make sure indexEffect does not go out of bound
    if(indexEffect < _effects.size()) {
        // Iterate parameters...
        for (VstInt32 paramIndex = 0; paramIndex < _effects[indexEffect]->numParams; paramIndex++)
        {
            char paramName[256] = {0};
            char paramLabel[256] = {0};
            char paramDisplay[256] = {0};
            
            dispatcher (_effects[indexEffect], effGetParamName, paramIndex, 0, paramName, 0);
            dispatcher (_effects[indexEffect], effGetParamLabel, paramIndex, 0, paramLabel, 0);
            dispatcher (_effects[indexEffect], effGetParamDisplay, paramIndex, 0, paramDisplay, 0);
            float value = _effects[indexEffect]->getParameter (_effects[indexEffect], paramIndex);
            
            //std::cout << "ofxVstHost: " << "Param " << paramIndex << ": " << paramName << " [" << paramDisplay << " " << paramLabel << "] (normalized = " << value << ")"  << std::endl;
            str += "Param " + std::to_string(paramIndex) + ": " + std::string(paramName) + " [" + std::string(paramDisplay) + " " + std::string(paramLabel) + "]  (normalized = " + std::to_string(value) + ")\n";
        }
    }
    else {
        std::cerr << "ofxVstHost: " << "listParameterValues(...), indexEffect out of bound. Expected " << _effects.size() - 1 << " max, got " << indexEffect << std::endl;
    }
    
    return str;
}


//--------------------------------------------------------------
float ofxVstHost::getParameterValue(int indexEffect, int indexParam) const {
    
    // Make sure indexEffect does not go out of bound
    if(indexEffect < _effects.size()) {
        return _effects[indexEffect]->getParameter (_effects[indexEffect], indexParam);
    }
    else {
        std::cerr << "ofxVstHost: " << "getParameterValue(...), indexEffect out of bound. Expected " << _effects.size() - 1 << " max, got " << indexEffect << std::endl;
    }
    return 0;
}

//--------------------------------------------------------------
void ofxVstHost::setParameterValue(int indexEffect, int indexParam, float value)
{
    // Make sure indexEffect does not go out of bound
    if(indexEffect < _effects.size()) {
        _effects[indexEffect]->setParameter (_effects[indexEffect], indexParam, value);
    }
    else {
        std::cerr << "ofxVstHost: " << "setParameterValue(...), indexEffect out of bound. Expected " << _effects.size() - 1 << " max, got " << indexEffect << std::endl;
    }
}

//--------------------------------------------------------------
std::string ofxVstHost::getParameterName(int indexEffect, int indexParam) const
{
    char paramName[256] = {0};
    
    // Make sure indexEffect does not go out of bound
    if(indexEffect < _effects.size()) {
        dispatcher (_effects[indexEffect], effGetParamName, indexParam, 0, paramName);
    }
    else {
        std::cerr << "ofxVstHost: " << "getParameterName(...), indexEffect out of bound. Expected " << _effects.size() - 1 << " max, got " << indexEffect << std::endl;
    }
    
    return std::string(paramName);
}

//--------------------------------------------------------------
std::string ofxVstHost::getParameterLabel(int indexEffect, int indexParam) const
{
    char paramLabel[256] = {0};
    
    // Make sure indexEffect does not go out of bound
    if(indexEffect < _effects.size()) {
        dispatcher (_effects[indexEffect], effGetParamLabel, indexParam, 0, paramLabel);
    }
    else {
        std::cerr << "ofxVstHost: " << "getParameterLabel(...), indexEffect out of bound. Expected " << _effects.size() - 1 << " max, got " << indexEffect << std::endl;
    }
    
    return std::string(paramLabel);
}

void ofxVstHost::procEvents(VstEvents *events) {
    dispatcher(_effects[0], effProcessEvents, 0, 0, events, 0);
}


// Make sure the platform architecture match the VST library one
//--------------------------------------------------------------
bool ofxVstHost::checkPlatform ()
{
#if VST_64BIT_PLATFORM
    std::cout << "ofxVstHost: " << "This is a 64 Bit Build!" << std::endl;
#else
    std::cout << "ofxVstHost: " << "This is a 32 Bit Build!";
#endif
    
    int sizeOfVstIntPtr = sizeof (VstIntPtr);
    int sizeOfVstInt32 = sizeof (VstInt32);
    int sizeOfPointer = sizeof (void*);
    int sizeOfAEffect = sizeof (AEffect);
    
    std::cout << "ofxVstHost: " << "VstIntPtr = " << sizeOfVstIntPtr << " Bytes, VstInt32 = " << sizeOfVstInt32 << " Bytes, Pointer = " << sizeOfPointer << " Bytes, AEffect = " << sizeOfAEffect << " Bytes" << std::endl;
    
    return sizeOfVstIntPtr == sizeOfPointer;
}


// Helper function to make calls to dispacher a bit cleaner
//--------------------------------------------------------------
intptr_t ofxVstHost::dispatcher(AEffect* effect, int32_t opcode, int32_t index, intptr_t value, void *ptr, float opt) const
{
    return effect->dispatcher(effect, opcode, index, value, ptr, opt);
}


//--------------------------------------------------------------
VstIntPtr VSTCALLBACK ofxVstHost::HostCallback (AEffect* effect, VstInt32 opcode, VstInt32 index, VstIntPtr value, void* ptr, float opt)
{
    VstIntPtr result = 0;
    
    // Filter idle calls...
    bool filtered = false;
    if (opcode == audioMasterIdle)
    {
        static bool wasIdle = false;
        if (wasIdle)
            filtered = true;
        else
        {
            std::cout << "ofxVstHost: " << "(Future idle calls will not be displayed!)" << std::endl;
            wasIdle = true;
        }
    }
    
    if (!filtered) {
        //std::cout << "ofxVstHost: " << "PLUG> HostCallback (opcode " << opcode << ")" << std::endl;
        //std::cout << "ofxVstHost: " << "index = " << index << ", value = " << FromVstPtr<void> (value) << ", ptr = " << ptr << ", opt = " << opt << std::endl;
    }
    
    switch (opcode)
    {
        case audioMasterVersion :
            result = kVstVersion;
            break;
            
        case audioMasterCurrentId:
            result = effect->uniqueID;
            break;
            
        case audioMasterGetSampleRate:
            //result = getSampleRate();
            result = 44100.0;
            break;
            
        case audioMasterGetBlockSize:
            //result = getBlockSize();
            result = 512;
            break;
            
        case DECLARE_VST_DEPRECATED(audioMasterWantMidi):
            result = 1;
            break;
            
        case DECLARE_VST_DEPRECATED(audioMasterNeedIdle):
            result = 1;
            break;
            
        case audioMasterGetCurrentProcessLevel:
            result = kVstProcessLevelRealtime;
            break;
            
        case audioMasterGetAutomationState:
            //result = kVstAutomationOff;
            break;
            
        case audioMasterGetLanguage:
            result = kVstLangEnglish;
            break;
            
        case audioMasterGetVendorVersion:
            result = 0x01000000;
            break;
            
        case audioMasterGetVendorString:
            strcpy(static_cast<char*>(ptr), "1.0.0");
            result = 1;
            break;
            
        case audioMasterGetProductString:
            strcpy(static_cast<char*>(ptr), "TinyPiano");
            result = 1;
            break;
            
        /*case audioMasterGetTime:
            timeinfo.flags      = 0;
            timeinfo.samplePos  = getSamplePos();
            timeinfo.sampleRate = getSampleRate();
            result = reinterpret_cast<VstIntPtr>(&timeinfo);
            break;
            
        case audioMasterGetDirectory:
            result = reinterpret_cast<VstIntPtr>(directoryMultiByte.c_str());
            break;
            
        case audioMasterIdle:
            if(editorHwnd) {
                dispatcher(effEditIdle);
            }
            break;
            
        case audioMasterSizeWindow:
            if(editorHwnd) {
                RECT rc {};
                GetWindowRect(editorHwnd, &rc);
                rc.right = rc.left + static_cast<int>(index);
                rc.bottom = rc.top + static_cast<int>(value);
                resizeEditor(rc);
            }
            break;
            
        case audioMasterCanDo:
            for(const char** pp = getCapabilities(); *pp; ++pp) {
                if(strcmp(*pp, static_cast<const char*>(ptr)) == 0) {
                    result = 1;
                }
            }
            break;*/
    }
    
    return result;
}
