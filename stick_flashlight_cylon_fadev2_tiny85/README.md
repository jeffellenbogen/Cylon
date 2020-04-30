# Loop consolidation notes

## Overview:
The cyclon eye itself is made up of a "bright" leading pixel, followed by a mid pixel, followed by a dim pixel.  These pixels travel first right until the leading pixel hits the right boarder, then reverse positions such that the dim pixel is on the far right.  We then travel these pixels across the display until we hit the left boarder, then reverse positions and continue right.

## Data used:
NUMPIXELS tells us how many pixels long our display is.  Currently 8.
CYLONSIZE tells how many of those pixels make up the cyclon eye.

pixelState is an array that tells us whether each pixel is a part of the cylon or part of the background.
cylonIndex

## Algorithm:
```
Initialize lights (setup)

While we've got space to shift lights right:   (meaning the far right pixel is still a background pixel)
  Copy all pixels one slot to the right (for loop)
  Set the far left pixel to be a background pixel.
  show the LEDs (embedded delay here)
  update the speed based on the pot
  check for button presses.  If there's one, update the color palette.
Then, after we can't go right anymore...
While we've got space to shift lights left: (meaning the far left pixel is still a background pixel)
  Copy all pixels one slot to the left (for loop)
  set the far right pixel to be a background pixel
  show the LEDs (embedded delay here)
  update the speed based on the pot
  check for button presses.  If there's one, update the color palette.
  ```
  
The current implementation has the both whiles called from "loop".  This has two undesirable side effects:
* The activites necessary for looping are in two places.  Specifically, we've got a "show, update-speed, check buttons" duplicated.  Would be better to have one control loop.
* The light delays are embedded in the show funcitons.

Instead, consider the following flow:
```
Inside loop:
  Check for button presses to update color palette.
  Update speed based on potentiometer.  Sets delay variable.
  if it's time for a light update:
    * show the lights in their current positions
    * move the lights either one click left or right, depending on direction
```
The functions for button press and speed update stay the same.

If we had a single delay time, we could just use the existng cylonDelay varable to determine if it's tiime for an update.
However, since we want the cylon to "slow down" at the edges, we need an additional "current delay" variable that is set by the "showLEDs function".  This variable set replaces the existing delays.

Then, inside the main loop, we can check whether "current delay" time has elapsed since last update, and then show the LEDs.

Also note that the "whiles" inside of "shiftRight" and "shiftLeft" need to be reworked...instead, consider the following framework for moving the lights:
```
If we're currently going right:
  if we're on the right edge, reverse direction.
  otherwise, copy all lights one slot right.
else (we're currently going left):
  if we're on the left edge, reverse direction.
  otherwise, copy all lights one slot left.
```
  

 
