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

const std::map<KeyCode, KeyboardMapping::Info> DEFUALT_KEY_MAP_INFOS =
{
    {kGrave, {7, 0, 0, 0}},
    {k1, {1, 1, 0, 0}},
    {k2, {2, 1, 0, 0}},
    {k3, {3, 1, 0, 0}},
    {k4, {4, 1, 0, 0}},
    {k5, {5, 1, 0, 0}},
    {k6, {6, 1, 0, 0}},
    {k7, {7, 1, 0, 0}},
    {k8, {1, 2, 0, 0}},
    {k9, {2, 2, 0, 0}},
    {k0, {3, 2, 0, 0}},
    {kMinus, {4, 2, 0, 0}},
    {kEq,    {5, 2, 0, 0}},
    {kBackspace, {6, 2, 0, 0}},
    {kTab,       {7, -1, 0, 0}},
    {kQ, {1, 0, 0, 0}},
    {kW, {2, 0, 0, 0}},
    {kE, {3, 0, 0, 0}},
    {kR, {4, 0, 0, 0}},
    {kT, {5, 0, 0, 0}},
    {kY, {6, 0, 0, 0}},
    {kU, {7, 0, 0, 0}},
    {kI, {1, 1, 0, 0}},
    {kO, {2, 1, 0, 0}},
    {kP, {3, 1, 0, 0}},
    {kLSB, {4, 1, 0, 0}},
    {kRSB, {5, 1, 0, 0}},
    {kBackslash, {6, 1, 0, 0}},
    {kCapsLock, {7, -2, 0, 0}},
    {kA, {1, -1, 0, 0}},
    {kS, {2, -1, 0, 0}},
    {kD, {3, -1, 0, 0}},
    {kF, {4, -1, 0, 0}},
    {kG, {5, -1, 0, 0}},
    {kH, {6, -1, 0, 0}},
    {kJ, {7, -1, 0, 0}},
    {kK, {1, 0, 0, 0}},
    {kL, {2, 0, 0, 0}},
    {kSemi,   {3, 0, 0, 0}},
    {kQM,     {4, 0, 0, 0}},
    {kEnter,  {5, 0, 0, 0}},
    {kLShift, {7, -3, 0, 0}},
    {kZ, {1, -2, 0, 0}},
    {kX, {2, -2, 0, 0}},
    {kC, {3, -2, 0, 0}},
    {kV, {4, -2, 0, 0}},
    {kB, {5, -2, 0, 0}},
    {kN, {6, -2, 0, 0}},
    {kM, {7, -2, 0, 0}},
    {kComma,  {1, -1, 0, 0}},
    {kDot,  {2, -1, 0, 0}},
    {kSlash,  {3, -1, 0, 0}},
    {kRShift, {4, -1, 0, 0}},

    {kLOption,  {3, -1, 0, 0}}, //
    {kSpace,    {5, -1, 1, 0}}, //
    {kRCommand, {6, -1, 0, 0}}, //

    {kLeft,  {1, -1, 0, 1}},
    {kDown,  {2, -1, 0, 1}},

    {kRight, {3, -1, 0, 1}},
    {kUp,    {4, -1, 0, 1}},
    //{kRight, {kRight, 5, 1, 1, 1}}, //
    //{kUp,    {kUp,    4, 1, 1, 1}}, //

    {kKP0,      {5, -1, 0, 1}},
    {kKPDot,    {6, -1, 0, 1}},
    {kKPEnter,  {7, -1, 0, 1}},
    {kKP1,      {1, 0, 0, 1}},
    {kKP2,      {2, 0, 0, 1}},
    {kKP3,      {3, 0, 0, 1}},
    {kKP4,      {4, 0, 0, 1}},
    {kKP5,      {5, 0, 0, 1}},
    {kKP6,      {6, 0, 0, 1}},
    {kKP7,      {7, 0, 0, 1}},
    {kKP8,      {1, 1, 0, 1}},
    {kKP9,      {2, 1, 0, 1}},
    {kKPlus,    {3, 1, 0, 1}},
    {kKPNumLock,{4, 1, 0, 1}},
    {kKPSlash,    {5, 1, 0, 1}},
    {kKPAsterisk, {6, 1, 0, 1}},
    {kKPHyphen,   {7, 1, 0, 1}},

    {kDelete,   {1, 2, 0, 1}},
    {kEnd,      {2, 2, 0, 1}},
    {kPageDown, {3, 2, 0, 1}},
    {kInsert,   {4, 2, 0, 1}},
    {kHome,     {5, 2, 0, 1}},
    {kPageUp,   {6, 2, 0, 1}},
    {kF13,      {7, 2, 0, 1}},
    {kF14,      {1, 3, 0, 1}},
    {kF15,      {2, 3, 0, 1}},

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

void config_set_resource_path(const std::string& path) {
    resource_path = path;
}

std::string config_get_fn_text(FunctionId fn, int modifier) {
    if (modifier > 0) {
        return FN_TEXT.at(fn) + "+";
    } else if (modifier < 0) {
        return FN_TEXT.at(fn) + "-";
    }
    return FN_TEXT.at(fn);
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

GlobalData& config_get_global_data_rer() {
    return data;
}
