/*
 *  FakamImage.cpp
 *  Fakam - Fake camera driver using shared memory
 *	Based on macam - http://webcam-osx.sourceforge.net/
 *
 *  Created by Roger on 25/08/10.
 *  Copyright 2010 Studio Avante. All rights reserved.
 *
 */

#include "FakamImage.h"


// Must match FakamImageStatus
char FakamImageStatusDesc[][128] = {
{ "OK" },					// SHM_OK
{ "SHM_SHMGET_ERROR" },		// SHM_SHMGET_ERROR
{ "SHM_SHMAT_ERROR" },		// SHM_SHMAT_ERROR
{ "SHM_SHMDT_ERROR" },		// SHM_SHMDT_ERROR
{ "SHM_MALLOC_ERROR" },		// SHM_MALLOC_ERROR
{ "SHM_COUNT" }				// SHM_COUNT
};


/////////////////////////////////////////////////////////
//
// FakamImage Class
//
FakamImage::FakamImage(bool server)
{
	addr_header = NULL;
	addr_data = NULL;
	// Alloc SHM
	if (this->allocSharedMemory(server) == false)
		return;
}
FakamImage::~FakamImage()
{
	this->freeSharedMemory();
}

//
// GETTERS
short FakamImage::getWidth()
{
	return addr_header->width;
}
short FakamImage::getHeight()
{
	return addr_header->height;
}
short FakamImage::getStatus()
{
	return shmStatus;
}
char* FakamImage::getStatusDesc()
{
	return FakamImageStatusDesc[shmStatus];
}

/////////////////////////////////////////////////////////
//
// SHARED MEM
//

//
// Get shared memory
bool FakamImage::allocSharedMemory(bool server)
{
	int shmflg = ( server ? IPC_CREAT|0666 : 0666 );
	
	//
	// ALLOC HEADER
	if ((shmid_header = shmget(SHM_KEY_HEADER, sizeof(FakamImageHeader), shmflg)) < 0)
	{
		printf("FAKAM: HEADER: shmget failed!!! errno[%d/%s]\n",errno,strerror(errno));
		shmStatus = SHM_SHMGET_ERROR;
		return false;
	}
    if ((char*)(addr_header = (FakamImageHeader*) shmat(shmid_header, NULL, 0)) == (char *) -1)
	{
		printf("FAKAM: HEADER: shmat failed!!! errno[%d/%s]\n",errno,strerror(errno));
		shmStatus = SHM_SHMAT_ERROR;
		return false;
	}
	printf("FAKAM: HEADER: addr_header[%d] OK!\n",(int)addr_header);
	
	//
	// ALLOC DATA
	if ((shmid_data = shmget(SHM_KEY_DATA, SHARED_BUF_SIZE, shmflg)) < 0)
	{
		printf("FAKAM: DATA: shmget failed!!! errno[%d/%s]\n",errno,strerror(errno));
		shmStatus = SHM_SHMGET_ERROR;
		return false;
	}
    if ((char*)(addr_data = (unsigned char*) shmat(shmid_data, NULL, 0)) == (char *) -1)
	{
		printf("FAKAM: DATA: shmat failed!!! errno[%d/%s]\n",errno,strerror(errno));
		shmStatus = SHM_SHMAT_ERROR;
		return false;
	}
	printf("FAKAM: DATA:   addr_data[%d] OK!\n",(int)addr_header);

	// Ok!
	this->erase();
	shmStatus = SHM_OK;
	return true;
}

//
// Get shared memory
void FakamImage::freeSharedMemory()
{
	// Release shared memory
	if (addr_header > 0)
	{
		if (shmdt(addr_header) < 0)
		{
			printf("FAKAM: addr_header shmdt failed!!!\n");
			shmStatus = SHM_SHMDT_ERROR;
			return;
		}
	}
	// Release shared memory
	if (addr_data > 0)
	{
		if (shmdt(addr_data) < 0)
		{
			printf("FAKAM: addr_data shmdt failed!!!\n");
			shmStatus = SHM_SHMDT_ERROR;
			return;
		}
	}
	// Ok!
	shmStatus = SHM_OK;
}



/////////////////////////////////////////////////////////
//
// BUFFER STUFF
//

//
// Erase shared image
void FakamImage::erase()
{
	memset(addr_data, 0, addr_header->dataSize);
}







