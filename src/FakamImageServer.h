/*
 *  FakamImageServer.h
 *  Fakam - Fake camera driver using shared memory
 *	Based on macam - http://webcam-osx.sourceforge.net/
 *
 *  Created by Roger on 25/08/10.
 *  Copyright 2010 Studio Avante. All rights reserved.
 *
 */
#pragma once

#include "FakamImage.h"


class FakamImageServer : public FakamImage {
public:
	
	FakamImageServer();
	~FakamImageServer();

	bool connected();
	bool read(unsigned char *buffer);

private:
	
	// local buffer
	unsigned char	*lastBuffer;
	unsigned long	lastBufferTime;
	
};
