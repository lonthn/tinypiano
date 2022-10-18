//
//  ViewController.m
//  tinypiano
//
//  Created by zq on 2022/9/1.
//

#import "ViewController.h"
#import "MainView.h"

@implementation ViewController
{
    MainView *mainView;
}

- (void)viewDidLoad {
    [super viewDidLoad];
    MainView* view = (MainView*) self.view;
    [view initView];
}


- (void)setRepresentedObject:(id)representedObject {
    [super setRepresentedObject:representedObject];
    // Update the view, if already loaded.
}

@end
