//
//  config.hpp
//  tinypiano
//
//  Created by zq on 2022/9/3.
//

#ifndef config_hpp
#define config_hpp

#include <stdio.h>
#include <string>

enum KeyCode {
    kNone = 999,
    
    kEsc = 53,

    // Function area
    kF1 = 122,
    kF2 = 120,
    kF3 = 99,
    kF4 = 118,
    kF5 = 96,
    kF6 = 97,
    kF7 = 98,
    kF8 = 100,
    kF9 = 101,
    kF10 = 109,
    kF11 = 103,
    kF12 = 111,
    kF13 = 105,
    kF14 = 107,
    kF15 = 113,

    // Main area
    k1 = 18,
    k2 = 19,
    k3 = 20,
    k4 = 21,
    k5 = 23,
    k6 = 22,
    k7 = 26,
    k8 = 28,
    k9 = 25,
    k0 = 29,
    kA = 0,
    kB = 11,
    kC = 8,
    kD = 2,
    kE = 14,
    kF = 3,
    kG = 5,
    kH = 4,
    kI = 34,
    kJ = 38,
    kK = 40,
    kL = 37,
    kM = 46,
    kN = 45,
    kO = 31,
    kP = 35,
    kQ = 12,
    kR = 15,
    kS = 1,
    kT = 17,
    kU = 32,
    kV = 9,
    kW = 13,
    kX = 7,
    kY = 16,
    kZ = 6,
    kGrave = 50,
    kMinus = 27,
    kEq = 24,
    kLSB = 33, // Left Square Bracket [
    kRSB = 30, // Right Square Bracket ]
    kBackslash = 42, //
    kSemi = 41, // ;
    kQM = 39, // '
    kComma = 43, // ,
    kDot = 47, // .
    kSlash = 44, // /
    kTab = 48,
    kCapsLock = 57,
    kLShift = 56,
    kRShift = 60,
    kLCtrl = 59,
    kRCtrl = 62,
    kLCommand = 55,
    kRCommand = 54,
    kLOption = 58,
    kROption = 61,
    kSpace = 49,
    kMenu = 63,
    kEnter = 36,
    kBackspace = 51,

    /// Control area
    kInsert = 114,
    kDelete = 117,
    kHome = 115,
    kEnd = 119,
    kPageUp = 116,
    kPageDown = 121,
    kUp = 126,
    kDown = 125,
    kLeft = 123,
    kRight = 124,

    /// Num pad area
    kKPNumLock = 71,
    kKPSlash = 75,
    kKPAsterisk = 67,
    kKPHyphen = 78,
    kKPlus = 69,
    kKPEnter = 76,
    kKPDot = 65,
    kKP1 = 83,
    kKP2 = 84,
    kKP3 = 85,
    kKP4 = 86,
    kKP5 = 87,
    kKP6 = 88,
    kKP7 = 89,
    kKP8 = 91,
    kKP9 = 92,
    kKP0 = 82,
};

enum FunctionId : uint8_t {
    kMoveMode       = 0x80, // 1000 0000

    kLeftSostenuto  = 0x90, // 1001 0000
    kRightSostenuto = 0x91, // 1001 0001

    kLeftAdjOctave  = 0xA0, // 1010 0000
    kRightAdjOctave = 0xA1, // 1010 0001

    kLeftIntensity  = 0xC0, // 1100 0000
    kRightIntensity = 0xC1, // 1100 0001
};

struct KeyboardLayout {
    float width;
    float height;
    
    struct {
        KeyCode code;
        float x;
        float y;
        float w;
        float h;
    } keys[127];
    int num;
};

struct KeyboardMapping {
    struct Info {
        uint8_t note;
        int8_t pith;
        int8_t modifier : 4;  // 1: up, -1: down
        int8_t left : 4; // 1, 0
    } keys[127];
};

struct GlobalData {
    int mode;

    bool lSostenuto = false;
    bool rSostenuto = false;

    int8_t lOffOctave = 0;
    int8_t rOffOctave = 0;

    int lIntensity = 40;
    int rIntensity = 40;
};

const char*     config_mode_text(int index);

void            config_set_resource_path(const std::string& path);

void            config_chg_mode(int modifier);
void            config_chg_octave(uint8_t fn, int modifier);
void            config_chg_intensity(uint8_t fn, int modifier);

std::string     config_get_fn_text(FunctionId fn, int modifier);

bool            config_is_fn(int num);
uint8_t         config_real_note(const KeyboardMapping::Info& map);

KeyboardLayout  config_get_keyboard_layout();

KeyboardMapping config_default_keyboard_mapping();
KeyboardMapping config_get_curr_keyboard_mapping();

GlobalData&     config_get_global_data_ref();



#endif /* config_hpp */
