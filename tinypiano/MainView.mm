//
//  MainView.mm
//  tinypiano
//
//  Created by zq on 2022/9/2.
//

#import "MainView.h"

#include "drawcontext.hpp"
#include "audio.hpp"
#include "config.hpp"
#include "midievent.hpp"

#include <map>


struct KeyboardButton {
    CGRect frame;
    CFStringRef text;
    
    KeyboardMapping::Info map;
};

struct PianoButton {
    CGRect frame;
    bool is_black;
};


CGColorRef cgcolor(int r, int g, int b) {
    return CGColorCreateSRGB(float(r)/255.0,
                             float(g)/255.0,
                             float(b)/255.0,
                             1.0);
}

CFStringRef cfstr_from_int(int val) {
    std::string utf8_str = std::to_string(val);
    return CFStringCreateWithCString(kCFAllocatorDefault,
                                     utf8_str.c_str(),
                                     kCFStringEncodingUTF8);
}

CFStringRef cfstr_from_str(const char *str) {
    return CFStringCreateWithCString(kCFAllocatorDefault,
                                     str,
                                     kCFStringEncodingUTF8);
}

@implementation MainView {
    CGContextRef memContext;
    CGColorRef back_color;
    CGColorRef keybtn_color;
    CGColorRef kempty_color;
    CGColorRef keytxt_color;
    CGColorRef mode_color;
    CGColorRef sostenuto_color;
    CGColorRef adj_lotc_color;
    CGColorRef adj_rotc_color;
    CGColorRef lintensity_color;
    CGColorRef rintensity_color;
    CGColorRef fn_val_color;
    CGColorRef piano_black_button_color;
    
    float width_ratio;
    float height_ratio;
    
    float keyboard_width;
    float keyboard_height;
    float keyboard_height_rate;
    float fn_area_height_rate;
    struct KeyboardButton *kb_buttons[127];
    struct PianoButton *midi_buttons[127];
    std::map<uint8_t, uint8_t> lbutton_state;
    std::map<uint8_t, uint8_t> rbutton_state;
    
    AudioClient audio;
}

- (void) initView {
    keyboard_width = 0;
    keyboard_height = 0;
    keyboard_height_rate = 2.4/4.0;
    fn_area_height_rate = 0.5/4.0;
    
    // Black theme.
//    key_lbutton_down_ = CColor(131, 178, 35);
//    key_rbutton_down_ = CColor(52, 142, 231);
//    an se zhu ti
//    key_button_up_ = CColor(57, 57, 57);
//    key_button_empty_ = CColor(35, 35, 35);
//    key_text_ = CColor(193, 191, 192);

    back_color       = cgcolor(237, 237, 237);
    keybtn_color     = cgcolor(198, 198, 198);
    kempty_color     = cgcolor(220, 220, 220);
    keytxt_color     = cgcolor(62,  64,  63);
    mode_color       = cgcolor(200, 157, 200);
    sostenuto_color  = mode_color;
    adj_lotc_color   = cgcolor(131, 180, 251);
    adj_rotc_color   = adj_lotc_color;
    lintensity_color = cgcolor(99,  187, 208);
    rintensity_color = lintensity_color;
    
    fn_val_color     = cgcolor(120, 120, 120);
    
    piano_black_button_color = cgcolor(100, 100, 100);
    
    // Create draw area of memory.
    NSSize frameSize = self.frame.size;
    CGColorSpaceRef colorSpace = CGColorSpaceCreateDeviceRGB();
    self->memContext = CGBitmapContextCreate(NULL,
                                             frameSize.width,
                                             frameSize.height,
                                             8, 32 * frameSize.width,
                                             colorSpace,
                                             kCGImageAlphaPremultipliedLast);
    CGColorSpaceRelease(colorSpace);
    
    // Initialize keyboard layout.
    [self initKeyboardButtons];
    [self initMIDIKeyboardButtons];
    
    midievent_open();
    audio.open();
}

- (void) initKeyboardButtons {
    KeyboardLayout layout = config_get_keyboard_layout();
    KeyboardMapping mapping = config_get_curr_keyboard_mapping();
    for (int i = 0; i < layout.num; i++) {
        auto& key = layout.keys[i];
        auto& map = mapping.keys[int(key.code)];
        self->kb_buttons[key.code] = new KeyboardButton();
        auto* button = self->kb_buttons[key.code];
        button->map.note = map.note;
        button->map.pith = map.pith;
        button->map.modifier = map.modifier;
        button->map.left = map.left;
        button->frame = CGRectInset(CGRectMake(key.x, key.y, key.w, key.h),
                                    0.05, 0.05);
        std::string text;
        if (button->map.note > 100)
            text = config_get_fn_text(FunctionId(button->map.note), button->map.modifier);
        else
            text = std::to_string(button->map.note);
        button->text = CFStringCreateWithCString(kCFAllocatorDefault, text.c_str(),
                                                 kCFStringEncodingUTF8);
    }
    // 0.25 is right margin
    self->keyboard_width = layout.width + 0.25;
    self->keyboard_height = layout.height;
    
    width_ratio = self.frame.size.width / self->keyboard_width;
    height_ratio = (self.frame.size.height * keyboard_height_rate) / self->keyboard_height;
}

