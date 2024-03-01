/*
The MIT License (MIT)

library writen by Kris Kasprzak

Permission is hereby granted, free of charge, to any person obtaining a copy of
this software and associated documentation files (the "Software"), to deal in
the Software without restriction, including without limitation the rights to
use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
the Software, and to permit persons to whom the Software is furnished to do so,
subject to the following conditions:
The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.
THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

On a personal note, if you develop an application or product using this library
and make millions of dollars, I'm happy for you!

*/



#ifndef ILI9341_T3_PRINTSCREEN_SDFAT_H
#define ILI9341_T3_PRINTSCREEN_SDFAT_H

#if ARDUINO >= 100
 #include "Arduino.h"
 #include "Print.h"
#else
 #include "WProgram.h"
#endif

#ifdef __cplusplus
#include "Arduino.h"
#endif

#include <ILI9341_t3.h>   

// if you are using SD librray, use the other .h library
#include <SdFat.h>


/*
code to capture a screen and save to a 24 bit BMP file

code was adapted from an unknow source that wrote a 16-bit BMP file saving utility
this code was converted to write a 24-bit file
this function relies on SdFat.fat

you will need to pass in:
1. your display object that uses ILI9431_t3 lib
2. chip selet pin
3. filename

*/


 bool inline SaveBMP24(ILI9341_t3 *d, int cs, const char *FileName) {


	SdFat sd;
	SdFile dataFile;

	int hh, ww;
	int i, j = 0;
	bool IsSD = false;
	uint8_t r, g, b;
	uint16_t rgb;
	hh = d->height();
	ww = d->width();

	// it may look odd not error trapping SD create
	// but if the user already has a created instance this call will fail
	// if no and SD does fail, we'll cathc it in the SD open statement
	sd.begin(cs);

	IsSD = dataFile.open(FileName, FILE_WRITE);

	if (!IsSD){
		return false;
	}

	unsigned char bmFlHdr[14] = {
	'B', 'M', 0, 0, 0, 0, 0, 0, 0, 0, 54, 0, 0, 0
	};

	// set color depth to 24 as we're storing 8 bits for r-g-b
	unsigned char bmInHdr[40] = {
	40, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 24, 0
	};

	unsigned long fileSize = 3ul * hh * ww + 54; 

	bmFlHdr[ 2] = (unsigned char)(fileSize      ); 
	bmFlHdr[ 3] = (unsigned char)(fileSize >>  8); 
	bmFlHdr[ 4] = (unsigned char)(fileSize >> 16);
	bmFlHdr[ 5] = (unsigned char)(fileSize >> 24);

	bmInHdr[ 4] = (unsigned char)(       ww      );
	bmInHdr[ 5] = (unsigned char)(       ww >>  8);
	bmInHdr[ 6] = (unsigned char)(       ww >> 16);
	bmInHdr[ 7] = (unsigned char)(       ww >> 24);
	bmInHdr[ 8] = (unsigned char)(       hh      );
	bmInHdr[ 9] = (unsigned char)(       hh >>  8);
	bmInHdr[10] = (unsigned char)(       hh >> 16);
	bmInHdr[11] = (unsigned char)(       hh >> 24);

	dataFile.write(bmFlHdr, sizeof(bmFlHdr));
	dataFile.write(bmInHdr, sizeof(bmInHdr));

	for (i = hh-1; i >= 0; i--) {
		for (j = 0; j < ww; j++) {

			// if you are attempting to convert this library to use another display library, this
			// is where you may run into issues
			// the libries must have a readPixel function
			rgb = d->readPixel(j, i);

			// convert the 16 bit color to full 24
			// that way we have a legit bmp that can be read into the
			// bmp reader below
			d->color565toRGB(rgb, r, g, b);

			// write the data in BMP reverse order
			dataFile.write(b);
			dataFile.write(g);
			dataFile.write(r);
		}
	}

	// file written
	dataFile.close();
	delay(10);
	return true;

}



/*

code adapted from Teensy forum user: socalpinplayer
code has been combined in to a single inline functiion call
this function relies on SD.fat but can be easily converted to SdFat

you will need to pass in:
1. your display object that uses ILI9431_t3 lib
2. chip selet pin
3. filename
4. and optional x and  for image location

*/



