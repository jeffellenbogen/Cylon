
/*===========================================================
 * Tiny Stick Cylon - table driven example.
 *   
 * Uses button to change LED color.
 * Uses Potentiometer to change brightness.
 */

#include <Adafruit_NeoPixel.h>

// Which pin to use for LED control
#define LED_PIN    3

#define BUTTON_PIN 0      // Tiny
//#define BUTTON_PIN 8    // Uno

#define NUMPIXELS 8
#define CYLONSIZE 3

#define POT_PIN    2   // Tiny
//#define POT_PIN    A0    // Uno

typedef enum
{
  CYLON_BACKGROUND=0,
  CYLON_EYE
} cylon_state_type;

cylon_state_type pixelState[NUMPIXELS];

#define CYLON_MIN_DELAY 10
#define CYLON_MAX_DELAY 1000
int cylonDelay;

int cylonIndex = 0;
int cylonColorMode = 1;

// track direction of cylon right or left to allow for fading trail 
bool cylonMovingRight = true;

// track position of cylon head in relation to edge of LED strip so we can pause longer at edges and know when to turn around
bool cylonHeadAtEdge = false;

Adafruit_NeoPixel pixels = Adafruit_NeoPixel(NUMPIXELS, LED_PIN, NEO_GRB+NEO_KHZ800);

#define COLOR_RED     0xFF0000
#define COLOR_RED_MED 0x3C0000
#define COLOR_RED_DIM 0x100000

#define COLOR_GREEN   0x00FF00
#define COLOR_GREEN_MED 0x003C00
#define COLOR_GREEN_DIM   0x001000

#define COLOR_BLUE    0x0000FF
#define COLOR_BLUE_MED   0x00003C
#define COLOR_BLUE_DIM  0x000010

#define COLOR_MAGENTA 0xFF00FF
#define COLOR_MAGENTA_MED 0x3C003C
#define COLOR_MAGENTA_DIM 0x100010

#define COLOR_YELLOW  0xFFFF00
#define COLOR_YELLOW_MED  0x3C3C00
#define COLOR_YELLOW_DIM  0x101000

#define COLOR_CYAN    0x00FFFF
#define COLOR_CYAN_MED    0x003C3C
#define COLOR_CYAN_DIM    0x001010

#define COLOR_BLACK   0
#define COLOR_WHITE   0xFFFFFF

uint32_t bgrd_color = COLOR_BLACK;

typedef struct
{
  uint32_t bright;
  uint32_t med;
  uint32_t dim;
} cylon_palette_type;

cylon_palette_type cylon_palette[]=
{
  // bright        med                dim
  {COLOR_RED,     COLOR_RED_MED,     COLOR_RED_DIM},      // mode 0...currently unused as logic is "1" based
  {COLOR_RED,     COLOR_RED_MED,     COLOR_RED_DIM},      // mode 1
  {COLOR_GREEN,   COLOR_GREEN_MED,   COLOR_GREEN_DIM},    // mode 2
  {COLOR_BLUE,    COLOR_BLUE_MED,    COLOR_BLUE_DIM},     // mode 3
  {COLOR_MAGENTA, COLOR_MAGENTA_MED, COLOR_MAGENTA_DIM},  // mode 4
  {COLOR_YELLOW,  COLOR_YELLOW_MED,  COLOR_YELLOW_DIM},   // mode 5
  {COLOR_CYAN,    COLOR_CYAN_MED,    COLOR_CYAN_DIM},     // mode 6
  {COLOR_RED,     COLOR_MAGENTA_MED, COLOR_CYAN_DIM},     // mode 7
  {COLOR_GREEN,   COLOR_YELLOW_MED,  COLOR_RED_DIM},      // mode 8
  {COLOR_BLUE,   COLOR_CYAN_MED,  COLOR_GREEN_DIM}       // mode 9
};

/*================================================================================
 * fillAll
 */
void fillAll( uint32_t color )
{
  int i;

  for (i=0; i<NUMPIXELS; i++)
  {
    pixels.setPixelColor(i, color);
  }
}

/*================================================
 * Debounce function.
 * 
 * Only count a button press on release, and only if it's been down for a sufficient 
 * amount of time.
 *===============================================*/ 

