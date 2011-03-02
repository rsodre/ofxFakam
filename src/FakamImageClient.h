/*
 *  FakamImageClient.h
 *  Fakam - Fake camera driver using shared memory
 *	Based on macam - http://webcam-osx.sourceforge.net/
 *
 *  Created by Roger on 25/08/10.
 *  Copyright 2010 Studio Avante. All rights reserved.
 *
 */
#pragma once

#include "FakamImage.h"


class FakamImageClient : public FakamImage {
public:
	
	FakamImageClient();
	~FakamImageClient();

	bool write(unsigned char *buffer=NULL);

private:
	
	int myPid;
	
	unsigned char *buffer;
};
