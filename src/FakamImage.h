/*
 *  FakamImage.h
 *  Fakam - Fake camera driver using shared memory
 *	Based on macam - http://webcam-osx.sourceforge.net/
 *
 *  Created by Roger on 25/08/10.
 *  Copyright 2010 Studio Avante. All rights reserved.
 *
 */
#pragma once

/////////////////////////////////////////////////////////
//
// SHARED MEM STUFF
//
// Resource: http://www.cs.cf.ac.uk/Dave/C/node27.html
//
// Increase Shared mem:
// http://www.spy-hill.net/help/apple/SharedMemory.html
// check:
//	sysctl -A | grep shm
// sudo vi /etc/sysctl.conf
//	kern.sysv.shmmax=33554432
//	kern.sysv.shmmin=1
//	kern.sysv.shmmni=256
//	kern.sysv.shmseg=64
//	kern.sysv.shmall=8192
// THEN REBOOT!
//
// view alloc:
//	ipcs -a
// free:
//	ipcrm -M 5522
//



#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/time.h>

// Shared memory key
#define SHM_KEY_HEADER				((key_t)5511)
#define SHM_KEY_DATA				((key_t)5522)

// Buffer size, in order
//	1	= FLAG
//	3	= depth (RGB)
//	640	= width
//	480	= height
#define SHM_IMAGE_BPP				3
#define SHM_IMAGE_WIDTH				640
#define SHM_IMAGE_HEIGHT			480
#define SHARED_BUF_SIZE				(SHM_IMAGE_BPP*SHM_IMAGE_WIDTH*SHM_IMAGE_HEIGHT)

// 1st byte Flag
#define SHARED_FLAG_EMPTY			0
#define SHARED_FLAG_FILLED			1
//#define SHARED_FLAG_DISCONNECTED	2

// Client timeout diconnect (millis)
#define SHARED_CLIENT_TIMEOUT		(3000)

// SHM Errors
enum FakamImageStatus {
	SHM_OK = 0,
	SHM_SHMGET_ERROR,
	SHM_SHMAT_ERROR,
	SHM_SHMDT_ERROR,
	SHM_MALLOC_ERROR,
	SHM_COUNT
};
extern char FakamImageStatusDesc[][128];

//
// HEADER
//
typedef struct _FakamImageHeader {
	short	bpp;			// RGB
	short	width;			// image width
	short	height;			// image height
	int		dataSize;		// data buffer size
	int		clientPid;		// connected client
	bool	filled;			// client pushed some data?
} FakamImageHeader;

class FakamImage {
public:
	
	FakamImage(bool server);
	~FakamImage();

	// Getters
	short		getWidth();
	short		getHeight();
	short		getStatus();
	char*		getStatusDesc();
	
	
protected:
	
	// shm
    int					shmid_header;
    int					shmid_data;
	FakamImageHeader	*addr_header;
	unsigned char		*addr_data;
	short				shmStatus;
	
	// Shared Mem stuff
	bool		allocSharedMemory(bool server);
	void		freeSharedMemory();
	// Buffer stuff
	void		erase();
	void		write(unsigned char *buffer);
	
};