#define DEBOUNCE_TIME_MS 50
bool buttonPressed( void )
{
  static int last_state=HIGH;
  int current_state;
  static unsigned long down_start_time=0;
  unsigned long current_time;

  current_state = digitalRead(BUTTON_PIN);

  /* Look for high-to-low transistions */
  if (last_state == HIGH)
  {
    if (current_state == LOW)
    {
      last_state = current_state;
      down_start_time = millis();
    }

    return(false);
  }
  
  else
  {
    /* look for the release with "enough time" to count a button press. */
    if (current_state == HIGH)
    {
      /* button went from low to high.  Was it down long enough? */
      current_time = millis();
      if (current_time - down_start_time > DEBOUNCE_TIME_MS)
      {
        last_state = current_state;
        return(true);
      }
      else
      {
        /* went up too quick...this is a bounce.  */
        last_state = current_state;
        return(false);
      }
    }
    else
    {
      /* We're still "low".  Waiting for release.  */
      return(false);
    }
  }  
}

void setup()
{
    #if 0
    // Uno debug 
    Serial.begin(9600);
    pinMode(BUTTON_PIN,INPUT_PULLUP);
    #endif
    
    pixels.begin();
    setupCylon();
    delay(1000);
}


/*================================================
 * setupCylon function called from setup() and perhaps other times
 * 
 * Fills the leds based on CYLONSIZE and NUMPIXELS.
 *===============================================*/ 
void setupCylon(){
    int i;
    uint32_t led_color=COLOR_RED;
    for (int i=0; i < NUMPIXELS; i++)
    {
      if (i < CYLONSIZE)
      {
         pixelState[i] = CYLON_EYE;
      }
      else
      {
         pixelState[i] = CYLON_BACKGROUND; 
      }    
    }
    showLEDs();
}

/*================================================
 * showLEDs() function 
 * 
 * Sets the color of cylon based on the cylonColorMode.
 * Uses the bool cylonMovingRight to determine which side of cylon is bright and which side is faded.
 * Slows the cylon at the edges of the array of leds.
 *===============================================*/ 
void showLEDs(){
  cylon_palette_type palette;
  
  uint32_t cylon_color;
  uint32_t cylon_color_med;
  uint32_t cylon_color_dim;

  /* set our current cylon "palette" */
  palette = cylon_palette[cylonColorMode];
  cylon_color = palette.bright;
  cylon_color_med = palette.med;
  cylon_color_dim = palette.dim;
  
   // fills the cylon based on value of cylonMovingRight
   // need to improve the math and clean up code that determines what portion of the
   // cylon is full colr, medium, or dim

   int cylonCounter = 0; // track how many pixels have been lit for fading
   
   for (int i=0; i < NUMPIXELS; i++)
    {
        /*cylons will have a dim tail made of a variable length beyond the first two leds
        second to front led will be medium and head will be full color
        */
      if (cylonMovingRight == true && pixelState[i] == CYLON_EYE )
      {
        cylonCounter++;  
        /*When filling the cylon moving right, we start with the dim leds
        (CYLONSIZE - cylonCounter) tracks when we have two leds left to fill with medium and full
        */
        if (CYLONSIZE - cylonCounter >= 2)
          pixels.setPixelColor(i,cylon_color_dim);
        else if (CYLONSIZE - cylonCounter == 1)
          pixels.setPixelColor(i,cylon_color_med);   
        else //if (CYLONSIZE - cylonCounter == 0)
          pixels.setPixelColor(i,cylon_color);  
           
      }
      else if (cylonMovingRight == false && pixelState[i] == CYLON_EYE)
      {
        cylonCounter++;
        /*When filling the cylon moving left, we start with the bright led at the head
        This is much easier. Just start with the head at full brightness and the second led
        will be medium. All others are dim, so we can just used the cylonCounter
        */
        if (cylonCounter == 1)
          pixels.setPixelColor(i,cylon_color);
        else if (cylonCounter == 2)
          pixels.setPixelColor(i,cylon_color_med);   
        else
          pixels.setPixelColor(i,cylon_color_dim);   
      }
      else
      {
        pixels.setPixelColor(i,bgrd_color);
      }
    }
    pixels.show();    
}

