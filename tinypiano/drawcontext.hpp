//
//  drawcontext.hpp
//  tinypiano
//
//  Created by zq on 2022/9/10.
//

#ifndef drawcontext_hpp
#define drawcontext_hpp

#include <stdio.h>

#import <CoreGraphics/CGContext.h>
#import <CoreText/CoreText.h>

class DrawContext {
    
public:
    DrawContext(CGContextRef context, CGSize size);
    virtual ~DrawContext();
    
    void set_wh_ratio(float wr, float hr) {
        width_ratio_ = wr;
        height_ratio_ = hr;
    }
    void set_coord_reverse(bool reverse) {
        coord_reverse_ = reverse;
    }
    
public:
    void set_draw_color(CGColorRef color);
    void set_fill_color(CGColorRef color);
    void set_line_width(float width);
    
    void draw_rect(CGRect rect, bool zoom = true);
    void fill_rect(CGRect rect, bool zoom = true);
    void draw_rrect(CGRect rect, float radius, bool zoom = true);
    void fill_rrect(CGRect rect, float radius, bool zoom = true);
    void draw_line(CGPoint from, CGPoint to, bool zoom = true);
    void fill_ellipse_in_rect(CGRect rect, bool zoom = true);
    void fill_ellipse(CGPoint point, float radius, bool zoom = true);
    void fill_polygon(CGPoint points[], int count, bool zoom = true);
    
    CTLineRef create_line(CFStringRef str, CTFontRef font, CGColorRef color);
    void draw_line(CTLineRef line, CGPoint point, bool zoom = true);
    void destroy_line(CTLineRef line);
    
    // custom
    void draw_note(CFStringRef note, int pith, int modifier, CGRect rect,
                   CTFontRef font, CTFontRef mfont, CGColorRef color, bool zoom = true);
    enum MButtonStyle {
        kTriangle,
        kParallelLine,
        kRect,
        kCircle,
    };
    void draw_mbutton(MButtonStyle style, CGRect rect, CGColorRef back, CGColorRef fore);
    
    
public:
    CGRect real_rect(CGRect rect);
    CGPoint real_point(CGPoint point);
    void real_rect(CGRect* rect);
    void real_point(CGPoint* point);
    void real_points(CGPoint *points, int count);
    
private:
    float width_ratio_;
    float height_ratio_;
    CGContextRef context_;
    CGSize context_size_;
    
    bool coord_reverse_;
};

#endif /* drawcontext_hpp */
