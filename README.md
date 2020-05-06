# Cylon
UPDATED 5/5/2020 6:11 pm
I went with Option 2. Reworked the showLEDs() function with a different strategy. Now this function looks at which direction the cylon is moving via the bool cylonMovingRight. Then there are two separate for loops depending on the direction. When moving right the cylon is filled from the right side first, which has the benefit of starting with the head or brightest pixel and then as the for loops moves through the medium and then dim pixels are added.

Video of cylon working in after the reworked showLEDs() update.
https://drive.google.com/file/d/1I9NZccexW2EgEiqjy2WH0xh5EaE1K6Uc/view?usp=sharing

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