- (void) initMIDIKeyboardButtons {
    // 0: white key, 1: black key
    const int layout[] = {0, 1, 0, 1, 0, 0, 1, 0, 1, 0, 1, 0};
    
    float piano_keyboard_height = (keyboard_height / keyboard_height_rate)
                                * (1-(keyboard_height_rate+fn_area_height_rate)) - 0.25;
    
    float white_width = (keyboard_width - 0.55) / 52;
    float black_width = white_width * 0.65;
    
    float x = 0.3;
    float y = keyboard_height + (self->keyboard_height / keyboard_height_rate) * fn_area_height_rate + 0.25;
    
    for (int i = 21; i < 21+88; i++) {
        midi_buttons[i] = new PianoButton();
        midi_buttons[i]->is_black = layout[i%12];
        if (midi_buttons[i]->is_black) {
            midi_buttons[i]->frame = CGRectMake(x - (white_width*0.3),
                                               y,
                                               black_width,
                                               piano_keyboard_height * 0.55);
        } else {
            midi_buttons[i]->frame = CGRectMake(x,
                                               y,
                                               white_width,
                                               piano_keyboard_height);
            x += white_width;
        }
    }
}

- (void) keyDown:(NSEvent *)event {
    uint16_t keycode = event.keyCode;
    bool repeat = event.ARepeat;
    if (repeat) {
        return;
    }
    
    auto *kb = kb_buttons[keycode];
    if (kb == nullptr) {
        return;
    }
    
    if (config_is_fn(kb->map.note)) {
        if (kb->map.note == kMoveMode) {
            config_chg_mode(kb->map.modifier);
        } else if ((kb->map.note & kLeftAdjOctave) == kLeftAdjOctave) {
            config_chg_octave(kb->map.note, kb->map.modifier);
        } else if ((kb->map.note & kLeftIntensity) == kLeftIntensity) {
            config_chg_intensity(kb->map.note, kb->map.modifier);
        } else if ((kb->map.note & kLeftSostenuto) == kLeftSostenuto) {
            auto &btn_state = kb->map.note == kLeftSostenuto
                                    ? lbutton_state : rbutton_state;
            for (auto &item : btn_state) {
                if (item.second == 0) {
                    VstMidiEvent midievent;
                    midievent.midiData[0] = (char) 0x81;
                    midievent.midiData[1] = item.first;
                    midievent.midiData[2] = 0;
                    midievent_send(&midievent);
                }
            }
            btn_state.clear();
        }
        [self setNeedsDisplay:TRUE];
        return;
    }
    
    uint8_t note = config_real_note(kb->map);
    
    GlobalData &gd = config_get_global_data_ref();
    VstMidiEvent midievent;
    midievent.midiData[0] = (char) 0x91;
    midievent.midiData[1] = note;
    midievent.midiData[2] = kb->map.left ? gd.lIntensity : gd.rIntensity;
    midievent_send(&midievent);
    
    // Push down.
    if (kb->map.left) {
        lbutton_state[note] = 1;
    } else {
        rbutton_state[note] = 1;
    }
    [self setNeedsDisplay:TRUE];
}

- (void) keyUp:(NSEvent *)event {
    uint16_t keycode = event.keyCode;
    bool repeat = event.ARepeat;
    if (repeat) {
        return;
    }
    
    auto *kb = kb_buttons[keycode];
    if (kb == nullptr) {
        return;
    }
    
    uint8_t note = config_real_note(kb->map);
    
    // Push down.
    if (kb->map.left) {
        lbutton_state[note] = 0;
    } else {
        rbutton_state[note] = 0;
    }
    [self setNeedsDisplay:FALSE];
}

