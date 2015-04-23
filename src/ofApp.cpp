#include "ofApp.h"
#include "helpers.h"
#include "OscSender.h"

//--------------------------------------------------------------
void ofApp::setup(){

    ofSetFrameRate(25);
    
    kinect.setRegistration(true);
    kinect.init();
    kinect.open();
    
    kinectRect.set(0, 0, kinect.width, kinect.height);

    depthImage.allocate(kinect.width, kinect.height);
    grayImage.allocate(kinect.width, kinect.height);
    imageRect.set(0, 0, ofGetWidth(), ofGetHeight());

    roiRect.set(0, 0, 1, 1);
    nearThreshold = 230;
    farThreshold = 190;
    selectingRoi = false;
    
    drawGui = false;
    
    gui.setup();

    balloons.push_back(shared_ptr<Balloon>(new Balloon(0, gui)));
    balloons.push_back(shared_ptr<Balloon>(new Balloon(1, gui)));
    
    gui.loadFromFile("settings.xml");

}

void ofApp::exit() {
    gui.saveToFile("settings.xml");
    
    OscSender::destroy();
}

//--------------------------------------------------------------
void ofApp::update(){

    
    kinect.update();
    
    if(kinect.isFrameNew()) {
        
        depthImage.setFromPixels(kinect.getDepthPixels());
        
        static ofxCvGrayscaleImage grayThreshNear, grayThreshFar;
        if (!grayThreshNear.bAllocated) grayThreshNear.allocate(depthImage.width, depthImage.height);
        if (!grayThreshFar.bAllocated) grayThreshFar.allocate(depthImage.width, depthImage.height);

        grayThreshNear = depthImage;
        grayThreshFar = depthImage;
        grayThreshNear.threshold(nearThreshold, true);
        grayThreshFar.threshold(farThreshold);
        grayImage.resetROI();
        cvAnd(grayThreshNear.getCvImage(), grayThreshFar.getCvImage(), grayImage.getCvImage(), NULL);
        grayImage.flagImageChanged();
        
        
        for (auto it = balloons.begin(); it != balloons.end(); ++it) {
            
            it->get()->update(grayImage);
        }
    }


    ofSetWindowTitle(ofToString(ofGetFrameRate()));
}



//--------------------------------------------------------------
void ofApp::draw(){

    ofBackgroundHex(0x2222222);
    
    ofSetHexColor(0xffffff);
    grayImage.draw(imageRect);
    
    
    
    for (auto it = balloons.begin(); it != balloons.end(); ++it) {
        Balloon *b = it->get();
        
        b->draw(kinectRect, imageRect);
    }

    
    
    if (drawGui) {
        stringstream ss;
        ss << "near threshold " << nearThreshold << " (press: + -)" << endl;
        ss << "far threshold " << farThreshold << " (press: < >)" << endl;
        
        
        ofSetHexColor(0x008899);
        ofDrawBitmapString(ss.str(), 10, imageRect.height- 60);
        
        gui.draw();
    }
    

}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){

    switch(key) {
        case '>':
        case '.':
            farThreshold ++;
            if (farThreshold > 255) farThreshold = 255;
            break;
            
        case '<':
        case ',':
            farThreshold --;
            if (farThreshold < 0) farThreshold = 0;
            break;

        case '+':
        case '=':
            nearThreshold ++;
            if (nearThreshold > 255) nearThreshold = 255;
            break;
            
        case '-':
            nearThreshold --;
            if (nearThreshold < 0) nearThreshold = 0;
            break;
            
        case 'r':
            selectingRoi = true;
            break;
        case ' ':
            drawGui = !drawGui;
            break;
    }

    if (key >= '0' && (key - '0') < balloons.size()) {
        balloons[key - '0'].get()->selectingRoi = true;
    }
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){
    
    if (key >= '0' && (key - '0') < balloons.size()) {
        balloons[key - '0'].get()->selectingRoi = false;
    }

}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y){

}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){

    ofVec2f p(x, y);
    
    for (auto it = balloons.begin(); it != balloons.end(); ++it) {
        Balloon *b = it->get();
        if (b->selectingRoi) {
            ofVec2f q = ofMap(p-mouseDownPos, imageRect, kinectRect);
            b->rectVec = ofVec4f(b->rectVec->x, b->rectVec->y, q.x, q.y);
        }
    }

}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){

    ofVec2f p(x, y);
    
    for (auto it = balloons.begin(); it != balloons.end(); ++it) {
        Balloon *b = it->get();
        if (b->selectingRoi) {
            ofVec2f q = ofMap(p, imageRect, kinectRect);

            b->rectVec = ofVec4f(q.x, q.y, b->rectVec->z, b->rectVec->w);
        }
    }
    
    mouseDownPos = p;
}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){

    for (auto it = balloons.begin(); it != balloons.end(); ++it) {
        Balloon *b = it->get();
        if (b->selectingRoi) {
            b->roiRect.standardize();
            
        }
    }

}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){
    imageRect.width = w;
    imageRect.height = h;
}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){ 

}