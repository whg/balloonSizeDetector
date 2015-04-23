#pragma once

#include "ofMain.h"
#include "ofxKinect.h"
#include "ofxOpenCv.h"
#include "ofxGui.h"

#include "Balloon.h"

class ofApp : public ofBaseApp{
public:
    void setup();
    void update();
    void draw();
    void exit();
    
    void keyPressed(int key);
    void keyReleased(int key);
    void mouseMoved(int x, int y);
    void mouseDragged(int x, int y, int button);
    void mousePressed(int x, int y, int button);
    void mouseReleased(int x, int y, int button);
    void windowResized(int w, int h);
    void dragEvent(ofDragInfo dragInfo);
    void gotMessage(ofMessage msg);
    
    ofxKinect kinect;
    ofRectangle kinectRect;
    
    ofxCvGrayscaleImage depthImage, grayImage;
    ofRectangle imageRect;
    
//    ofxCvGrayscaleImage grayThreshNear, grayThreshFar;
//    ofxCvGrayscaleImage grayThreshNear, grayThreshFar;
    ofxCvContourFinder contourFinder;
    ofVec2f mouseDownPos;
    int nearThreshold, farThreshold;
    ofRectangle roiRect;
    bool selectingRoi;
    
    vector< shared_ptr<Balloon> > balloons;
    
    ofxPanel gui;
    bool drawGui;
    
};
