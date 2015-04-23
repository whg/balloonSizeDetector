//
//  Balloon.h
//  balloonSizeDetector
//
//  Created by Will Gallia on 22/04/2015.
//
//

#pragma once

#include "ofMain.h"
#include "ofxOpenCv.h"
#include "ofxGui.h"
#include "helpers.h"
#include "OscSender.h"

class Balloon {

    ofxCvContourFinder contourFinder;

    ofxLabel label;
    ofParameter<ofColor> col;
    int number;
    float updateFrequency;
    float lastUpdateTime;
    
public:

    ofRectangle roiRect;
    ofParameter<ofVec4f> rectVec;
    bool selectingRoi;
    ofxGuiGroup gui;

    
    typedef struct {
        float area, circleness;
    } BalloonShape;
    
    
    
    Balloon() {}
    
    Balloon(int number, ofxPanel &panel, float updateFrequency=2) {
        
        gui.setup("balloon" + ofToString(number));
        gui.add(col.set("colour", 100, ofColor(0, 0) ,255));
        gui.add(label.setup("area", ofToString(0)));
        gui.add(rectVec.set("rect", ofVec4f(10), ofVec4f(0), ofVec4f(300)));
        gui.loadFromFile("settings.xml");
        panel.add(&gui);
        
        this->number = number;
        this->updateFrequency = updateFrequency;
        
    }
    
    ~Balloon() {
        cout << "desc balloon" << endl;
    }

    

    void update(ofxCvGrayscaleImage &img) {
        roiRect = vec4f2Rectangle(rectVec.get());
        
        if (roiRect.getArea() > 50) {
            
            ofRectangle stroi = roiRect.getStandardized();
            img.setROI(stroi);
            contourFinder.findContours(img, 10, stroi.getArea(), 1, false);
            img.resetROI();
            
            float now = ofGetElapsedTimef();
            if ((now - lastUpdateTime) > 1/updateFrequency) {
                lastUpdateTime = now;
                
                BalloonShape bs = getShape();
                
                ofxOscMessage m;
                m.setAddress("/balloon");
                m.addIntArg(number);
                m.addFloatArg(bs.area);
                m.addFloatArg(bs.circleness);
                OscSender::get()->sendMessage(m);
                cout << "sent message for" << number << endl;
            }
            
            
        }
        
//        rectVec.set(ofVec4f(roiRect.position.x, roiRect.position.y, roiRect.width, roiRect.height));
    }
    
    BalloonShape getShape() {
        BalloonShape bs = { .area = 0, .circleness = 0 };
        
        if (contourFinder.blobs.size()) {
            ofPolyline line;
            line.setClosed(true);
            
            // from observation, it looks like you always need to reverse the contour
            vector<ofPoint> pts = contourFinder.blobs[0].pts;
            for (int i = pts.size()-1; i >= 0; i--) {
                line.addVertex(pts[i]);
            }
            
            float area = line.getArea();
            float perimeter = line.getPerimeter();
            float ar = sqrtf(area / PI);
            float pr = perimeter / 2.0 / PI;
            bs.circleness = ar / pr;
            bs.area = area;
        }
        return bs;
    }
    
    void draw(ofRectangle &fromRect, ofRectangle &toRect) {
        ofPushMatrix();
        ofTranslate(ofMap(roiRect.getStandardized().position, fromRect, toRect));
        contourFinder.draw(toRect);
        ofPopMatrix();
        
        ofPushStyle();
        ofNoFill();
        ofSetColor(col);
        ofDrawRectangle(ofMap(roiRect, fromRect, toRect));
        
        ofPopStyle();
    }
    
    void drawGui() {
        gui.draw();
    }
    
};