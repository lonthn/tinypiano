//
//  midievent.cpp
//  tinypiano
//
//  Created by zq on 2022/11/8.
//

#include "midievent.hpp"

#include <unistd.h>
#include <stdio.h>
#include <stdint.h>
#include <errno.h>
#include <string.h>
#include <sys/socket.h>
#include <fcntl.h>

#define MIDIEVENT_LEN   3
#define MAX_READBUF_LEN (256 * MIDIEVENT_LEN)

#define min(a, b) ((a) < (b) ? (a) : (b))

static bool         isopen_ = false;
static int          iofds_[2];
static uint8_t      readbuf_[MAX_READBUF_LEN];
static int          spill_len_;

static VstMidiEvent midievents_[256];
static int          events_num_;

void midievent_open() {
    if (isopen_) {
        return;
    }
    
    int r = socketpair(AF_UNIX, SOCK_STREAM, 0, iofds_);
    if (r < 0) {
        printf("Can't open the midievent: %s\n", strerror(errno));
        return;
    }

    // Set nonblocking for fd
    int flags = fcntl(iofds_[0], F_GETFL);
    fcntl(iofds_[0], F_SETFL, flags | O_NONBLOCK);

    flags = fcntl(iofds_[1], F_GETFL);
    fcntl(iofds_[1], F_SETFL, flags | O_NONBLOCK);
    
    spill_len_ = 0;
    events_num_ = 0;
    isopen_ = true;
    memset(midievents_, 0, sizeof(VstMidiEvent) * 256);
}

void midievent_close() {
    if (isopen_) {
        return;
    }
    spill_len_ = 0;
    close(iofds_[0]);
    close(iofds_[1]);
    isopen_ = false;
}

bool midievent_send(VstMidiEvent *event) {
    int wn = (int) write(iofds_[1], event->midiData, MIDIEVENT_LEN);
    return wn == MIDIEVENT_LEN;
}

int  midievent_read(VstEvents *events, int num) {
    int rn = (int) read(iofds_[0], &(readbuf_[spill_len_]), MAX_READBUF_LEN - spill_len_);
    if (rn <= 0)
        return 0;

    rn += spill_len_;
    spill_len_ =  (rn % MIDIEVENT_LEN);

    int off = 0;
    int count = (rn / MIDIEVENT_LEN);
    if (count != 0) {
        count = min(count, num);
        events->numEvents = count;
        events->reserved = 0;
        for (int i = 0; i < count; i++) {
            midievents_[i].type = kVstMidiType;
            midievents_[i].byteSize = sizeof(VstMidiEvent);
            midievents_[i].deltaFrames = 0;
            midievents_[i].flags = kVstMidiEventIsRealtime;
            midievents_[i].noteLength = 0;
            midievents_[i].noteOffset = 0;
            midievents_[i].noteOffVelocity = 0;
            midievents_[i].midiData[0] = readbuf_[off++];
            midievents_[i].midiData[1] = readbuf_[off++];
            midievents_[i].midiData[2] = readbuf_[off++];
            midievents_[i].midiData[3] = 0;
            midievents_[i].detune = 0;
            midievents_[i].noteOffVelocity = 0;
            midievents_[i].reserved1 = 0;
            midievents_[i].reserved2 = 0;
            events->events[i] = (VstEvent *) &midievents_[i];
        }
        events->events[count] = nullptr;
    }
    if (spill_len_ != 0) {
        memmove(readbuf_, readbuf_+off, spill_len_);
    }
    return count;
}
