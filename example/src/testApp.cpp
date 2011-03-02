/*
 *  testApp.cpp
 *  Fakam - Fake camera driver using shared memory
 *	Based on macam - http://webcam-osx.sourceforge.net/
 *
 *  Created by Roger on 18/08/10.
 *  Copyright 2010 Studio Avante. All rights reserved.
 *
 */

#include "testApp.h"

void draw_fps();


//--------------------------------------------------------------
void testApp::setup()
{
#ifdef RELEASE
	ofSetFrameRate(30);
#endif
	ofSetBackgroundAuto(false);		// erase manually
	ofDisableTextureEdgeHack();		// roger: solves ofTexture minor scale/blur
	ofDisableArbTex();
	ofFill();
	glDisable(GL_DEPTH_TEST);
	
	counter = 0;
}

//--------------------------------------------------------------
void testApp::update(){
	counter++;
}

//--------------------------------------------------------------
void testApp::draw(){
	int x, y;
	
	ofBackground(255,0,0);
	
	float cut_x = 1.0;
	float cut_y = fabs(sin(TWO_PI * ((counter%60)/60.0f)));
	ofSetColor(0,255,255);
	ofRect(0,0,cut_x*ofGetWidth(),cut_y*ofGetHeight());
	
	draw_fps();
	
	// Draw to Facam!
	fakam.copyScreen();
}

void draw_fps()
{
	float x = 1.0;
	float y = ofGetHeight()-3.0;
	ofSetColor(255,255,255,255);
	ofDrawBitmapString(ofToString(ofGetFrameRate(), 1), x, y);
}


//--------------------------------------------------------------
void testApp::keyPressed  (int key){
	//printf("KEY [%d] [%c]\n",key,key);
	switch (key){
		case ' ':
			break;
	}
}

//--------------------------------------------------------------
void testApp::mouseMoved(int x, int y ){
}

//--------------------------------------------------------------
void testApp::mouseDragged(int x, int y, int button){
}

//--------------------------------------------------------------
void testApp::mousePressed(int x, int y, int button){
}

//--------------------------------------------------------------
void testApp::mouseReleased(int x, int y, int button){
	
}

//--------------------------------------------------------------
void testApp::windowResized(int w, int h){
	
}

