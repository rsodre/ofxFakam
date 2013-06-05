/*
 macam - webcam app and QuickTime driver component
 Copyright (C) 2002 Matthias Krauss (macam@matthias-krauss.de)
 
 This program is free software; you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation; either version 2 of the License, or
 (at your option) any later version.
 
 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.
 
 You should have received a copy of the GNU General Public License
 along with this program; if not, write to the Free Software
 Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 $Id: Processing.h,v 1.3 2007/01/31 18:08:34 hxr Exp $
 */

//
// ROGER
// Based on MyDummyCameraDriver.m
//


#include "GlobalDefs.h"
#import "MyCameraDriver.h"
#import "MyCameraCentral.h"
#import "FrameCounter.h"

@interface Processing : MyCameraDriver {
	int width;
	int height;
	// Sockets
	NSSocketPort* socket;
	NSFileHandle* sockHandle;
	NSFileHandle* remoteHandle;
	NSDictionary *userInfo;
	// Buffer
	BOOL connected;
	BOOL bufReady;
	int bufSize;
	UInt8* bufProcessing;
	UInt8 red;
	// FPS (copy from GenericDriver)
    FrameCounter * displayFPS;
}

//Get info about the camera specifics.
+ (unsigned short) cameraUsbProductID;
+ (unsigned short) cameraUsbVendorID;
+ (NSString*) cameraName;
- (BOOL) realCamera;	//Returns if the camera is a real image grabber or a dummy

//Start/stop
- (id) initWithCentral:(MyCameraCentral*)c;	//same as above with CameraErrorOK
- (BOOL) supportsResolution:(CameraResolution)r fps:(short)fr;
- (CameraResolution) defaultResolutionAndRate:(short*)dFps;

//Grabbing
- (CameraError) decodingThread;				//We don't actually grab but draw images..

//ROGER
- (void) draw;
// network
- (void) startServer;
- (void) stopServer;
- (void) listen;
- (void) acceptConnection:(NSNotification *)notification;
- (void) disconnect;
- (void) readBuffer:(NSNotification *)notification;

@end
