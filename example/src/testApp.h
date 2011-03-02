/*
 *  testApp.h
 *  Fakam - Fake camera driver using shared memory
 *	Based on macam - http://webcam-osx.sourceforge.net/
 *
 *  Created by Roger on 18/08/10.
 *  Copyright 2010 Studio Avante. All rights reserved.
 *
 */
#pragma once

#include "ofMain.h"
#include "ofxFakam.h"

class testApp : public ofBaseApp{

public:
	
	void setup();
	void update();
	void draw();
	
	void keyPressed  (int key);
	void mouseMoved(int x, int y );
	void mouseDragged(int x, int y, int button);
	void mousePressed(int x, int y, int button);
	void mouseReleased(int x, int y, int button);
	void windowResized(int w, int h);

	ofxFakam	fakam;
	int			counter;
	
};

