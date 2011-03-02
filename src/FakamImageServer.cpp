/*
 *  FakamImageServer.cpp
 *  Fakam - Fake camera driver using shared memory
 *	Based on macam - http://webcam-osx.sourceforge.net/
 *
 *  Created by Roger on 25/08/10.
 *  Copyright 2010 Studio Avante. All rights reserved.
 *
 */

#include "FakamImageServer.h"

FakamImageServer::FakamImageServer() : FakamImage(true)
{
	// not initialized
	if (shmStatus != SHM_OK)
		return;
	
	// Alloc local buffer
	lastBuffer = (unsigned char*) malloc(addr_header->dataSize);
	if (lastBuffer == NULL)
		shmStatus = SHM_MALLOC_ERROR;
	
	// No client connected
	lastBufferTime = 0;
	
	// Init header
	addr_header->bpp		= SHM_IMAGE_BPP;		// RGB
	addr_header->width		= SHM_IMAGE_WIDTH;
	addr_header->height		= SHM_IMAGE_HEIGHT;
	addr_header->dataSize	= SHARED_BUF_SIZE;
	addr_header->clientPid	= 0;		// no-one connected
	addr_header->filled		= false;	// empty
}
FakamImageServer::~FakamImageServer()
{
	free((void*)lastBuffer);
}

// Any client connected?
bool FakamImageServer::connected()
{
	if (shmStatus != SHM_OK)
		return false;
	else if (addr_header->clientPid == 0)
		return false;
	else
		return true;
}

//
// Read frame from shared memory
// Return if read new frame or not
//
// Must ALWAYS copy data to outBuffer
// So if no data is available, return last buffer
bool FakamImageServer::read(unsigned char *outBuffer)
{
	if (shmStatus != SHM_OK)
		return false;
	
	// Get time
	struct timeval time;
	gettimeofday( &time, NULL );
	unsigned long now = time.tv_usec/1000 + time.tv_sec*1000;
		
	// Read new frame
	bool newFrame = false;
	if (addr_header->filled == true)
	{
		memcpy(lastBuffer, addr_data, addr_header->dataSize);
		// Empty buffer
		addr_header->filled = false;
		// save frame time
		lastBufferTime = now;
		newFrame = true;
	}
	// Check client Timeout
	else if ( (now - lastBufferTime) >= SHARED_CLIENT_TIMEOUT )
	{
		addr_header->clientPid = 0;
	}
	
	// Copy data
	memcpy(outBuffer, lastBuffer, addr_header->dataSize);
	// return new frame
	return newFrame;
}


