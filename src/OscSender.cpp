//
//  OscSender.cpp
//  balloonSizeDetector
//
//  Created by Will Gallia on 23/04/2015.
//
//

#include "OscSender.h"

ofxOscSender *sender = NULL;

OscSender::OscSender() {
    
}

void OscSender::destroy() {
    
    if (sender != NULL) {
        delete sender;
        cout << "deleted sender" << endl;
    }
}

ofxOscSender* OscSender::get() {
    if (sender == NULL) {
        sender = new ofxOscSender();
        sender->setup("localhost", 5005);
    }
    return sender;
}

void OscSender::send(ofxOscMessage &message) {
    sender->sendMessage(message);
}