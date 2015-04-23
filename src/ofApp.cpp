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
    imageRect.set(0, 0, kinect.width, kinect.height);

    grayThreshNear.allocate(depthImage.width, depthImage.height);
    grayThreshFar.allocate(depthImage.width, depthImage.height);

    roiRect.set(0, 0, 1, 1);
    nearThreshold = 230;
    farThreshold = 190;
    selectingRoi = false;
    
    drawGui = false;
    
//    gui = *gui.setup();
    gui.setup();

    balloons.push_back(shared_ptr<Balloon>(new Balloon(0, gui)));
    balloons.push_back(shared_ptr<Balloon>(new Balloon(1, gui)));
    
    gui.loadFromFile("settings.xml");
//    for (auto it = balloons.begin(); it != balloons.end(); ++it) {
//        it->get()->gui.loadFromFile("settings.xml");
//    }
//    gui.setup();
}

void ofApp::exit() {
    gui.saveToFile("settings.xml");
    
    OscSender::destroy();
//    for (auto it = balloons.begin(); it != balloons.end(); ++it) {
//        it->get()->gui.saveToFile("settings.xml");
//    }
}

//--------------------------------------------------------------
void ofApp::update(){

    
    kinect.update();
    
    // there is a new frame and we are connected
    if(kinect.isFrameNew()) {
        
        // load grayscale depth image from the kinect source
        depthImage.setFromPixels(kinect.getDepthPixels());
        
//        static ofxCvGrayscaleImage grayThreshNear, grayThreshFar;
//        if (!grayThreshNear.bAllocated) grayThreshNear.allocate(depthImage.width, depthImage.height);
//        if (!grayThreshFar.bAllocated) grayThreshFar.allocate(depthImage.width, depthImage.height);

        grayThreshNear = depthImage;
        grayThreshFar = depthImage;
        grayThreshNear.threshold(nearThreshold, true);
        grayThreshFar.threshold(farThreshold);
        grayImage.resetROI();
        cvAnd(grayThreshNear.getCvImage(), grayThreshFar.getCvImage(), grayImage.getCvImage(), NULL);
        grayImage.flagImageChanged();
        
        int d = 60;
        if (roiRect.getArea() > 50) {
            grayImage.setROI(roiRect);
            contourFinder.findContours(grayImage, 10, (kinect.width*kinect.height)/2, 20, false);
        }
        
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
    
    ofPushMatrix();
    ofTranslate(ofMap(roiRect.position, kinectRect, imageRect));
    contourFinder.draw(imageRect);
    ofPopMatrix();
    
    
    for (auto it = balloons.begin(); it != balloons.end(); ++it) {
        Balloon *b = it->get();
        
        
        b->draw(kinectRect, imageRect);
        
        
    }
    

//    ofPushStyle();
//    ofNoFill();
//    ofSetHexColor(0x00ff00);
//    ofDrawRectangle(ofMap(roiRect, kinectRect, imageRect));
//    
//    ofPopStyle();
    
    
    
    if (drawGui) {
        stringstream ss;
        ss << "near threshold " << nearThreshold << " (press: + -)" << endl;
        ss << "far threshold " << farThreshold << " (press: < >) num blobs found " << contourFinder.nBlobs;
        
        
        ofSetHexColor(0xffffff);
        ofDrawBitmapString(ss.str(), 10, imageRect.height+ 10);
        
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
//            b->roiRect.width = q.x;
//            b->roiRect.height = q.y;
//            b->rectVec->z = q.x;
//            b->rectVec->w = q.y;
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
//
            b->rectVec = ofVec4f(q.x, q.y, b->rectVec->z, b->rectVec->w);
//            b->roiRect.setPosition(ofMap(p, imageRect, kinectRect));
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

}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){ 

}