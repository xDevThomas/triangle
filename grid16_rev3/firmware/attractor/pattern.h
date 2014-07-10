/*
 * Triangle Attractor Firmware
 * 
 * Copyright (c) 2014 Micah Elizabeth Scott
 * All Rights Reserved
 */

#pragma once
#include "fc_usb.h"

class fcPattern {
    static const unsigned kNumPixels = 16;

public:

    void init(fcBuffers& buffers)
    {
        // Copy default color LUT to RAM.
        for (unsigned i = 0; i < PACKETS_PER_LUT; i++) {
            usb_packet_t *p = usb_malloc();
            p->len = sizeof p->buf;
            memcpy(p->buf, colorLUT + (i * sizeof p->buf), sizeof p->buf);
            buffers.handleUSB(p);
        }
    }

    void update(fcBuffers& buffers)
    {
        // Spread out the work of generating a frame.
        // Generate a frame of video. It fits in a single packet.
        usb_packet_t *p = usb_malloc();
        p->len = sizeof p->buf;
        p->buf[0] = 0x20;
        for (unsigned pixel = 0; pixel < kNumPixels; pixel++) {

            uint8_t r = 255;
            uint8_t g = 128;
            uint8_t b = millis() / 10;

            p->buf[1 + pixel * 3] = r;
            p->buf[2 + pixel * 3] = g;
            p->buf[3 + pixel * 3] = b;
        }
        buffers.handleUSB(p);
    }

private:
    // Data tables
    static const uint8_t colorLUT[];

};
