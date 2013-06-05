
ofxFakam
==================

ofxFakam is an Open Frameworks addon that streams screen data to a fake camera driver.
It relies on a QuickTime component that actually stream the screens to any program enabled to read from a camera source.

Works with OpenFrameworks, a cross platform open source toolkit for creative coding in C++.
[http://www.openframeworks.cc/](http://www.openframeworks.cc/)

Also woks with Cinder (sources pending)
[http://www.cinderlib.org/](http://www.cinderlib.org/)

The component is a stripped-down version of macam, and the component-src folder is basically a copy of the macam repository at the time I adapted it, and has not been updated until then.

Fakam works only on Mac OSX.

Shared Memory configuration
------------

Fakam uses UNIX shared memory. Maybe you'll need to increase the available ammount on your system. 

Check your current shared memory:
<pre>
sysctl -A | grep shm
</pre>

These params should be, AT LEAST:
<pre>
kern.sysv.shmmax: 33554432
kern.sysv.shmmin: 1
kern.sysv.shmmni: 256
kern.sysv.shmseg: 64
kern.sysv.shmall: 8192
</pre>

If not, edit /etc/sysctl.conf
<pre>
sudo vi /etc/sysctl.conf
</pre>

And add this:
<pre>
kern.sysv.shmmax=33554432
kern.sysv.shmmin=1
kern.sysv.shmmni=256
kern.sysv.shmseg=64
kern.sysv.shmall=8192
</pre>

Then REBOOT!

Component Installation
------------

The Fakam QuickTime component is based on macam, an open-source generic webcam driver for mac osx.
[http://webcam-osx.sourceforge.net/](http://webcam-osx.sourceforge.net/)

To install the component, copy the file coponent/fakam.component to /Library/QuickTime

If you're using macam, please delete the macam.component from the QuickTime folder for a while. Using both will cause conflicts. (See issues below)

ofxFakam addon Installation
------------

To use ofxFakam, first you need to download and install OpenFrameworks.
[http://www.openframeworks.cc/](http://www.openframeworks.cc/)

Then download ofxFakam sources from [http://github.com/rsodre/ofxFakam/zipball/master](http://github.com/rsodre/ofxFakam/zipball/master) or, alternatively, you can use git clone:
<pre>
git clone git://github.com/rsodre/ofxFakam.git
</pre>

The addon should sit in openFrameworks/addons/ofxFakam/

There's an example in ofxFakam/example/, just open and hit "Build and Run". You might want to chosoe "Release" instead of "Debug" for faster performance.

You can use oponent/FakamClient.app to test a Fakam stream. Just open it, press Play and then run your app.

How to add ofxFakam to your project
-------------------------------

Make sure your app is being rendered at resolution 640 x 480.
Fakam is currently fixed at this resolution, any other will surely break it or render crap.

So on main.cpp, there sould be something like this:
<pre>
ofSetupOpenGL(&window, 640, 480, OF_WINDOW);
</pre>

Add the ofxFakam/src folder to your XCode project.

On your main app header, include the following in the header includes:
<pre>
#include "ofxFakam.h"
</pre>

In your base App class, add the following:
<pre>
ofxFakam fakam;
</pre>

Render your OpenGL screen normally and then do the magic:
<pre>
fakam.copyScreen();
</pre>

That's it!!

ISSUES!!!
-----------------------------------------

Fakam is a stripped-down version of macam, and is currently conflicting with it.
If you're using macam, please delete the macam.component from the QuickTime components folder.
There must be a way to eliminate this conflict, but I did not discover it yet.

You should see a lot of warnings on the Console, please ignore them.

Being "always on", I don't really know if Fakam is eternally consuming CPU or looping endlessly trying to connect to a stream. Maybe the warnings come from there.




