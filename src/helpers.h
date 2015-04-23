//
//  Header.h
//  balloonSizeDetector
//
//  Created by Will Gallia on 22/04/2015.
//
//

#pragma once

#include "ofMain.h"

inline ofVec2f ofMap(ofVec2f p, ofRectangle fromRect, ofRectangle toRect){
    
    float x = ofMap(p.x, fromRect.position.x, fromRect.width, toRect.position.x, toRect.width);
    float y = ofMap(p.y, fromRect.position.y, fromRect.height, toRect.position.x, toRect.height);
    return ofVec2f(x, y);
}


inline ofRectangle ofMap(ofRectangle r, ofRectangle fromRect, ofRectangle toRect){
    
    float x = ofMap(r.position.x, fromRect.position.x, fromRect.width, toRect.position.x, toRect.width);
    float y = ofMap(r.position.y, fromRect.position.y, fromRect.height, toRect.position.x, toRect.height);
    float w = ofMap(r.width, fromRect.position.x, fromRect.width, toRect.position.x, toRect.width);
    float h = ofMap(r.height, fromRect.position.y, fromRect.height, toRect.position.x, toRect.height);
    
    return ofRectangle(x, y, w, h);
}

inline ofRectangle vec4f2Rectangle(const ofVec4f &v) {
    return ofRectangle(v.x, v.y, v.z, v.w);
}