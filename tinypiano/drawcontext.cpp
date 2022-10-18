//
//  drawcontext.cpp
//  tinypiano
//
//  Created by zq on 2022/9/10.
//

#include "drawcontext.hpp"


const CFStringRef UP_MODIFIER = CFStringCreateWithCString(kCFAllocatorDefault, "♯", kCFStringEncodingUTF8);
const CFStringRef DN_MODIFIER = CFStringCreateWithCString(kCFAllocatorDefault, "♭", kCFStringEncodingUTF8);


DrawContext::DrawContext(CGContextRef context, CGSize size) {
    context_ = context;
    context_size_ = size;
}

DrawContext::~DrawContext() {
}

void DrawContext::set_draw_color(CGColorRef color) {
    CGContextSetStrokeColorWithColor(context_, color);
}
void DrawContext::set_fill_color(CGColorRef color) {
    CGContextSetFillColorWithColor(context_, color);
}
void DrawContext::set_line_width(float width) {
    CGContextSetLineWidth(context_, width);
}

void DrawContext::draw_rect(CGRect rect, bool zoom) {
    if (zoom) {
        real_rect(&rect);
    }
    if (coord_reverse_) {
        rect.origin.y = context_size_.height - rect.origin.y - rect.size.height;
    }
    CGContextStrokeRect(context_, rect);
}

void DrawContext::fill_rect(CGRect rect, bool zoom) {
    if (zoom) {
        real_rect(&rect);
    }
    if (coord_reverse_) {
        rect.origin.y = context_size_.height - rect.origin.y - rect.size.height;
    }
    CGContextFillRect(context_, rect);
}

void DrawContext::draw_rrect(CGRect rect, float radius, bool zoom) {
    if (zoom) {
        real_rect(&rect);
    }
    if (coord_reverse_) {
        rect.origin.y = context_size_.height - rect.origin.y - rect.size.height;
    }
    
    float x1 = rect.origin.x;
    float y1 = rect.origin.y;
    float x2 = x1+rect.size.width;
    float y2 = y1;
    float x3 = x2;
    float y3 = y1+rect.size.height;
    float x4 = x1;
    float y4 = y3;
    
    CGContextMoveToPoint(context_, x1, y1+radius);
    CGContextAddArcToPoint(context_, x1, y1, x1+radius, y1, radius);
    CGContextAddArcToPoint(context_, x2, y2, x2, y2+radius, radius);
    CGContextAddArcToPoint(context_, x3, y3, x3-radius, y3, radius);
    CGContextAddArcToPoint(context_, x4, y4, x4, y4-radius, radius);
    CGContextAddLineToPoint(context_, x4, y4-radius);
    CGContextAddLineToPoint(context_, x4, y1+radius);
    CGContextStrokePath(context_);
}

void DrawContext::fill_rrect(CGRect rect, float radius, bool zoom) {
    if (zoom) {
        real_rect(&rect);
    }
    if (coord_reverse_) {
        rect.origin.y = context_size_.height - rect.origin.y - rect.size.height;
    }
    
    float x1 = rect.origin.x;
    float y1 = rect.origin.y;
    float x2 = x1 + rect.size.width;
    float y2 = y1;
    float x3 = x2;
    float y3 = y1 + rect.size.height;
    float x4 = x1;
    float y4 = y3;
    
    CGContextMoveToPoint(context_, x1, y1+radius);
    CGContextAddArcToPoint(context_, x1, y1, x1+radius, y1, radius);
    CGContextAddArcToPoint(context_, x2, y2, x2, y2+radius, radius);
    CGContextAddArcToPoint(context_, x3, y3, x3-radius, y3, radius);
    CGContextAddArcToPoint(context_, x4, y4, x4, y4-radius, radius);
    CGContextAddLineToPoint(context_, x4, y1+radius);
    CGContextFillPath(context_);
}

void DrawContext::draw_line(CGPoint from, CGPoint to, bool zoom) {
    if (zoom) {
        real_point(&from);
        real_point(&to);
    }
    if (coord_reverse_) {
        from.y = context_size_.height - from.y;
        to.y = context_size_.height - to.y;
    }
    CGPoint points[2] = {from, to};
    CGContextStrokeLineSegments(context_, points, 2);
}

void DrawContext::fill_ellipse_in_rect(CGRect rect, bool zoom) {
    if (zoom) {
        real_rect(&rect);
    }
    if (coord_reverse_) {
        rect.origin.y = context_size_.height - rect.origin.y - rect.size.height;
    }
    CGContextFillEllipseInRect(context_, rect);
}

void DrawContext::fill_ellipse(CGPoint point, float radius, bool zoom) {
    if (zoom) {
        real_point(&point);
        radius *= height_ratio_;
    }
    if (coord_reverse_) {
        point.y = context_size_.height - point.y;
    }
    CGRect rect = CGRectMake(point.x-radius, point.y-radius, radius*2, radius*2);
    CGContextFillEllipseInRect(context_, rect);
}

void DrawContext::fill_polygon(CGPoint points[], int count, bool zoom) {
    if (count < 3)
        return;
    
    if (zoom) {
        real_points(points, count);
    }
    if (coord_reverse_) {
        for (int i = 0; i < count; i++) {
            points[i].y = context_size_.height - points[i].y;
        }
    }
    
    CGContextAddLines(context_, points, count);
    CGContextClosePath(context_);
    CGContextFillPath(context_);
}

