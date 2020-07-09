<b><h2><center>Print Screen for ILI9341_t3 libraries and Touchscreen Displays</center></h1></b>

This library will let programmers add a print screen capability to their projects. There are 2 functions: 1) SaveBMP25 and 2) DrawBMP24 so you can save and draw images. The created imagtes are 24-bit BMP files
As this library requires and SD card there are 2 versions of the functions 1) for SD.h and 2) SdFat.h. The functions are simple inline and don't require any object creation. The goal was to make implementation simple such as

SaveBMP24(&display_object, chip select pin, "the_file.bmp");

and 

DrawBMP24(&display_object, chip select pin, "the_file.bmp");

You need not initialize and SD card or even include the SD librar, all that is done in inside the functions.

While this library was created for the ILI9341_t3 and intended for Teensy-based MCU's, the library *may* be adaptable to other display drivers provide the driver has a method getPixel() . Note the driver must use the Adafruit_GFX libraries. To allow other display drivers, modify the lines in the .h and .cpp files to allow a different driver to be passed in. For example change call such as SliderH(ILI9341_t3 *disp,... to SliderH(Adafruit_RA8875 *disp,...
<br>
<br>

