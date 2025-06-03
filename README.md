<b><h2><center>Print Screen for ILI9341_t3 libraries and Touchscreen Displays</center></h1></b>

This library will let programmers add a print screen capability to their projects. There are 2 functions: 1) SaveBMP24 and 2) DrawBMP24 so you can save and draw images. The created imagtes are 24-bit BMP files
As this library requires and SD card there are 2 versions of the functions 1) for SD.h and 2) SdFat.h. The functions are simple inline and don't require any object creation. The goal was to make implementation simple such as


#include <ILI9341_t3_PrintScreen_SD.h>

or

#include <ILI9341_t3_PrintScreen_SdFat.h>


The functiion names in either .h are the same.

To save a screen shot, make a call like this

SaveBMP24(&display_object, chip select pin, "the_file.bmp");

To draw a BMP file to the screen, make a call like this

DrawBMP24(&display_object, chip select pin, "the_file.bmp");


You need not initialize and SD card or even include the SD librar, all that is done in inside the functions.

Example screenshots

![header image](https://raw.github.com/KrisKasprzak/ILI9341_t3_PrintScreen/master/0009.jpg)
![header image](https://raw.github.com/KrisKasprzak/ILI9341_t3_PrintScreen/master/0003d.jpg)

While this library was created for the ILI9341_t3 and intended for Teensy-based MCU's, the library *may* be adaptable to other display drivers provide the driver has a method getPixel(). At the time of posting, I don't know of any lib other that the ILI9341_t3.h that support the getPixel() method.

<br>
<br>

