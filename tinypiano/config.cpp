//
//  config.cpp
//  tinypiano
//
//  Created by zq on 2022/9/3.
//

#include "config.hpp"

#include <json/json.h>
#include <fstream>

#define keyboard_layout_name "keyboard_layout.json"

std::string resource_path;

const std::map<std::string, KeyCode>           KEY_CODES =
{
    {"Esc",kEsc}, {"F1",kF1}, {"F2",kF2}, {"F3", kF3}, {"F4",kF4}, {"F5",kF5}, {"F6",kF6}, {"F7",kF7}, {"F8",kF8}, {"F9",kF9}, {"F10",kF10}, {"F11",kF11}, {"F12",kF12}, {"PrtSc",kF13}, {"Scroll Lock",kF14}, {"Pause",kF15},
    {"`",kGrave}, {"1",k1}, {"2",k2}, {"3",k3}, {"4",k4}, {"5",k5}, {"6",k6}, {"7",k7}, {"8",k8}, {"9",k9}, {"0",k0}, {"-",kMinus}, {"=",kEq}, {"Backspace",kBackspace}, {"Insert", kInsert}, {"Home",kHome}, {"PgUp",kPageUp}, {"NumLock",kKPNumLock}, {"keypad(/)",kKPSlash}, {"keypad(*)",kKPAsterisk}, {"keypad(-)",kKPHyphen},
    {"Tab",kTab}, {"Q",kQ}, {"W",kW}, {"E",kE}, {"R",kR}, {"T",kT}, {"Y",kY}, {"U",kU}, {"I",kI}, {"O",kO}, {"P",kP}, {"[",kLSB}, {"]",kRSB}, {"\\",kBackslash}, {"Delete",kDelete}, {"End",kEnd}, {"PgDn",kPageDown}, {"keypad(7)",kKP7}, {"keypad(8)",kKP8}, {"keypad(9)",kKP9}, {"keypad(+)",kKPlus},
    {"Caps Lock",kCapsLock}, {"A", kA}, {"S",kS}, {"D",kD}, {"F",kF}, {"G",kG}, {"H",kH}, {"J",kJ}, {"K",kK}, {"L",kL}, {";",kSemi}, {"'",kQM}, {"Enter",kEnter}, {"keypad(4)",kKP4}, {"keypad(5)",kKP5}, {"keypad(6)",kKP6},
    {"LShift",kLShift}, {"Z",kZ}, {"X",kX}, {"C",kC}, {"V",kV} ,{"B",kB}, {"N",kN}, {"M",kM}, {",",kComma}, {".",kDot}, {"/",kSlash}, {"RShift",kRShift}, {"↑",kUp}, {"keypad(1)",kKP1}, {"keypad(2)",kKP2}, {"keypad(3)",kKP3}, {"keypad(Enter)",kKPEnter},
    {"LCtrl",kLCtrl}, {"LCommand",kLCommand}, {"LOption",kLOption}, {"Space",kSpace}, {"ROption",kROption}, {"RCommand",kRCommand}, {"Menu", kMenu}, {"RCtrl",kRCtrl}, {"←",kLeft}, {"↓",kDown}, {"→",kRight}, {"keypad(0)",kKP0}, {"keypad(.)",kKPDot}
};

const std::map<FunctionId, std::string>           FN_TEXT =
{
    {kMoveMode,"M"}, {kLeftSostenuto,"LS"}, {kRightSostenuto,"RS"},
    {kLeftAdjOctave,"LO"}, {kRightAdjOctave,"RO"},
    {kLeftIntensity,"LV"}, {kRightIntensity,"RV"}
};

