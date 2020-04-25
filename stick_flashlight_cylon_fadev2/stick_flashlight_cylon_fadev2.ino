/*===========================================================
 * Tiny Stick Flashlight
 *  
 * Test code for the tiny-stick board.  
 * Uses button to change LED color.
 * Uses Potentiometer to change brightness.
 */

#include <Adafruit_NeoPixel.h>

// Which pin to use for DualRingLED control
#define LED_PIN    3

//#define BUTTON_PIN 0      // Tiny
#define BUTTON_PIN 8    // Uno

#define NUMPIXELS 96
#define CYLONSIZEMIN 3
#define CYLONSIZEMAX 10

//#define POT_PIN    2   // Tiny
#define POT_PIN    A0    // Uno

typedef enum
{
  CYLON_BACKGROUND=0,
  CYLON_EYE
} cylon_state_type;

cylon_state_type pixelState[NUMPIXELS];

int cylonDelay = 70;
int cylonIndex = 0;
int cylonSize = CYLONSIZEMIN;

// track direction of cylon right or left to allow for fading trail 
bool cylonMovingRight = true;

Adafruit_NeoPixel pixels = Adafruit_NeoPixel(NUMPIXELS, LED_PIN, NEO_GRB+NEO_KHZ800);

#define COLOR_RED     0xFF0000
#define COLOR_RED_MED 0x3C0000
#define COLOR_RED_DIM 0x1F0000

#define COLOR_GREEN   0x00FF00
#define COLOR_BLUE    0x0000FF
#define COLOR_MAGENTA 0xFF00FF
#define COLOR_YELLOW  0xFFFF00
#define COLOR_CYAN    0x00FFFF
#define COLOR_BLACK   0
#define COLOR_WHITE   0xFFFFFF



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
    #if 1
    // Uno debug 
    Serial.begin(9600);
    pinMode(BUTTON_PIN,INPUT_PULLUP);
    #endif
    
    pixels.begin();
    setupCylon();
    delay(1000);
}

uint32_t colors[] = 
{ 
  COLOR_RED,
  COLOR_BLUE,
  COLOR_GREEN,
  COLOR_WHITE
};

void setupCylon(){
    int i;
    uint32_t led_color=COLOR_RED;
    for (int i=0; i < NUMPIXELS; i++)
    {
      if (i < cylonSize)
      {
         pixelState[i] = CYLON_EYE;
         pixels.setPixelColor(i,led_color);
      }
      else
      {
         pixelState[i] = CYLON_BACKGROUND; 
         pixels.setPixelColor(i,0);
      }    
      //pixels.show();
    }
    showLEDs();
}
void showLEDs(){
   int cylonCounter = 0; // track how many pixels have been lit for fading
   
   uint32_t cylon_color = COLOR_RED;
   uint32_t cylon_color_med = COLOR_RED_MED;   
   uint32_t cylon_color_dim = COLOR_RED_DIM;
   uint32_t bgrd_color = COLOR_BLACK;
   for (int i=0; i < NUMPIXELS; i++)
    {
      if (cylonMovingRight == true && pixelState[i] == CYLON_EYE )
      {
 
        if (cylonCounter < 1)
          pixels.setPixelColor(i,cylon_color_dim);
        else if (cylonCounter <= 2)
          pixels.setPixelColor(i,cylon_color_med);   
        else
          pixels.setPixelColor(i,cylon_color);  
        cylonCounter++;     
      }
      else if (cylonMovingRight == false && pixelState[i] == CYLON_EYE)
      {
        if (cylonSize - cylonCounter > 3)
          pixels.setPixelColor(i,cylon_color);
        else if (cylonSize - cylonCounter >= 2)
          pixels.setPixelColor(i,cylon_color_med);   
        else
          pixels.setPixelColor(i,cylon_color_dim);   
        cylonCounter++;      
      }
      else
      {
        pixels.setPixelColor(i,bgrd_color);
      }

    }
    pixels.show();
    if( pixelState[0] == CYLON_EYE || pixelState[NUMPIXELS-1] == CYLON_EYE)
      delay(cylonDelay*2.5);
    else if
      ( pixelState[1] == CYLON_EYE || pixelState[NUMPIXELS-2] == CYLON_EYE)
      delay(cylonDelay*2);
    else
      delay(cylonDelay);
}


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
    showLEDs();
    checkSpeed();
    checkButton();
  }
}

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
    showLEDs();
    checkSpeed();
    checkButton();
  }
}

void checkButton(){
    if (buttonPressed())
  {
    cylonSize++;
    if (cylonSize > CYLONSIZEMAX)
      cylonSize = CYLONSIZEMIN;
    Serial.print("cylonSize = ");
    Serial.println(cylonSize);
    setupCylon();
  }
}

void checkSpeed(){
  cylonDelay = map (analogRead(POT_PIN),0,1024,5,100);
}

void loop()
{
  shiftRIGHT();
  shiftLEFT();
}
