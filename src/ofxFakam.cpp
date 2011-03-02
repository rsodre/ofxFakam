/*
 *  ofxFakam.cpp
 *  Fakam - Fake camera driver using shared memory
 *	Based on macam - http://webcam-osx.sourceforge.net/
 *
 *  Created by Roger on 18/08/10.
 *  Copyright 2010 Studio Avante. All rights reserved.
 *
 */

#include "ofxFakam.h"

ofxFakam::ofxFakam()
{
	// Alloc local image buffer
	imgBuffer.allocate(SHM_IMAGE_WIDTH, SHM_IMAGE_HEIGHT, OF_IMAGE_COLOR);

	// Verify shm
	shm = new FakamImageClient();
	if (shm == NULL)
		printf("FAKAM: shm NULL!!!\n");
	else if (shm->getStatus() != SHM_OK)
		printf("FAKAM: shm error [%s]\n",shm->getStatusDesc());
}
ofxFakam::~ofxFakam()
{
	free(shm);
}

////////////////////////////////////////
//
// DRAW
//

//
// Fill shred memory with color
void ofxFakam::fill(ofColor c, bool force)
{
	// Fill buffer
	imgBuffer.fill(c);
	// write to shm
	shm->write(imgBuffer.getPixels());
}

//
// Send current screen to shared memory
void ofxFakam::copyScreen(bool force)
{
	// Copy screen from framebuffer > ofImage
	//imgBuffer.grabScreen(0, 0, SHM_IMAGE_WIDTH, SHM_IMAGE_HEIGHT);
	// Copy from ofImage > Fakam
	//shm->write(imgBuffer.getPixels());

	// Copy screen direct to Fakam
	shm->write();
}

