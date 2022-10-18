//
//  MainView.h
//  tinypiano
//
//  Created by luo.zq on 2022/9/2.
//

#import <Foundation/Foundation.h>
#import <Cocoa/Cocoa.h>

//NS_ASSUME_NONNULL_BEGIN

struct KeyboardButton {
    CGRect frame;
    CFStringRef text;
    
    struct {
        uint8_t note;
        int8_t  pith;
        int8_t  modifier;
        int8_t  channel;
        bool    left;
    } map;
};

struct PianoButton {
    CGRect frame;
    bool is_black;
};

@interface MainView : NSView

- (void) initView;

@end

//NS_ASSUME_NONNULL_END
