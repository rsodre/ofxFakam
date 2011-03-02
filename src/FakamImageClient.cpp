/*
 *  FakamImageClient.cpp
 *  Fakam - Fake camera driver using shared memory
 *	Based on macam - http://webcam-osx.sourceforge.net/
 *
 *  Created by Roger on 25/08/10.
 *  Copyright 2010 Studio Avante. All rights reserved.
 *
 */

#include "FakamImageClient.h"
#include <OpenGL/gl.h>

FakamImageClient::FakamImageClient() : FakamImage(false)
{
	myPid = getpid();
	buffer = (unsigned char*) malloc(SHM_IMAGE_WIDTH*SHM_IMAGE_HEIGHT*SHM_IMAGE_BPP);
}
FakamImageClient::~FakamImageClient()
{
	// disconnect!
	addr_header->clientPid = 0;
	free(buffer);
	printf("FAKAM: disconnect!\n");
}

//
// Write frame from shared memory
// Return if wrote or not
//
// if (inBuffer == NULL), copy from screen
//
bool FakamImageClient::write(unsigned char *inBuffer)
{
	if (shmStatus != SHM_OK)
		return false;
	
	// Wrong image size
	//if (w != addr_header->width || h != addr_header->height)
	//	return false;
	
	// New Connection
	if (addr_header->clientPid == 0)
		addr_header->clientPid = myPid;
	// Am I connected?
	else if (addr_header->clientPid != myPid)
		return false;
	
	// Buffer is filled!
	if (addr_header->filled == true)
		return false;
	
	// Copy image from inBuffer
	if (inBuffer)
	{
		memcpy(addr_data, inBuffer, addr_header->dataSize);
	}
	// Copy image from framebuffer
	else
	{
		glPushClientAttrib( GL_CLIENT_PIXEL_STORE_BIT );	// be nice to anyone else who might use pixelStore
		glPixelStorei(GL_PACK_ALIGNMENT, 1);				// set read non block aligned...
		glReadPixels(0, 0, SHM_IMAGE_WIDTH, SHM_IMAGE_HEIGHT, GL_RGB, GL_UNSIGNED_BYTE, buffer);
		glPopClientAttrib();
		// Invert top <> down
		size_t linesz = (SHM_IMAGE_WIDTH * SHM_IMAGE_BPP);
		for (int y = 0 ; y <= SHM_IMAGE_HEIGHT ; y++)
		{
			unsigned char *bp = buffer + (linesz * y);
			unsigned char *sp = addr_data + (linesz * (SHM_IMAGE_HEIGHT-y-1));
			memcpy(sp, bp, linesz);
		}
	}
	// Fill flag
	addr_header->filled = true;
	// Ok!
	return true;	
}

