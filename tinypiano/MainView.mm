//
//  MainView.mm
//  tinypiano
//
//  Created by zq on 2022/9/2.
//

#import "MainView.h"

#include "config.hpp"
#include "drawcontext.hpp"


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
    CGColorRef piano_black_button_color;
    
    float width_ratio;
    float height_ratio;
    
    float keyboard_width;
    float keyboard_height;
    float keyboard_height_rate;
    float fn_area_height_rate;
    struct KeyboardButton *kbButtons[127];
    struct PianoButton *midiButtons[127];
}

- (void) initView {
    keyboard_width = 0;
    keyboard_height = 0;
    keyboard_height_rate = 2.4/4.0;
    fn_area_height_rate = 0.5/4.0;
    
//    key_lbutton_down_ = CColor(131, 178, 35);
//    key_rbutton_down_ = CColor(52, 142, 231);
//    an se zhu ti
//    key_button_up_ = CColor(57, 57, 57);
//    key_button_empty_ = CColor(35, 35, 35);
//    key_text_ = CColor(193, 191, 192);

    back_color = CGColorCreateSRGB(237.0/255.0, 237.0/255.0, 237.0/255.0, 1.0);
    
    keybtn_color = CGColorCreateSRGB(198.0/255.0, 198.0/255.0, 198.0/255.0, 1.0);
    kempty_color = CGColorCreateSRGB(220.0/255.0, 220.0/255.0, 220.0/255.0, 1.0);
    keytxt_color = CGColorCreateSRGB(62.0/255.0, 64.0/255.0, 63.0/255.0, 1.0);
    
    mode_color = CGColorCreateSRGB(200.0/255.0, 157.0/255.0, 200.0/255.0, 1.0);
    sostenuto_color = mode_color;
    adj_lotc_color = CGColorCreateSRGB(131.0/255.0, 180.0/255.0, 251.0/255.0, 1.0);
    adj_rotc_color = adj_lotc_color;
    lintensity_color = CGColorCreateSRGB(99.0/255.0, 187.0/255.0, 208.0/255.0, 1.0);
    rintensity_color = lintensity_color;
    
    piano_black_button_color = CGColorCreateSRGB(100.0/255.0, 100.0/255.0, 100.0/255.0, 1);
    
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
    
    // Initialize keyboard layout
    [self initKeyboardButtons];
    [self initPianoKeyboardButtons];
}

- (void) initKeyboardButtons {
    KeyboardLayout layout = config_get_keyboard_layout();
    KeyboardMapping mapping = config_get_curr_keyboard_mapping();
    for (int i = 0; i < layout.num; i++) {
        auto& key = layout.keys[i];
        auto& map = mapping.keys[int(key.code)];
        self->kbButtons[key.code] = new KeyboardButton();
        auto* button = self->kbButtons[key.code];
        button->map.note = map.note;
        button->map.pith = map.pith;
        button->map.modifier = map.modifier;
        button->map.channel = map.channel;
        button->frame = CGRectMake(key.x, key.y, key.w, key.h);
        std::string text;
        if (button->map.note > 100)
            text = config_get_fn_text(FunctionId(button->map.note), button->map.modifier);
        else
            text = std::to_string(button->map.note);
        button->text = CFStringCreateWithCString(kCFAllocatorDefault, text.c_str(), kCFStringEncodingUTF8);
    }
    // 0.25 is right margin
    self->keyboard_width = layout.width + 0.25;
    self->keyboard_height = layout.height;
    
    width_ratio = self.frame.size.width / self->keyboard_width;
    height_ratio = (self.frame.size.height * keyboard_height_rate) / self->keyboard_height;
}

- (void) initPianoKeyboardButtons {
    // 0: white key, 1: black key
    const int layout[] = {0, 1, 0, 1, 0, 0, 1, 0, 1, 0, 1, 0};
    
    float piano_keyboard_height = (keyboard_height / keyboard_height_rate)
                                * (1-(keyboard_height_rate+fn_area_height_rate)) - 0.25;
    
    float white_width = (keyboard_width - 0.55) / 52;
    float black_width = white_width * 0.65;
    
    float offx = 0.3;
    float y = keyboard_height + (self->keyboard_height / keyboard_height_rate) * fn_area_height_rate + 0.25;
    for (int i = 21; i < 21+88; i++) {
        midiButtons[i] = new PianoButton();
        midiButtons[i]->is_black = layout[i%12];
        if (midiButtons[i]->is_black) {
            midiButtons[i]->frame = CGRectMake(offx - (white_width*0.3), y,
                                                black_width, piano_keyboard_height * 0.55);
        } else {
            midiButtons[i]->frame = CGRectMake(offx, y, white_width, piano_keyboard_height);
            offx += white_width;
        }
    }
}

- (void) drawKeyboard:(DrawContext&)dcontext {
    CFStringRef font_name = CFStringCreateWithCString(kCFAllocatorDefault, "Lucida Grande", kCFStringEncodingUTF8);
    CTFontRef   font = CTFontCreateWithName(font_name, 12, NULL);
    CTFontRef   mfont = CTFontCreateWithName(font_name, 8, NULL);
    
    for (auto *button : self->kbButtons) {
        if (button == NULL)
            continue;
        
        CGRect rect = dcontext.real_rect(CGRectInset(button->frame, 0.05, 0.05));
        
        if (button->map.note == 0) {
            dcontext.set_fill_color(kempty_color);
            dcontext.fill_rrect(rect, 2, false);
            continue;
        }
        
        dcontext.set_fill_color(keybtn_color);
        dcontext.fill_rrect(rect, 2, false);
        
        if (button->map.note == 0)
            continue;
        
        if (button->map.note < 100) {
            dcontext.draw_note(button->text, button->map.pith, button->map.modifier,
                               rect, font, mfont, keytxt_color, false);
            continue;
        } else {
            dcontext.draw_note(button->text, 0, 0,
                               rect, font, mfont, keytxt_color, false);
        }
        
        FunctionId fn = FunctionId(button->map.note);
        if (fn == kMoveMode) {
            dcontext.set_draw_color(mode_color);
        } else if (fn == kLeftSostenuto || fn == kRightSostenuto) {
            dcontext.set_draw_color(sostenuto_color);
        } else if (fn == kLeftAdjOctave) {
            dcontext.set_draw_color(adj_lotc_color);
        } else if (fn == kRightAdjOctave) {
            dcontext.set_draw_color(adj_rotc_color);
        } else if (fn == kLeftIntensity) {
            dcontext.set_draw_color(lintensity_color);
        } else if (fn == kRightIntensity) {
            dcontext.set_draw_color(rintensity_color);
        }
        dcontext.set_line_width(2);
        dcontext.draw_rrect(CGRectInset(rect, 1, 1), 2, false);
    }
    
    CFRelease(font);
    CFRelease(mfont);
    CFRelease(font_name);
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
        auto *button = midiButtons[i];
        CGRect rect = dcontext.real_rect(button->frame);
        if (!button->is_black) {
            dcontext.set_fill_color(back_color);
            dcontext.fill_rect(rect, false);
            dcontext.draw_rect(rect, false);
        }
        auto* pre_button = midiButtons[i-1];
        if (pre_button != NULL && pre_button->is_black) {
            rect = dcontext.real_rect(pre_button->frame);
            dcontext.set_fill_color(piano_black_button_color);
            dcontext.fill_rect(rect, false);
        }
    }
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


@end
