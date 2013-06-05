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
 $Id: MyCameraCentral.h,v 1.10 2008/04/26 19:59:44 hxr Exp $
 */

//
// ROGER
// Based on MyDummyCameraDriver.m
//

#import "SharedDriver.h"
#import "Resolvers.h"
#import "ControllerInterface.h"  // chip interface
#import "MyController.h"         // user interface
extern "C" {
#import "MiniGraphicsTools.h"
}

#import "FakamImageServer.h"



@implementation SharedDriver

+ (unsigned short) cameraUsbProductID { return 0; }
+ (unsigned short) cameraUsbVendorID { return 0; }
+ (NSString*) cameraName { return [MyCameraCentral localizedStringFor:@"SharedDriver"]; }
- (BOOL) realCamera { return NO; }
// ROGER
- (BOOL) fixedFrameRate { return fixedFrameRate; }

- (id) initWithCentral:(MyCameraCentral*)c
{
	// super
    self = [super initWithCentral:c];	//init superclass
    if (self == NULL)
		return NULL;
	
	// set resolution
    short rate;
    CameraResolution res;
    res = [self defaultResolutionAndRate:&rate];
    //[self setResolution:res fps:rate];	// must be grabbing
	resolution = res;
	fps = rate;
	// ROGER
	fixedFrameRate = YES;
	fpsCounter = [[FrameCounter alloc] init];

	// misc
	width = [self width];
	height = [self height];
	
	// Alloc shm
	shm = new FakamImageServer();
	if (shm == NULL)
		NSLog(@"SHARED MEM: shm NULL!!!");
	else if (shm->getStatus() != SHM_OK)
		NSLog(@"SHARED MEM: shm error [%s]",shm->getStatusDesc());

	// Start server
	shouldBeGrabbing = YES;
	
	// ok!
    return self;
}

- (void) dealloc
{
	// super
	[super dealloc];

	delete shm;
}

- (BOOL) supportsResolution:(CameraResolution)r fps:(short)fr
{
    if ((fr==3020)&&(r==ResolutionVGA))
		return YES;
    else
		return NO;
}

- (CameraResolution) defaultResolutionAndRate:(short*)dFps
{
    if (dFps)
		*dFps=30;
    return ResolutionVGA;
}

//
// Grabbing
// ROGER: based on makeOKImage
- (CameraError) decodingThread {
	NSLog(@"SHARED_MEM: decodingThread starting...");
	
    // Grab until told to stop
	while (shouldBeGrabbing) 
	{
		// wait for buffer
		if (nextImageBufferSet == NO)
			continue;
		// maccam stuff
		[imageBufferLock lock];
		lastImageBuffer=nextImageBuffer;
		lastImageBufferBPP=nextImageBufferBPP;
		lastImageBufferRowBytes=nextImageBufferRowBytes;
		nextImageBufferSet=NO;
		[imageBufferLock unlock];
		//NSLog(@"SHARED_MEM: draw last/nextImageBuffer [%d][%d] %c...",lastImageBuffer,nextImageBuffer,(lastImageBuffer==nextImageBuffer?'=':'!'));
		// Draw!
		[self sleepToMantainFramerate];
		[self draw];
	}
	
	/*
	 // Dummy driver image
	 [NSTimer scheduledTimerWithTimeInterval:(1.0f)/((float)fps)
	 target:self
	 selector:@selector(imageTime:)
	 userInfo:NULL
	 repeats:YES];
	 CFRunLoopRun();
	 */
	
	NSLog(@"SHARED_MEM: decodingThread ending...");
    return CameraErrorOK;
}
// ROGER
- (void) sleepToMantainFramerate {
	// No sleep!
	if ([self fixedFrameRate] == NO)
		return;
	// Sleep...
	static unsigned long lastTime = 0;
	unsigned long currentTime = 0;
	do {
		struct timeval now;
		gettimeofday( &now, NULL );
		currentTime = now.tv_usec/1000 + now.tv_sec*1000;
	} while ( (currentTime-lastTime) < (unsigned long)(1000/fps));
	lastTime = currentTime;
}

//
// Draw to maccam from read buffer
- (void) draw
{
	//NSLog(@"SHARED_MEM: Drawing... connected[%d] bufReady[%d]",connected,bufReady);

	// Get maccam buffer
	BOOL alpha = (lastImageBufferBPP==4);
	UInt8* buf = lastImageBuffer;
	
	//
	// SHM ERROR
	if (shm->getStatus() != SHM_OK)
	{
		short x, y;
		UInt8 r, g, b;
		for ( y = 0 ; y < height ; y++ )
		{
			for ( x = 0 ; x < width ; x++ )
			{
				r = (UInt8) (255.0 - (255.0 * ((float)y/(float)height)));
				g = 0;
				b = 0;
				// write to buffer
				if (alpha) buf++;
				*(buf++) = r;
				*(buf++) = g;
				*(buf++) = b;
			}
		}
		// off-line message
		MiniDrawString(lastImageBuffer,
					   lastImageBufferBPP,
					   lastImageBufferRowBytes,
					   (width/2)-90,
					   height/2,
					   shm->getStatusDesc());
	}
	//
	// Read SHM
	else
	{
		shm->read((unsigned char *)buf);

		//
		// DISCONNECTED?
		if (shm->connected() == false)
		{
			short x, y;
			UInt8 r, g, b;
			for ( y = 0 ; y < height ; y++ )
			{
				for ( x = 0 ; x < width ; x++ )
				{
					b = (UInt8) (255.0 - (255.0 * ((float)y/(float)height)));
					g = 0;
					r = 0;
					// write to buffer
					if (alpha) buf++;
					*(buf++) = r;
					*(buf++) = g;
					*(buf++) = b;
				}
			}
			// off-line message
			MiniDrawString(lastImageBuffer,
						   lastImageBufferBPP,
						   lastImageBufferRowBytes,
						   (width/2)-40,
						   height/2,
						   "DISCONNECTED");
		}
	}
	
	// FPS
    [fpsCounter addFrame];
    if ([fpsCounter update]) 
        [[central delegate] updateStatus:NULL fpsDisplay:fps fpsReceived:[fpsCounter getCumulativeFPS] ];
	// Draw FPS
	MiniDrawString(lastImageBuffer,
				   lastImageBufferBPP,
				   lastImageBufferRowBytes,
				   width-35,
				   height-20,
				   (char*)[[NSString stringWithFormat:@"%.1f",[fpsCounter getCumulativeFPS]] UTF8String]);

	// Move to maccam
	[self mergeImageReady];
}



@end