- (void) drawRect:(NSRect)dirtyRect {
    CGContextRef context = [[NSGraphicsContext currentContext] CGContext];
    
//    NSSize frameSize = self.frame.size;
//    CGColorSpaceRef colorSpace = CGColorSpaceCreateDeviceRGB();
//    self->memContext = CGBitmapContextCreate(NULL,
//                                             frameSize.width,
//                                             frameSize.height,
//                                             8, 32 * frameSize.width,
//                                             colorSpace,
//                                             kCGImageAlphaPremultipliedLast);
//    CGColorSpaceRelease(colorSpace);
    
    CGContextSetRGBFillColor(context, 237.0/255.0, 237.0/255.0, 237.0/255.0, 1.0);
    CGContextFillRect(context, self.frame);
    
    DrawContext dcontext(context, self.frame.size);
    dcontext.set_wh_ratio(width_ratio, height_ratio);
    dcontext.set_coord_reverse(true);
    
    [self drawKeyboard:dcontext];
    [self drawFnArea:dcontext];
    [self drawPianoKeyboard:dcontext];
    
//    CGImageRef image = CGBitmapContextCreateImage(self->memContext);
//    CGContextDrawImage(context, self.frame, image);
//    CGImageRelease(image);
//
//    CGContextRelease(self->memContext);
}

- (BOOL) isFlipped {
    return FALSE;
}

- (BOOL) acceptsFirstResponder {
    return TRUE;
}


- (void) drawKeyboard:(DrawContext&)dcontext {
    CFStringRef font_name = CFStringCreateWithCString(kCFAllocatorDefault,
                                                      "Lucida Grande",
                                                      kCFStringEncodingUTF8);
    CTFontRef   font = CTFontCreateWithName(font_name, 12, NULL);
    CTFontRef   mfont = CTFontCreateWithName(font_name, 8, NULL);
    
    GlobalData& gd = config_get_global_data_ref();
    [self drawFnBtns:dcontext :kb_buttons[kF1] :kb_buttons[kF2]
                    :gd.mode :0 :11 :mode_color :true :font :mfont];
    [self drawFnBtns:dcontext :kb_buttons[kF3] :kb_buttons[kF4]
                    :0 :0 :0 :sostenuto_color :false :font :mfont];
    [self drawFnBtns:dcontext :kb_buttons[kF5] :kb_buttons[kF6]
                    :gd.lOffOctave :-1 :1 :adj_lotc_color :true :font :mfont];
    [self drawFnBtns:dcontext :kb_buttons[kF7] :kb_buttons[kF8]
                    :gd.rOffOctave :-1 :1 :adj_rotc_color :true :font :mfont];
    [self drawFnBtns:dcontext :kb_buttons[kF9] :kb_buttons[kF10]
                    :gd.lIntensity :0 :127 :lintensity_color :true :font :mfont];
    [self drawFnBtns:dcontext :kb_buttons[kF11] :kb_buttons[kF12]
                    :gd.rIntensity :0 :127 :rintensity_color :true :font :mfont];
    
    for (auto *button : self->kb_buttons) {
        if (button == NULL || config_is_fn(button->map.note))
            continue;
        
        CGRect rect = dcontext.real_rect(button->frame);
        
        if (button->map.note == 0) {
            dcontext.set_fill_color(kempty_color);
            dcontext.fill_rrect(rect, 2, false);
            continue;
        }
        
        dcontext.set_fill_color(keybtn_color);
        dcontext.fill_rrect(rect, 2, false);
        dcontext.draw_note(button->text, button->map.pith, button->map.modifier,
                           rect, font, mfont, keytxt_color, false);
    }
    
    CFRelease(font);
    CFRelease(mfont);
    CFRelease(font_name);
}

