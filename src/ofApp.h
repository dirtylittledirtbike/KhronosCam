#pragma once

#include "ofMain.h"
#include "ofxGui.h"
#include "ofxOpenCv.h"

class ofApp : public ofBaseApp{
    
public:
    void setup();
    void update();
    void draw();
    void audioIn(ofSoundBuffer &input);
    void keyPressed(int key);
    
    ofSoundStream stream;
    
    float smoothedVol;
    float unsmoothed;
    float distance;
    float smoothedVolb;
    
  
    
    bool bUseSmooth;
    ofVideoGrabber grabber;
    ofPixels grabberPix;
    ofPixels finalFrame;
    ofTexture finalTex;
    ofPixels grabberBuffer;
    //ADAM GLITCH
    ofPixels bufferThree;
    ofPixels prevPix;
    
    
    
    
    std::vector<ofPixels> frames;
    
    int average;
    int numFrames;
    int frameHeight;
    int frameWidth;
    
    ofxPanel panel;
    ofParameter<int> soundThresh;
    ofParameter<int> pixelThresh;
    ofParameter<int> movementThresh;
    ofParameter<int> fluidThresh;
    
};