void moveLEDs(){

  static uint32_t last_update_time_ms = 0;
  uint32_t        curr_time_ms;

  curr_time_ms = millis();

 
  /* cylon IS at an Edge - triple the time before updating */   
  if ((cylonHeadAtEdge == true) && (last_update_time_ms + 5 * cylonDelay > curr_time_ms))
  {
    return;
  }

 /* cylon is NOT at an Edge - and enough time hasn't passed before updating, just return.  */
  else if ((cylonHeadAtEdge == false) && (last_update_time_ms + cylonDelay > curr_time_ms))
  {
    return;
  }


  last_update_time_ms = curr_time_ms;
  
  if (cylonMovingRight == true)
  {
    if (pixelState[NUMPIXELS - 1] == 1) // check to see if far right spot is  cylon head
      cylonHeadAtEdge = true;
      
    if(cylonHeadAtEdge == true) // if at the edge switch direction to move left
    {
      cylonMovingRight = false;
      cylonHeadAtEdge = false;
    }
    else // move all pixels one spot to the right
    {
      for (int i = NUMPIXELS - 1; i > 0; i--)
      {
        pixelState[i]=pixelState[i-1];
      }
      pixelState[0]=0; // fill the far left spot with background or empty
    }
   
  }
  else // cylon is moving left
  {
     if (pixelState[0] == 1) // check to see if far left spot is now cylon head
      cylonHeadAtEdge = true;

    if(cylonHeadAtEdge == true)
    {
      cylonMovingRight = true;
      cylonHeadAtEdge = false;
    }
    else
    {
      for (int i = 0; i < NUMPIXELS - 1; i++)
      {
        pixelState[i]=pixelState[i+1];
      }
      pixelState[NUMPIXELS - 1]=0;
    }
  }
}

/*================================================
 * shiftRIGHT function moves the on/off status of the array of leds to the right,
 * until the far right is on rather than off.
 * 
 * After each shift, showLEDs is called, then we check the pot for speed (delay) changes,
 * and checkButton looks but buttonPressed() to determine cylon color changes.
 *===============================================*/ 
 
/*
void shiftRIGHT(){
  cylonMovingRight = true;
 // Serial.println("RIGHT");
  while (pixelState[NUMPIXELS - 1] == 0)
  {
    for (int i = NUMPIXELS - 1; i > 0; i--)
    {
      pixelState[i]=pixelState[i-1];
    }
    pixelState[0]=0;
  }
}
*/


/*================================================
 * shiftLEFT function does the same as shiftRIGHT, but moves array to the left instead of right
 *===============================================*/ 
 
/*
void shiftLEFT(){
 // Serial.println("LEFT");
  cylonMovingRight = false;
  while (pixelState[0] == 0)
  {
    for (int i = 0; i < NUMPIXELS - 1; i++)
    {
      pixelState[i]=pixelState[i+1];
    }
    pixelState[NUMPIXELS - 1]=0;
  }
}
*/

/*================================================
 * checkButton function calls buttonPressed() function which uses debouncing to determine if the
 * button was pressed.
 * 
 * Also increments the colors if the button is pressed.
 * 
 * Plan to eventually use ellapsedMillis instead of delays in the loop() and other functions.
 * Would like to implement a long press (2 seconds or more) that puts the array into another mode or
 * changes another value besides the color of the array. Perhaps this would make the CYLONSIZE bigger.
 * 
 *===============================================*/ 
void checkButton(){
  if (buttonPressed())
  {
     cylonColorMode++;
     if (cylonColorMode > 9)
        cylonColorMode = 1;
  }
}

/*================================================
 * checkSpeed() function reads the pot to determine the delay when shifting the cylon back and forth
 * Eventually may switch from delays to EllapsedMillis to remove blocking calls.
 *===============================================*/ 
void checkSpeed(){
  int pot_val;
  pot_val = analogRead(POT_PIN);
  cylonDelay = map (pot_val,0,1024,CYLON_MAX_DELAY,CYLON_MIN_DELAY); //pot is backwards, so swap MAX and MIN to have faster delay when turned to the right
  cylonDelay = constrain(cylonDelay,CYLON_MIN_DELAY,CYLON_MAX_DELAY);
}

/*================================================
 * loop function repeats over and over after setup()
 * shiftRIGHT() and shiftLEFT() alternate the cylon back and forth
 *===============================================*/ 
void loop()
{ 
  moveLEDs();
  showLEDs();
  checkSpeed();
  checkButton();
}
