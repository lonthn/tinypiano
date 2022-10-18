//
//  main.m
//  tinypiano
//
//  Created by zq on 2022/9/1.
//

#import <Cocoa/Cocoa.h>

#include "config.hpp"
#include <string>

void load_path(const char* arg) {
    std::string path(arg);
    path = path.substr(0, path.rfind("/") + 1);
    path.append("../Resources/");
    config_set_resource_path(path);
}

int main(int argc, const char * argv[]) {
    load_path(argv[0]);
    
    @autoreleasepool {
        // Setup code that might create autoreleased objects goes here.
    }
    return NSApplicationMain(argc, argv);
}
