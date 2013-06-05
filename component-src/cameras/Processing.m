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

#import "Processing.h"
#import "Resolvers.h"
#import "MiniGraphicsTools.h"
#import "ControllerInterface.h"  // chip interface
#import "MyController.h"         // user interface

@implementation Processing

+ (unsigned short) cameraUsbProductID { return 0; }
+ (unsigned short) cameraUsbVendorID { return 0; }
+ (NSString*) cameraName { return [MyCameraCentral localizedStringFor:@"Processing"]; }
- (BOOL) realCamera { return NO; }

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
	
	// Alloc buffer
	width = [self width];
	height = [self height];
	bufSize = (width * height * 3);
	bufProcessing = malloc(bufSize);
	
	// Start server
	[self startServer];
	shouldBeGrabbing = true;
	
	// FPS
    displayFPS = [[FrameCounter alloc] init];

	// ok!
    return self;
}

- (void) dealloc
{
	if (socket)
		[self stopServer];
	free (bufProcessing);
	// super
	[super dealloc];
}

- (BOOL) supportsResolution:(CameraResolution)r fps:(short)fr
{
    if ((fr<20)&&(r>=ResolutionQSIF)) return YES;
    else return NO;
}

- (CameraResolution) defaultResolutionAndRate:(short*)dFps
{
    if (dFps) *dFps=30;
    return ResolutionVGA;
}

//
// Grabbing
// ROGER: based on makeOKImage
- (CameraError) decodingThread {
	NSLog(@"PROCESSING: decodingThread starting...");
	
    // Grab until told to stop
	while (shouldBeGrabbing) 
	{
		// maccam stuff
		[imageBufferLock lock];
		lastImageBuffer=nextImageBuffer;
		lastImageBufferBPP=nextImageBufferBPP;
		lastImageBufferRowBytes=nextImageBufferRowBytes;
		BOOL draw = nextImageBufferSet;
		nextImageBufferSet=NO;
		[imageBufferLock unlock];
		// Draw!
		if (draw)
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
	
	NSLog(@"PROCESSING: decodingThread ending...");
    return CameraErrorOK;
}

//
// Draw to maccam from read buffer
- (void) draw
{
	//NSLog(@"PROCESSING: Drawing... connected[%d] bufReady[%d]",connected,bufReady);

	// Get maccam buffer
	BOOL alpha = (lastImageBufferBPP==4);
	UInt8* buf = lastImageBuffer;
	
	//
	// Fill from Processing...
	if (connected == true && bufReady == true)
	{
		memcpy(buf, bufProcessing, bufSize);
		bufReady = false;
	}
	//
	// Fill dummy...
	else if (!connected)
	{
		short x, y;
		UInt8 r, g, b;
		for ( y = 0 ; y < height ; y++ )
		{
			for ( x = 0 ; x < width ; x++ )
			{
				r = 0;
				g = (UInt8) (255.0 * ((float)y/(float)height));
				b = (UInt8) (255.0 * ((float)x/(float)width));
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
					   (connected?"CONNECTED":"please start processing"));
	}
	
	// Move to maccam
	[self mergeImageReady];
}


#pragma mark NETWORK STUFF

#define PROCESSING_PORT			5522

/////////////////////////////////////////////////////////
//
// NETWORK STUFF
// http://macdevcenter.com/pub/a/mac/2006/11/14/how-to-write-a-cocoa-web-server.html
//
// Start the Server
- (void) startServer
{
	// create socket
	socket = [[NSSocketPort alloc] initWithTCPPort: PROCESSING_PORT];
	sockHandle = [[NSFileHandle alloc] initWithFileDescriptor: [socket socket]
											   closeOnDealloc: YES];
	[[NSNotificationCenter defaultCenter] addObserver: self
											 selector: @selector(acceptConnection:)
												 name: NSFileHandleConnectionAcceptedNotification
											   object: sockHandle];
	NSLog(@"PROCESSING: Server started!");
	// liten!
	[self listen];
}
//
// Stop the Server
- (void)stopServer
{
	// disconnect id connected
	if (remoteHandle != nil)
		[self disconnect];
	// release socket and handle
	[socket release];
	socket = nil;
	[sockHandle release];
	sockHandle = nil;
	// stop thread
    //[pool release];
	//[NSThread exit];
	NSLog(@"PROCESSING: Server stopped!");
}

///////////////////////////////////////////////////////////////
//
// Listen to Processing
//
- (void)listen
{
	connected = false;
	bufReady = false;
	// listen to new connections
	[sockHandle acceptConnectionInBackgroundAndNotify];
	[[central delegate] updateStatus:NULL fpsDisplay:0.0 fpsReceived:0.0];
	NSLog(@"PROCESSING: Wating for Processing...");
}

//
// Accept connection from Processing
- (void)acceptConnection:(NSNotification *)notification
{
	NSLog(@"PROCESSING: Connecting...");
	
	// Get remote
	userInfo = [notification userInfo];
	remoteHandle = [userInfo objectForKey:NSFileHandleNotificationFileHandleItem];
	
	// Accept remote data
	[[NSNotificationCenter defaultCenter] addObserver:self
											 selector:@selector(readBuffer:)
												 name:NSFileHandleReadCompletionNotification
											   object:remoteHandle];
	[remoteHandle readInBackgroundAndNotify];
	
	// continue listening to new connections
	//[sockHandle acceptConnectionInBackgroundAndNotify];
	
	// Connected!
	connected = true;
	NSLog(@"PROCESSING: Connected!");
}

//
// Disconnect from Processing
- (void)disconnect
{
	NSLog(@"PROCESSING: Disconnecting...");
	remoteHandle = nil;
	userInfo = nil;
	connected = false;
}

//
// Read buffer from processing
- (void) readBuffer:(NSNotification *)notification
{
	// make unavailabe
	bufReady = false;
	
	// Get data
	NSData *nsdata = [[notification userInfo] objectForKey:NSFileHandleNotificationDataItem];
	if ( [nsdata length] == 0 )
	{
		[self disconnect];
		[self listen];
		return;
	}
	//NSLog(@"PROCESSING: Read buffer... [%d] bytes",[nsdata length]);
	
	// Read single byte
	if ([nsdata length] == 1)
	{
		short x, y;
		UInt8 r, g, b;
		UInt8 *data = (UInt8*)([nsdata bytes]);
		UInt8 *buf = bufProcessing;
		for ( y = 0 ; y < height ; y++ )
		{
			for ( x = 0 ; x < width ; x++ )
			{
				r = *data;
				g = (UInt8) 0;
				b = (UInt8) 0;
				// write to buffer
				*(buf++) = r;
				*(buf++) = g;
				*(buf++) = b;
			}
		}
	}
	// Repeat pixels
	else
	{
		short x, y;
		UInt8 *data = (UInt8*)([nsdata bytes]);
		UInt8 *buf = bufProcessing;
		for ( y = 0 ; y < height ; y++ )
		{
			for ( x = 0 ; x < width ; x++ )
			{
				int i = (((y*width)+x)*3) % [nsdata length];
				// write rgb
				*(buf++) = *(data+i);
				*(buf++) = *(data+i+1);
				*(buf++) = *(data+i+2);
			}
		}
	}
	//bufProcessing
	
	// FPS
    [displayFPS addFrame];
    if ([displayFPS update]) 
        [[central delegate] updateStatus:NULL fpsDisplay:fps fpsReceived:[displayFPS getCumulativeFPS] ];

	// make availabe
	bufReady = true;
	
	// Wait next message
	[remoteHandle readInBackgroundAndNotify];
}


@end





