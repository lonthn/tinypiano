//
//  midievent.hpp
//  tinypiano
//
//  Created by zq on 2022/11/8.
//

#ifndef midievent_hpp
#define midievent_hpp

#include "vst/aeffectx.h"

void midievent_open();
void midievent_close();

bool midievent_send(VstMidiEvent *event);

int  midievent_read(VstEvents *events, int num);


#endif /* midievent_hpp */
