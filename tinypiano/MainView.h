//
//  MainView.h
//  tinypiano
//
//  Created by luo.zq on 2022/9/2.
//

#import <Foundation/Foundation.h>
#import <Cocoa/Cocoa.h>

//NS_ASSUME_NONNULL_BEGIN

@interface MainView : NSView

- (void) initView;
- (bool) keyDownHandler:(NSEvent *)event;
- (bool) keyUpHandler:(NSEvent *)event;

@end

//NS_ASSUME_NONNULL_END
