# Cylon
UPDATED 5/14/2020 4:11 pm
Created method for dividing cylon colors into a gradient with multiple colors spread across the cylon eye. CYLONSIZE is now defined based on the NUM_GRADIENT_COLORS and GRADIENT_COLOR_SPACING. Based on how many colors and the number of pixels between the colors, the cylon is established. This will work nicely on larger LED strips, but might not work as well on small strips like the TinyStrip with the TinyAT85.
setupGradient() function now takes the colorGradientMode set by the number of buttonPresses and sets the various gradient colors based on what mode we are in.
NOTE: The number of colors used in setupGradient should match the NUM_GRADIENT_COLORS defined at the start. There will always be a start_color and end_color and x number of colors in between. Each of these colors is a drgb_type struct with int values for red, green, and blue. start_color.red = 255 for example sets the red value of the first color of the gradient. mid_color2.blue = 0 would set the blue to 0 on the second mid color of the gradient.
makeGradient() now has parameters that are passed including the color_gradient array, number of LEDs or division to cover, and two drgb_type colors for the start and end point of that gradient. 
makeGradient() can be called on subsets of the whole cylon using pointers. The cylon is divided into overlapping zones based on GRADIENT_COLOR_SPACING and NUM_GRADIENT_COLORS. This allows the gradient to smoothly fade across each gradient color that is set.

EXAMPLE:
//1st call to makeGradient() calculates colors for the first section of the cylon
makeGradient(color_gradient, (2 + GRADIENT_COLOR_SPACING), start_color, mid_color1);  
//2nd call to makeGradient() starts by overlapping with the end of the last section of the cylon
makeGradient(&(color_gradient[(1+GRADIENT_COLOR_SPACING)]), (2 + GRADIENT_COLOR_SPACING), mid_color1, mid_color2); 
//3rd call overlaps the previous section and completes this example with NUM_GRADIENT_COLOR = 4
makeGradient(&(color_gradient[2 * (1+GRADIENT_COLOR_SPACING)]), (2 + GRADIENT_COLOR_SPACING), mid_color2, end_color);

printGradient() is also called in setupGradient and prints out the values of each drgb_type color for debugging purposes.


UPDATED 5/10/2020 1:22 pm
Successfully created and merged new branch colorGradient which implements the grgb datatype.
Using this new method for filling the cylon with colors created by makeGradient() function.
When buttonPressed() occurs, the gradientColorMode increments and setupGradient is called to set up a new start and end value for red, green, and blue colors. 
makeGradient() calculates and fills the drgb array of CYLONSIZE length with the colors based on the start and end color values changed by setupGradient().
When showLEDs() is called a new method for filling in the cylon is used. We still use cylonCounter to see how many pixels of cylon have been filled, but instead of filling with a bright, medium, and dim pre-set hex color, we are instead using the drgb array to set each color from the gradient.

UPDATED 5/10/2020 12:02 pm
Create a drgb datatype that is a struct that will contain the values of red, green, and blue for each item in a pixel array.
Then create two functions - makeGradient() and printGradient() to set and display the values of red,green, and blue as we move through the array from the start value of each color to the end value of each color.

UPDATED 5/5/2020 6:11 pm
I went with Option 2. Reworked the showLEDs() function with a different strategy. Now this function looks at which direction the cylon is moving via the bool cylonMovingRight. Then there are two separate for loops depending on the direction. When moving right the cylon is filled from the right side first, which has the benefit of starting with the head or brightest pixel and then as the for loops moves through the medium and then dim pixels are added.

<a href="https://drive.google.com/file/d/1I9NZccexW2EgEiqjy2WH0xh5EaE1K6Uc/view?usp=sharing" target = "new">Video of cylon working</a> after the reworked showLEDs() update.



UPDATED 5/5/2020 around 4pm
New version of Cylon creates a virtual WINDOW where the pixels are visible from a larger array of values where the state of the array is stored.

WINDOWSIZE is the size of the LEDs where the cylon can be displayed
CYLONSIZE is the size of the cylon itself
SIDEBUFFERSIZE is the size of the area on either side of the window. This is where the cylon can go offscreen.
FULLARRAYSIZE is calculated by WINDOWSIZE + 2 * SIDEBUFFERSIZE because there is a side buffer on both ends

Goal is to allow the cylon to move almost completely off screen to the right or left leaving only one pixel still visible. Then turning around to go back the other way.

One problem that needs to be fixed - The current method for filling the visible window with the state of the cylon is to first look for whether the cylon is present in a certain part of the FULLARRAYSIZE and then based on direction the cylon is moving, turn on the bright, medium, and dim pixels in the cylon. Previously, when the cylon didn't ever leave the screen, when the cylon reached an edge, it flipped and the bright pixel jumped to the front of the cylon in the new direction. Now, when moving off the left side of the screen, the leftmost visible LED stays ON in the brightest color and then when it comes back onto the screen going left, the dim pixels appear first and only once the whole cylon is visible again, do all three brightnesses appear with the front being full bright. This has to do with moving from left to right as we fill the array and calculating how much of the cylon has displayed so far. This needs to be reworked now that the cylon is sometimes off screen....

Things to try: 
Option 1. Instead of storing the state of the cylon as CYLON_EYE or CYLON_BACKGROUND, I could add three new states, CYLON_EYE_BRIGHT, CYLON_EYE_MID, and CYLON_EYE_DIM. If we seed the state of the cylon to the FULLARRAYSIZE from the state, then determining what color to fill the LEDs with in showLEDs() becomes simplified.

Option 2. When showLEDs() is called and the cylon is moving right, we could traverse the visible part of the WINDOWSIZE array from the right to left instead of left to right. That way, the brightest part of the cylon (head) could come first and a simple counter can determine when to switch to medium and then dim.

Note: In showLEDs() below, i is used to move through the WINDOWSIZE array of visible LEDs while j is used to adjust to the FULLARRAYSIZE array that stores the state of all places in the array.

for (int i=0; i < WINDOWSIZE; i++)
    {
        // i is used to increment through the LEDs themselves
        // j is used to adjust the values of the pixelState of the WINDOWSIZE LEDS from the whole FULLARRAYSIZE
        int j = i + SIDEBUFFERSIZE;