CTLineRef DrawContext::create_line(CFStringRef str, CTFontRef font, CGColorRef color) {
    CFStringRef keys[] = { kCTFontAttributeName, kCTForegroundColorAttributeName };
    CFTypeRef values[] = { font, color};
    CFDictionaryRef attributes = CFDictionaryCreate(kCFAllocatorDefault,
                                                    (const void**)&keys, (const void**)&values,
                                                    sizeof(keys) / sizeof(keys[0]),
                                                    &kCFTypeDictionaryKeyCallBacks,
                                                    &kCFTypeDictionaryValueCallBacks);
    
    CFAttributedStringRef attrString = CFAttributedStringCreate(kCFAllocatorDefault, str, attributes);
    CFRelease(attributes);
     
    return CTLineCreateWithAttributedString(attrString);
}

void DrawContext::draw_line(CTLineRef line, CGPoint point, bool zoom) {
    if (zoom) {
        real_point(&point);
    }
    CGFloat a, d;
    CTLineGetTypographicBounds(line, &a, &d, NULL);
    if (coord_reverse_) {
        point.y = context_size_.height - point.y - (a - d);
    }
    CGContextSetTextPosition(context_, point.x, point.y);
    CTLineDraw(line, context_);
}

void DrawContext::destroy_line(CTLineRef line) {
    CFRelease(line);
}

CGSize line_get_size(CTLineRef line) {
    CGFloat a, d;
    double width = CTLineGetTypographicBounds(line, &a, &d, NULL);
    return CGSizeMake(width, a - d);
}

void DrawContext::draw_note(CFStringRef note, int pith, int modifier, CGRect rect,
                            CTFontRef font, CTFontRef mfont, CGColorRef color, bool zoom) {
//    if (zoom) {
//        real_rect(&rect);
//    }

    set_fill_color(color);
    
    CTLineRef line = create_line(note, font, color);
    CGSize    tsize = line_get_size(line);
    CGSize    size = CGSizeMake(tsize.width, tsize.height + (abs(pith) * 3));
    
    CGPoint   point = CGPointMake(rect.origin.x + (rect.size.width-size.width)/2,
                                  rect.origin.y + (rect.size.height-size.height)/2);
    if (pith > 0) {
        point.y += abs(pith) * 3;
    }
    draw_line(line, point, false);
    
    if (modifier != 0) {
        CTLineRef mline = create_line(modifier == 1 ? UP_MODIFIER : DN_MODIFIER, mfont, color);
        CGSize    msize = line_get_size(mline);
        CGPoint   mpoint = CGPointMake(point.x - msize.width, point.y - (msize.height/2));
        draw_line(mline, mpoint, false);
        CFRelease(mline);
    }
    
    point.x = rect.origin.x + (rect.size.width - 2) / 2;
    if (pith > 0) {
        for (int i = 0; i < pith; i++) {
            point.y -= 3;
            fill_ellipse_in_rect(CGRectMake(point.x, point.y, 2, 2), false);
        }
    } else if (pith < 0) {
        point.y += tsize.height;
        for (int i = 0; i < abs(pith); i++) {
            point.y += 3;
            fill_ellipse_in_rect(CGRectMake(point.x, point.y, 2, 2), false);
        }
    }
    
    CFRelease(line);
}

void DrawContext::draw_mbutton(MButtonStyle style, CGRect rect, CGColorRef back, CGColorRef fore) {
    //real_rect(&rect);
    
    rect.size.width = rect.size.height;
    set_fill_color(back);
    fill_ellipse_in_rect(rect, false);
    
    set_fill_color(fore);
    set_draw_color(fore);
    CGPoint center = CGPointMake(rect.origin.x + rect.size.width/2,
                                 rect.origin.y + rect.size.height/2);
    if (style == kTriangle) {
        float R = rect.size.width / 4.0;
        float a = R / (sqrt(3.0)/3.0);
        float r = sqrt(3.0)/6.0 * a;
        CGPoint points[3] = {
            CGPointMake(center.x - r, center.y - (a/2)),
            CGPointMake(center.x + R, center.y),
            CGPointMake(center.x - r, center.y + (a/2)),
        };
        fill_polygon(points, 3, false);
    } else if (style == kParallelLine) {
        float distance = rect.size.width / 5.0;
        float height   = rect.size.height * (2.5/5.0);
        set_line_width(2);
        draw_line(CGPointMake(center.x - distance/2, center.y - height/2),
                  CGPointMake(center.x - distance/2, center.y + height/2), false);
        draw_line(CGPointMake(center.x + distance/2, center.y - height/2),
                  CGPointMake(center.x + distance/2, center.y + height/2), false);
    } else if (style == kCircle) {
        float r = rect.size.width / 6.0;
        fill_ellipse(center, r, false);
    } else if (style == kRect) {
        float r = rect.size.width / 6.0;
        fill_rrect(CGRectMake(center.x-r, center.y-r, r*2, r*2), 2, false);
    }
}

CGRect DrawContext::real_rect(CGRect rect) {
    return CGRectMake(rect.origin.x*width_ratio_, rect.origin.y*height_ratio_,
                      rect.size.width*width_ratio_, rect.size.height*height_ratio_);
}

CGPoint DrawContext::real_point(CGPoint point) {
    return CGPointMake(point.x*width_ratio_, point.y*height_ratio_);
}

void DrawContext::real_rect(CGRect* rect) {
    rect->origin.x *= width_ratio_;
    rect->origin.y *= height_ratio_;
    rect->size.width *= width_ratio_;
    rect->size.height *= height_ratio_;
}

void DrawContext::real_point(CGPoint* point) {
    point->x *= width_ratio_;
    point->y *= height_ratio_;
}

void DrawContext::real_points(CGPoint *points, int count) {
    for (int i = 0; i < count; i++) {
        real_point(&points[i]);
    }
}
