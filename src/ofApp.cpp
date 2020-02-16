#include "ofApp.h"


//--------------------------------------------------------------
void ofApp::setup(){
    numFrames = 500;
    frameHeight = 480;
    frameWidth = 640;
    ofSetFrameRate(60);
    grabber.setup(640, 480);
    grabberPix.allocate(frameWidth, frameHeight, OF_PIXELS_RGB);
    grabberBuffer.allocate(frameWidth, frameHeight, OF_PIXELS_RGB);
    frames.assign(numFrames, ofPixels());
    
    
    int bufferSize = 256;
    smoothedVol = 0.0;
    smoothedVolb = 0.0;
    unsmoothed = 0.0;
    distance = 0;
    
    auto devices = stream.getDeviceList();
    for ( auto &device : devices){
        std::cout << device << std::endl;
    }
    
    for (int i = 0; i < frames.size(); i++){
        //cout << frames[i] << endl;
        frames[i].allocate(frameWidth, frameHeight, OF_PIXELS_RGB);
        // set all pixels in every frame to 0 to start
        frames[i].set(0);
        
    }
    
    ofSoundStreamSettings settings;
    settings.setInDevice(devices[0]);
    settings.setInListener(this);
    settings.sampleRate = 44100;
    settings.numOutputChannels = 0;
    settings.numInputChannels = 2;
    settings.bufferSize = bufferSize;
    stream.setup(settings);
    
    bUseSmooth = true;
    
    panel.setup();
    panel.add(soundThresh.set("sound treshold", numFrames, 0, numFrames));
    //panel.add(fluidThresh.set("fluidity", numFrames-1, 0, numFrames-1));
    //panel.add(movementThresh.set("movement threshold", 1000, 0, 1000));
    panel.add(pixelThresh.set("pixel threshold", 255, 0, 255));
    
    
}

//--------------------------------------------------------------
void ofApp::update(){
    
    grabber.update();
    
    if (grabber.isFrameNew()){
        
        grabberPix = grabber.getPixels();
        //grabberBuffer = grabber.getPixels();
        
        grabberPix.resize(frameWidth, frameHeight);
        grabberBuffer.resize(frameWidth, frameHeight);
        grabberBuffer = grabberPix;
        
        for(int x = 0; x < frameWidth; x++){
            for(int y = 0; y < frameHeight; y++){
                
                int smoothSoundValue = ofMap(smoothedVol, 0.1, 5.0, 0, numFrames - 1, true);
                
                //int smoothSoundValue = ofMap(smoothedVol, 0.2, 5.0, 0, numFrames - 1, true);
                int colorShift = ofMap(smoothedVol, 0.0, 1.0, 0, 479, true);
                
                int index = 3 * (x + y * frameWidth);
                int rValue = grabberBuffer[index + 0];
                int gValue = grabberBuffer[index + 1];
                int bValue = grabberBuffer[index + 2];
                
                
                int xMinus = x - colorShift;
                if (xMinus < 0){
                    xMinus = frameWidth + xMinus;
                }
                
                int xPlus = x + colorShift;
                if (xPlus > frameWidth-1){  //100
                    xPlus = xPlus - frameWidth;
                }
                
                int yMinus = y - colorShift;
                if (yMinus < 0){
                    yMinus = frameHeight + yMinus;
                }
                
                int yPlus = y + colorShift;
                if (yPlus > frameHeight - 1){
                    yPlus = yPlus - frameHeight;
                };
                
                int left = 3 * ( xMinus + y * frameWidth);
                int up = 3 * ( x + yMinus * frameWidth);
                int down = 3 * ( x + yPlus * frameWidth);
                int right = 3 * (xPlus + y * frameWidth);
                
                average = (rValue + gValue + bValue)/3;
                int pixAvgToFrameNumber = ofMap(average, 0, 255, (numFrames - smoothSoundValue) - 1, numFrames - 1, true);
                int smoothSoundThresh = ofMap(smoothedVolb, 0.0, 1.0, 0, numFrames - 1, true);
//                int smoothSoundValue = ofMap(smoothedVol, 0.0, 1.0, 0, numFrames - 1, true);
                
                
                if (smoothSoundThresh < soundThresh ){
                    
                    //int pixAvgToFrameNumber = ofMap(average, 0, 255, (numFrames - smoothSoundValue) - 1, numFrames - 1, true);
                    ofColor pulledColor = frames[pixAvgToFrameNumber].getColor(x,y);
                    grabberBuffer.setColor(x, y, pulledColor);
                    
//                    if(average > pixelThresh){
//                        ofColor pulledColor = frames[0].getColor(x,y);
//                        grabberBuffer.setColor(x, y, pulledColor);
//                    }
                    
                } else {

                    int frameIndex = ofMap(average, 0, 255, (numFrames - smoothSoundValue) - 1, numFrames - 1, true);
                    grabberPix[left + 0] = rValue;
                    grabberPix[right + 1] = bValue;
                    grabberPix[down + 2] = gValue;

                    ofColor pulledColor = frames[frameIndex].getColor(x,y);
                    grabberBuffer.setColor(x, y, pulledColor);
                    
                }
                
                if (average > pixelThresh){
                    
                    ofColor pulledColor = frames[0].getColor(x,y);
                    grabberBuffer.setColor(x, y, pulledColor);
                    
                }
            
                
            }
        }
        
        frames.push_back(grabberPix);

        while (frames.size() > numFrames){
            
            frames.erase(frames.begin());
            
        }
        
    }
    
    finalTex.loadData(grabberBuffer);

}

//--------------------------------------------------------------
void ofApp::draw(){
    
    ofBackgroundGradient(ofColor(0), ofColor(0), OF_GRADIENT_CIRCULAR);
    finalTex.draw(ofGetWidth()/2 - finalTex.getWidth()/2, ofGetHeight()/2 - finalTex.getHeight()/2);
    string frameStr = "frame rate:: " + ofToString(ofGetFrameRate());
    //ofDrawBitmapString(ofToString(smoothedVol), 15, 15);
    ofDrawBitmapString(ofToString(average), 15, 15);
    panel.draw();
    
}

void ofApp::audioIn(ofSoundBuffer &input){
    double currentVol;
    int numCounted = 0;
    int channels = input.getNumChannels();
    
    // calculate RMS (root-mean-square) of audio buffer
    for (std::size_t i = 0; i < input.getNumFrames(); i++){
        
        // collect values at index for left and right channels
        double left = input[i * channels] * 0.5;
        double right = input[i * channels + 1] * 0.5;
        
        // square it to make sure it is always positive
        currentVol += left * left;
        currentVol += right * right;
        numCounted += 2;
    }
    
    // mean-- divide by total counted
    currentVol /= (float)numCounted;
    
    // root -- get the square root
    currentVol = sqrt(currentVol);
    
    unsmoothed = currentVol;
    smoothedVol *= 0.99;
    smoothedVol += currentVol;
    smoothedVolb *= 0.99;
    smoothedVolb += currentVol;
    
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
    if (key == 's'){
        bUseSmooth = !bUseSmooth;
    }
}