const std::map<KeyCode, KeyboardMapping::Info>    DEFUALT_KEY_MAP_INFOS =
{
    {kGrave, {7, 0, 0, 1}},
    {k1, {1, 1, 0, 1}},
    {k2, {2, 1, 0, 1}},
    {k3, {3, 1, 0, 1}},
    {k4, {4, 1, 0, 1}},
    {k5, {5, 1, 0, 1}},
    {k6, {6, 1, 0, 1}},
    {k7, {7, 1, 0, 1}},
    {k8, {1, 2, 0, 1}},
    {k9, {2, 2, 0, 1}},
    {k0, {3, 2, 0, 1}},
    {kMinus, {4, 2, 0, 1}},
    {kEq,    {5, 2, 0, 1}},
    {kBackspace, {6, 2, 0, 1}},
    {kTab,       {7, -1, 0, 1}},
    {kQ, {1, 0, 0, 1}},
    {kW, {2, 0, 0, 1}},
    {kE, {3, 0, 0, 1}},
    {kR, {4, 0, 0, 1}},
    {kT, {5, 0, 0, 1}},
    {kY, {6, 0, 0, 1}},
    {kU, {7, 0, 0, 1}},
    {kI, {1, 1, 0, 1}},
    {kO, {2, 1, 0, 1}},
    {kP, {3, 1, 0, 1}},
    {kLSB, {4, 1, 0, 1}},
    {kRSB, {5, 1, 0, 1}},
    {kBackslash, {6, 1, 0, 1}},
    {kCapsLock, {7, -2, 0, 1}},
    {kA, {1, -1, 0, 1}},
    {kS, {2, -1, 0, 1}},
    {kD, {3, -1, 0, 1}},
    {kF, {4, -1, 0, 1}},
    {kG, {5, -1, 0, 1}},
    {kH, {6, -1, 0, 1}},
    {kJ, {7, -1, 0, 1}},
    {kK, {1, 0, 0, 1}},
    {kL, {2, 0, 0, 1}},
    {kSemi,   {3, 0, 0, 1}},
    {kQM,     {4, 0, 0, 1}},
    {kEnter,  {5, 0, 0, 1}},
    {kLShift, {7, -3, 0, 1}},
    {kZ, {1, -2, 0, 1}},
    {kX, {2, -2, 0, 1}},
    {kC, {3, -2, 0, 1}},
    {kV, {4, -2, 0, 1}},
    {kB, {5, -2, 0, 1}},
    {kN, {6, -2, 0, 1}},
    {kM, {7, -2, 0, 1}},
    {kComma,  {1, -1, 0, 1}},
    {kDot,  {2, -1, 0, 1}},
    {kSlash,  {3, -1, 0, 1}},
    {kRShift, {4, -1, 0, 1}},

    {kLOption,  {3, -1, 0, 1}}, //
    {kSpace,    {5, -1, 1, 1}}, //
    {kRCommand, {6, -1, 0, 1}}, //

    {kLeft,  {1, -1, 0, 0}},
    {kDown,  {2, -1, 0, 0}},

    {kRight, {3, -1, 0, 0}},
    {kUp,    {4, -1, 0, 0}},
    //{kRight, {kRight, 5, 1, 1, 1}}, //
    //{kUp,    {kUp,    4, 1, 1, 1}}, //

    {kKP0,      {5, -1, 0, 0}},
    {kKPDot,    {6, -1, 0, 0}},
    {kKPEnter,  {7, -1, 0, 0}},
    {kKP1,      {1, 0, 0, 0}},
    {kKP2,      {2, 0, 0, 0}},
    {kKP3,      {3, 0, 0, 0}},
    {kKP4,      {4, 0, 0, 0}},
    {kKP5,      {5, 0, 0, 0}},
    {kKP6,      {6, 0, 0, 0}},
    {kKP7,      {7, 0, 0, 0}},
    {kKP8,      {1, 1, 0, 0}},
    {kKP9,      {2, 1, 0, 0}},
    {kKPlus,    {3, 1, 0, 0}},
    {kKPNumLock,{4, 1, 0, 0}},
    {kKPSlash,    {5, 1, 0, 0}},
    {kKPAsterisk, {6, 1, 0, 0}},
    {kKPHyphen,   {7, 1, 0, 0}},

    {kDelete,   {1, 2, 0, 0}},
    {kEnd,      {2, 2, 0, 0}},
    {kPageDown, {3, 2, 0, 0}},
    {kInsert,   {4, 2, 0, 0}},
    {kHome,     {5, 2, 0, 0}},
    {kPageUp,   {6, 2, 0, 0}},
    {kF13,      {7, 2, 0, 0}},
    {kF14,      {1, 3, 0, 0}},
    {kF15,      {2, 3, 0, 0}},

    {kF1,  {kMoveMode, 0, -1, 0}},
    {kF2,  {kMoveMode, 0, 1, 0}},
    {kF3,  {kLeftSostenuto, 0, 0, 0}},
    {kF4,  {kRightSostenuto, 0, 0, 0}},
    {kF5,  {kLeftAdjOctave, 0, -1, 0}},
    {kF6,  {kLeftAdjOctave, 0, 1, 0}},
    {kF7,  {kRightAdjOctave, 0, -1, 0}},
    {kF8,  {kRightAdjOctave, 0, 1, 0}},
    {kF9,  {kLeftIntensity, 0, -1, 0}},
    {kF10, {kLeftIntensity, 0, 1, 0}},
    {kF11, {kRightIntensity, 0, -1, 0}},
    {kF12, {kRightIntensity, 0, 1, 0}},
};

