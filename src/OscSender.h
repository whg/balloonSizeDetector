//
//  OscSender.h
//  balloonSizeDetector
//
//  Created by Will Gallia on 23/04/2015.
//
//

#pragma once

#include "ofxOsc.h"

class OscSender {
    
public:
    OscSender();
    ~OscSender();
    static ofxOscSender* get();
    static void destroy();
    void send(ofxOscMessage &message);
    
};