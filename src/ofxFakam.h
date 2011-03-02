/*
 *  ofxFakam.cpp
 *  Fakam - Fake camera driver using shared memory
 *	Based on macam - http://webcam-osx.sourceforge.net/
 *
 *  Created by Roger on 18/08/10.
 *  Copyright 2010 Studio Avante. All rights reserved.
 *
 */
#pragma once

#include "ofMain.h"
#include "FakamImageClient.h"


class ofxFakam {
public:
	
	ofxFakam();
	~ofxFakam(); 

	void fill(ofColor c, bool force=false);
	void copyScreen(bool force=false);
	
private:

	FakamImageClient	*shm;
	ofImage				imgBuffer;
	
};