GlobalData data;

// Indexes of white key
// 1 2 3 4 5 6 7 8 9 0 1 2
// 1   2   3 4   5   6   7
static int         major_mode[7] = {1, 3, 5, 6, 8, 10, 12};

// Modes offset
static int         modes[12] = {0, 1, 2, 3, 4, 5, 6, 7, -4, -3, -2, -1,};
static const char* modes_text[12] = {
    "C","♯C","D","Eb", "E", "F", "♯F", "G", "Ab", "A", "Bb", "B",
};

const char* config_mode_text(int index) {
    return modes_text[index];
}


void config_set_resource_path(const std::string& path) {
    resource_path = path;
}

void config_chg_mode(int modifier) {
    data.mode += modifier;
    if (data.mode < 0) {
        data.mode = 11;
    }
    if (data.mode > 11) {
        data.mode = 0;
    }
}

void config_chg_octave(uint8_t fn, int modifier) {
    int8_t &octave = fn==kLeftAdjOctave ? data.lOffOctave : data.rOffOctave;
    octave += modifier;
    octave = std::min((int) octave, 1);
    octave = std::max((int) octave, -1);
}

void config_chg_intensity(uint8_t fn, int modifier) {
    int &intensity = fn==kLeftIntensity ? data.lIntensity : data.rIntensity;
    intensity += modifier;
    intensity = std::min(intensity, 127);
    intensity = std::max(intensity, 0);
}

std::string config_get_fn_text(FunctionId fn, int modifier) {
    if (modifier > 0) {
        return FN_TEXT.at(fn) + "+";
    } else if (modifier < 0) {
        return "-" + FN_TEXT.at(fn);
    }
    return FN_TEXT.at(fn);
}

bool config_is_fn(int num) {
    return num & 0x80;
}

uint8_t config_real_note(const KeyboardMapping::Info& map) {
    uint8_t note = 23 + (12 * (map.pith + 3)) + major_mode[map.note - 1];
    return note + map.modifier  // # or b
                + modes[data.mode] // mode
                + (map.left ? data.lOffOctave*12 : data.rOffOctave*12); // Octave
}

KeyboardLayout config_get_keyboard_layout() {
    KeyboardLayout layout;
    
    Json::Value root;
    Json::CharReaderBuilder builder;
    Json::String errs = "";
    std::ifstream ifs(resource_path + keyboard_layout_name, std::ios::binary);
    if (!Json::parseFromStream(builder, ifs, &root, &errs))
        return {};
    
    int i = 0;
    int last_key_index = 0;
    float ofx = 0.0, ofy = 0.0, kw = 1.0, kh = 1.0;
    for (auto& row : root) {
        if (!row.isArray())
            continue;
        
        for (auto& item : row) {
            if (item.isString()) {
                auto& key = layout.keys[i++];
                key.code = KEY_CODES.at(item.asString());
                key.x = ofx;
                key.y = ofy;
                key.w = kw;
                key.h = kh;
                ofx += key.w;
                kw = 1.0;
                kh = 1.0;
                if (key.code == kKPEnter)
                    last_key_index = i-1;
            } else if (item.isObject()) {
                ofx += item.get("x", Json::Value(0.0)).asFloat();
                ofy += item.get("y", Json::Value(0.0)).asFloat();
                kw   = item.get("w", Json::Value(1.0)).asFloat();
                kh   = item.get("h", Json::Value(1.0)).asFloat();
            }
        }
        ofx = 0.0;
        ofy += 1;
    }
    
    auto& last_key = layout.keys[last_key_index];
    layout.width = last_key.x + last_key.w;
    layout.height = last_key.y + last_key.h;
    layout.num = i;
    
    return layout;
}


KeyboardMapping config_default_keyboard_mapping() {
    KeyboardMapping mapping = {};
    for (int i = 0; i < 127; i++) {
        auto iter = DEFUALT_KEY_MAP_INFOS.find(KeyCode(i));
        if (iter != DEFUALT_KEY_MAP_INFOS.end()) {
            mapping.keys[i] = iter->second;
        }
    }
    return mapping;
}

KeyboardMapping config_get_curr_keyboard_mapping() {
    return config_default_keyboard_mapping();
}

GlobalData& config_get_global_data_ref() {
    return data;
}