bool inline DrawBMP24(ILI9341_t3 *d, int cs, const char *FileName, uint8_t x = 0, uint16_t y = 0) {
	SdFat sd;
	SdFile   bmpFile;
	int      bmpWidth, bmpHeight;			// W+H in pixels
	int      w, h, row, col;
	uint8_t  bmpDepth;					// Bit depth (currently must be 24)
	uint32_t bmpImageoffset;				// Start of image data in file
	uint32_t rowSize;						// Not always = bmpWidth; may have padding
	uint8_t  sdbuffer[3 * 8];				// pixel buffer (R+G+B per pixel)
	uint8_t  buffidx = sizeof(sdbuffer);	// Current position in sdbuffer
	boolean  flip    = true;
	bool IsSD = false;
	// BMP is stored bottom-to-top
	uint8_t  r, g, b;						// holders for red green blue
	uint32_t pos = 0;						// file position
	uint16_t awColors[320];				// hold colors for one row at a time...
	uint16_t Read16;						// file read placeholder
	uint32_t Read32;						// file read placeholder

	// let's not test begin, reason is if you create a file SD exists and will fail here
	// just use any existing object
	// we will trap file open next
	sd.begin(cs);
  
	IsSD = bmpFile.open(FileName, FILE_READ);

	if (!IsSD){
		//Serial.print("SD.open: ");
		//Serial.println(bmpFile);
		return false;
	}

	if ((x >= d->width()) || (y >= d->height())) {
	return false;
	}

  // Parse BMP header

  ((uint8_t *)&Read16)[0] = bmpFile.read(); 
  ((uint8_t *)&Read16)[1] = bmpFile.read(); 

  if (Read16 == 0x4D42) { 
	// ignore read
	((uint8_t *)&Read32)[0] = bmpFile.read(); 
	((uint8_t *)&Read32)[1] = bmpFile.read();
	((uint8_t *)&Read32)[2] = bmpFile.read();
	((uint8_t *)&Read32)[3] = bmpFile.read(); 
	// ignore read
	((uint8_t *)&Read32)[0] = bmpFile.read(); 
	((uint8_t *)&Read32)[1] = bmpFile.read();
	((uint8_t *)&Read32)[2] = bmpFile.read();
	((uint8_t *)&Read32)[3] = bmpFile.read(); 

	// read offset
	((uint8_t *)&Read32)[0] = bmpFile.read(); 
	((uint8_t *)&Read32)[1] = bmpFile.read();
	((uint8_t *)&Read32)[2] = bmpFile.read();
	((uint8_t *)&Read32)[3] = bmpFile.read(); 
	bmpImageoffset = Read32; 

	// ignore read
    	((uint8_t *)&Read32)[0] = bmpFile.read(); 
	((uint8_t *)&Read32)[1] = bmpFile.read();
	((uint8_t *)&Read32)[2] = bmpFile.read();
	((uint8_t *)&Read32)[3] = bmpFile.read(); 

	// read width
	((uint8_t *)&Read32)[0] = bmpFile.read(); 
	((uint8_t *)&Read32)[1] = bmpFile.read();
	((uint8_t *)&Read32)[2] = bmpFile.read();
	((uint8_t *)&Read32)[3] = bmpFile.read(); 
	bmpWidth  = Read32;

	// read height
	((uint8_t *)&Read32)[0] = bmpFile.read(); 
	((uint8_t *)&Read32)[1] = bmpFile.read();
	((uint8_t *)&Read32)[2] = bmpFile.read();
	((uint8_t *)&Read32)[3] = bmpFile.read(); 
	bmpHeight = Read32;

	// get planes
	((uint8_t *)&Read16)[0] = bmpFile.read(); 
	((uint8_t *)&Read16)[1] = bmpFile.read();

    if (Read16 == 1) { // # planes -- must be '1'

		// read depth
		((uint8_t *)&Read16)[0] = bmpFile.read(); 
		((uint8_t *)&Read16)[1] = bmpFile.read();
		bmpDepth = Read16; // bits per pixel

		// read compression
		((uint8_t *)&Read32)[0] = bmpFile.read(); 
		((uint8_t *)&Read32)[1] = bmpFile.read();
		((uint8_t *)&Read32)[2] = bmpFile.read();
		((uint8_t *)&Read32)[3] = bmpFile.read(); 


      if ((bmpDepth == 24) && (Read32 == 0)) { // 0 = uncompressed

	// if you got this far, legit image file
        rowSize = (bmpWidth * 3 + 3) & ~3;

        if (bmpHeight < 0) {
          bmpHeight = -bmpHeight;
          flip      = false;
        }

        w = bmpWidth;
        h = bmpHeight;
        if ((x + w - 1) >= d->width())  w = d->width()  - x;
        if ((y + h - 1) >= d->height()) h = d->height() - y;

        for (row = 0; row < h; row++) { // For each scanline...
          if (flip) // Bitmap is stored bottom-to-top order (normal BMP)
            pos = bmpImageoffset + (bmpHeight - 1 - row) * rowSize;
          else     // Bitmap is stored top-to-bottom
            pos = bmpImageoffset + row * rowSize;
          if (bmpFile.position() != pos) { // Need seek?
            bmpFile.seek(pos);
            buffidx = sizeof(sdbuffer); // Force buffer reload
          }

          for (col = 0; col < w; col++) { // For each pixel...
            // Time to read more pixel data?
            if (buffidx >= sizeof(sdbuffer)) { // Indeed
              bmpFile.read(sdbuffer, sizeof(sdbuffer));
              buffidx = 0; // Set index to beginning
            }
           
            b = sdbuffer[buffidx++];
            g = sdbuffer[buffidx++];
            r = sdbuffer[buffidx++];
            awColors[col] = d->color565(r, g, b);
          } 

          d->writeRect(0, row, w, 1, awColors);

        }
      }
	  else {
		 return false;
	  }
    }
	else {
		 return false;
	}
  }

  bmpFile.close();

  return true;
  
}


#endif