- (void) drawFnBtns:(DrawContext&)dcontext
                   :(KeyboardButton*)down
                   :(KeyboardButton*)up
                   :(int)val :(int)min :(int)max
                   :(CGColorRef)border_color
                   :(bool)continuity
                   :(CTFontRef)font
                   :(CTFontRef)mfont {
    float radius = 2;
    dcontext.set_line_width(2);
    dcontext.set_draw_color(border_color);
    
    if (continuity) {
        float continuity_width = (up->frame.origin.x+up->frame.size.width) - down->frame.origin.x;
        CGRect rect = CGRectMake(down->frame.origin.x, down->frame.origin.y,
                                 continuity_width, up->frame.size.height);
        rect = dcontext.real_rect(rect);
        
        dcontext.set_fill_color(keybtn_color);
        dcontext.fill_rrect(rect, radius, false);
        
        CFStringRef str_val;
        if (down->map.note == kMoveMode) {
            str_val = cfstr_from_str(config_mode_text(val));
        } else {
            str_val = cfstr_from_int(val);
        }
        dcontext.draw_string(str_val, CGRectInset(rect,0,3), mfont, fn_val_color,
                             DrawContext::VALIGN_TOP);
        CFRelease(str_val);
        
        dcontext.draw_string(down->text,
                             dcontext.real_rect(CGRectInset(down->frame, 0.1, 0.1)),
                             font, keytxt_color, DrawContext::VALIGN_MIDDLE);
        dcontext.draw_string(up->text,
                             dcontext.real_rect(CGRectInset(up->frame, 0.1, 0.1)),
                             font, keytxt_color, DrawContext::VALIGN_MIDDLE);
        
        dcontext.draw_rrect(CGRectInset(rect, 1, 1), radius, false);
        
        if (val != 0 || min != 0 || max != 0) {
            float pos = ((rect.size.width - 2) / (max-min)) * (val-min);
            CGPoint from = CGPointMake(rect.origin.x + 1,
                                       rect.origin.y + rect.size.height - 3);
            CGPoint to   = CGPointMake(from.x + pos,
                                       rect.origin.y + rect.size.height - 3);
            dcontext.set_draw_color(fn_val_color);
            dcontext.draw_line(from, to, false);
        }
    } else {
        CGRect rect = dcontext.real_rect(down->frame);
        dcontext.set_fill_color(keybtn_color);
        dcontext.fill_rrect(rect, radius, false);
        dcontext.draw_string(down->text,
                             dcontext.real_rect(down->frame),
                             font, keytxt_color);
        dcontext.draw_rrect(CGRectInset(rect, 1, 1), radius, false);
        
        rect = dcontext.real_rect(up->frame);
        dcontext.set_fill_color(keybtn_color);
        dcontext.fill_rrect(rect, radius, false);
        dcontext.draw_string(up->text,
                             dcontext.real_rect(up->frame),
                             font, keytxt_color);
        dcontext.draw_rrect(CGRectInset(rect, 1, 1), radius, false);
        
    }
}

- (void) drawFnArea:(DrawContext&)dcontext {
    float top = keyboard_height + 0.25;
    float fn_area_height = (self->keyboard_height / keyboard_height_rate) * fn_area_height_rate - 0.25;
    
    CGRect rect = CGRectMake(0.3, top, keyboard_width - 0.55, fn_area_height);
    dcontext.real_rect(&rect);
    
    dcontext.set_draw_color(keybtn_color);
    dcontext.set_line_width(1);
    dcontext.draw_line(rect.origin,
                       CGPointMake(rect.origin.x + rect.size.width, rect.origin.y), false);
    
    float width = rect.size.height * 0.7;
    CGRect btnrect = CGRectMake(rect.origin.x+(rect.size.height-width)/2, rect.origin.y+(rect.size.height-width)/2,
                                width, width);
    dcontext.draw_mbutton(DrawContext::kTriangle, btnrect, keybtn_color, piano_black_button_color);
    
    btnrect.origin.x += btnrect.size.width + 10;
    dcontext.draw_mbutton(DrawContext::kCircle, btnrect, keybtn_color, piano_black_button_color);
    
    btnrect.origin.x += btnrect.size.width + 10;
    dcontext.draw_mbutton(DrawContext::kRect, btnrect, keybtn_color, piano_black_button_color);
    
    dcontext.set_draw_color(keybtn_color);
    dcontext.set_line_width(1);
    dcontext.draw_line(CGPointMake(rect.origin.x, rect.origin.y + rect.size.height),
                       CGPointMake(rect.origin.x + rect.size.width, rect.origin.y + rect.size.height), false);
    
    
    // function values
    dcontext.set_line_width(2);
    CGRect inforect = CGRectMake(rect.size.width - width, rect.origin.y + (rect.size.height-width)/2, width, width);
    dcontext.set_draw_color(rintensity_color);
    dcontext.draw_rrect(inforect, 2, false);
    inforect.origin.x -= width + 4;
    dcontext.set_draw_color(lintensity_color);
    dcontext.draw_rrect(inforect, 2, false);
    inforect.origin.x -= width + 6;
    dcontext.set_draw_color(adj_rotc_color);
    dcontext.draw_rrect(inforect, 2, false);
    inforect.origin.x -= width + 4;
    dcontext.set_draw_color(adj_lotc_color);
    dcontext.draw_rrect(inforect, 2, false);
}

- (void) drawPianoKeyboard:(DrawContext&)dcontext {
    dcontext.set_draw_color(keybtn_color);
    dcontext.set_line_width(1);
    for (int i = 21; i < 21+88; i++) {
        auto *button = midi_buttons[i];
        CGRect rect = dcontext.real_rect(button->frame);
        if (!button->is_black) {
            dcontext.set_fill_color(back_color);
            dcontext.fill_rect(rect, false);
            dcontext.draw_rect(rect, false);
        }
        auto* pre_button = midi_buttons[i-1];
        if (pre_button != NULL && pre_button->is_black) {
            rect = dcontext.real_rect(pre_button->frame);
            dcontext.set_fill_color(piano_black_button_color);
            dcontext.fill_rect(rect, false);
        }
    }
}

@end
