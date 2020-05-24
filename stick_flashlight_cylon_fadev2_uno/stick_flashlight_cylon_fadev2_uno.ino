
/*===========================================================
 * UNO w/ WS2812 strip Cylon - table driven example.
 *   
 * Uses button to change LED color.
 * Uses Potentiometer to change brightness.
 */

#include <Adafruit_NeoPixel.h>

// Which pin to use for LED control
#define LED_PIN    3

//#define BUTTON_PIN 0      // Tiny
#define BUTTON_PIN 8    // Uno

#define WINDOWSIZE 96

// changing the number of colors requires a lot of other changes, ie: create new mid_colors throughout the programa
#define NUM_GRADIENT_COLORS 5  
#define GRADIENT_COLOR_SPACING 4

#define CYLONSIZE (NUM_GRADIENT_COLORS + GRADIENT_COLOR_SPACING * (NUM_GRADIENT_COLORS - 1))
#define SIDEBUFFERSIZE (CYLONSIZE-1)
#define FULLARRAYSIZE (WINDOWSIZE + 2 * SIDEBUFFERSIZE)

//#define POT_PIN    2   // Tiny
#define POT_PIN    A0    // Uno

typedef enum
{
  CYLON_BACKGROUND=0,
  CYLON_EYE
} cylon_state_type;

cylon_state_type pixelState[FULLARRAYSIZE];

#define CYLON_MIN_DELAY 0
#define CYLON_MAX_DELAY 50
int cylonDelay;

int cylonIndex = 0;
//int cylonColorMode = 1;

int colorGradientMode = 0;

// track direction of cylon right or left to allow for fading trail 
bool cylonMovingRight = true;

// track position of cylon head in relation to edge of LED strip so we can pause longer at edges and know when to turn around
bool cylonHeadAtEdge = false;

Adafruit_NeoPixel pixels = Adafruit_NeoPixel(WINDOWSIZE, LED_PIN, NEO_GRB+NEO_KHZ800);

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
#define COLOR_WHITE   0x101010

uint32_t bgrd_color = COLOR_BLACK;

typedef struct
{
  int red;
  int green;
  int blue;
} drgb_type;

drgb_type cylon_gradient[CYLONSIZE];

//initialize drgb_type structs of a variety of colors to use for gradients
drgb_type drgb_red={255, 0, 0};
drgb_type drgb_blue={0,0,255};
drgb_type drgb_green={0,255,0};
drgb_type drgb_pink={200,0, 170};
drgb_type drgb_black={0,0,0};

drgb_type drbg_skyBlue={101, 222, 241};
drgb_type drbg_safetyOrange={249, 105, 0};
drgb_type drgb_blueMunsell={6, 141, 157};
drgb_type drgb_liberty={6, 141, 157};

drgb_type drgb_magneta={211, 12, 123};
drgb_type drgb_mistyRose={255, 227, 220};
drgb_type drgb_arcticLime={210, 255, 40};
drgb_type drgb_sinopia={200, 76, 9};
drgb_type drgb_darkSienna={66, 2, 23};

drgb_type drgb_titaniumYellow={244, 228, 9};
drgb_type drgb_orangeYellow={238, 186, 11};
drgb_type drgb_ochre={195, 111, 9};
drgb_type drgb_rust={166, 60, 6};
drgb_type drgb_bloodRed={113, 0, 0};


typedef struct
{
  drgb_type start_color;
  drgb_type mid_color1;
  drgb_type mid_color2;
  drgb_type mid_color3;
  drgb_type end_color;
} gradient_palette_type;


gradient_palette_type gradient_palette[]=
{ //start_color,  mid_color1,   mid_color2,   mid_color3,     end_color
   {drgb_red,     drgb_pink,     drgb_green,   drgb_green,     drgb_green},  // palette 0
   {drgb_pink,    drgb_blue,   drgb_blue,      drgb_blue,      drgb_blue},  // palette 1
   {drgb_red,     drgb_blue,     drgb_blue,    drgb_blue,     drgb_red},  // palette 2
   {drgb_red,     drgb_red,     drgb_red,     drgb_red,     drgb_green},  // palette 3
   {drgb_bloodRed, drgb_rust,   drgb_ochre,   drgb_orangeYellow,  drgb_titaniumYellow},  // palette 4
   {drbg_safetyOrange,    drgb_blueMunsell,    drgb_liberty,    drbg_skyBlue,      drbg_skyBlue},   // palette 5
   {drgb_magneta,    drgb_mistyRose,    drgb_arcticLime,    drgb_sinopia,      drgb_darkSienna}   // palette 6
};

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
    Serial.println("STARTING CYLON NOW!");
    setupCylon();
    Serial.print("colorGradientMode: ");
    Serial.println(colorGradientMode);
    delay(1000);

}


/*================================================
 * setupCylon function called from setup() and perhaps other times
 * 
 * Fills the leds based on CYLONSIZE and WINDOWSIZE.
 *===============================================*/ 
void setupCylon(){
    int i;
    uint32_t led_color=bgrd_color;
    for (int i=0; i < FULLARRAYSIZE; i++)
    {
      if (i < SIDEBUFFERSIZE) // populate the area to the left of the visible window of LEDs with background color
      {
         pixelState[i] = CYLON_BACKGROUND;
      }
      else if (i >= SIDEBUFFERSIZE && i< SIDEBUFFERSIZE + CYLONSIZE ) // populate the left side of the visible window of LEDs with cylon color
      {
        pixelState[i] = CYLON_EYE;
      }
      else // populate the rest of the FULLARRAY with background color
      {
         pixelState[i] = CYLON_BACKGROUND; 
      }    
    }
    setupGradient(colorGradientMode);
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
   // fills the cylon based on value of cylonMovingRight
   // need to improve the math and clean up code that determines what portion of the
   // cylon is full colr, medium, or dim

   int cylonCounter = 0; // track how many pixels have been lit for fading

   if (cylonMovingRight)
   {
       for (int i=WINDOWSIZE-1; i >= 0; i--)
       {
          int j = i + SIDEBUFFERSIZE;
          if (pixelState[j] == CYLON_EYE)
          {
            //fill the cylon from the start_color moving towards end_color. cylonCounter tracks how many pixels of cylon have been filled so far.
            pixels.setPixelColor(i, cylon_gradient[cylonCounter].red, cylon_gradient[cylonCounter].green, cylon_gradient[cylonCounter].blue);
            cylonCounter++;     
          }
          else
            pixels.setPixelColor(i,bgrd_color); 
       }
   }
   else
   {
    for (int i=0; i< WINDOWSIZE; i++)
       {
          int j = i + SIDEBUFFERSIZE;
          if (pixelState[j] == CYLON_EYE)
          {
             pixels.setPixelColor(i, cylon_gradient[cylonCounter].red, cylon_gradient[cylonCounter].green, cylon_gradient[cylonCounter].blue);
             cylonCounter++;           
          }
          
          else
            pixels.setPixelColor(i,bgrd_color); 
       } 
   }
   pixels.show();    
}

void moveLEDs(){

  static uint32_t last_update_time_ms = 0;
  uint32_t        curr_time_ms;

  curr_time_ms = millis();


 /* If enough time hasn't passed before updating, just return.  */
  if (last_update_time_ms + cylonDelay > curr_time_ms)
  {
    return;
  }


  last_update_time_ms = curr_time_ms;
  
  if (cylonMovingRight == true) // If we cylon is moving right...
  {
    if (pixelState[FULLARRAYSIZE - 1] == 1) // check to see if far right spot is cylon head
      cylonHeadAtEdge = true;
    else
      cylonHeadAtEdge = false;
      
    if(cylonHeadAtEdge == true) // if at the edge switch direction to move left
    {
      cylonMovingRight = false;
    }
    else // move all pixels one spot to the right
    {
      for (int j = FULLARRAYSIZE - 1; j > 0; j--)
      {
        pixelState[j]=pixelState[j-1];
      }
      pixelState[0]=0; // fill the far left spot with background or empty
    } 
  }
  else // cylon is moving left
  {
     if (pixelState[0] == 1) // check to see if far left spot is now cylon head
      cylonHeadAtEdge = true;
     else 
      cylonHeadAtEdge = false;
      
    if(cylonHeadAtEdge == true)
    {
      cylonMovingRight = true;
    }
    else
    {
      for (int j = 0; j < FULLARRAYSIZE - 1; j++)
      {
        pixelState[j]=pixelState[j+1];
      }
      pixelState[FULLARRAYSIZE - 1]=0;
    }
  }
}

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
  int gradientSize = sizeof(gradient_palette) / sizeof(gradient_palette_type);
     //Serial.print("gradientSize: ");
     //Serial.print(gradientSize);
  
  if (buttonPressed())
  { 
     colorGradientMode++;
     
     if (colorGradientMode >= gradientSize)
       colorGradientMode = 0;
     setupGradient(colorGradientMode);
     
     Serial.print("colorGradientMode: ");
     Serial.println(colorGradientMode);
  }
  /* This is the older method for incrementing between the colorPalettes of bright, med, dim
  {
     cylonColorMode++;
     if (cylonColorMode > 9)
        cylonColorMode = 1;

    #if 1
    // Uno debug 
    Serial.print("cylonColorMode = ");
    Serial.println(cylonColorMode);
    #endif
  }
  */
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
 * setupGradient function takes the passed value of colorGradientMode
 * and sets the start and end color values for red, green, and blue to be used
 * by makeGradient to calculate all of the drgb color values for the whole LED array
 * THIS is where we can create NEW gradients to be used to color the cylon
 *===============================================*/ 
void setupGradient(int gradientMode){

  // define NUM_GRADIENT_COLORS is based on how many colors will be in gradient below
  drgb_type start_color, mid_color1, mid_color2, mid_color3, end_color;
  
  start_color = gradient_palette[gradientMode].start_color;
  mid_color1 = gradient_palette[gradientMode].mid_color1;
  mid_color2 = gradient_palette[gradientMode].mid_color2;
  mid_color3 = gradient_palette[gradientMode].mid_color3;
  end_color = gradient_palette[gradientMode].end_color;

 
  makeGradient(cylon_gradient, (2 + GRADIENT_COLOR_SPACING), start_color, mid_color1);  
  makeGradient(&(cylon_gradient[(1+GRADIENT_COLOR_SPACING)]), (2 + GRADIENT_COLOR_SPACING), mid_color1, mid_color2); 
  makeGradient(&(cylon_gradient[2 * (1+GRADIENT_COLOR_SPACING)]), (2 + GRADIENT_COLOR_SPACING), mid_color2, mid_color3); 
  makeGradient(&(cylon_gradient[3 * (1+GRADIENT_COLOR_SPACING)]), (2 + GRADIENT_COLOR_SPACING), mid_color3, end_color); 

}

/*================================================
 * makeGrandient function takes the number of LEDs for the gradient array and
 * calculated the step_size for each color. Then we use a for loop to add that
 * step_size to the start_color for each color.
 *===============================================*/ 
void makeGradient(drgb_type passsed_grad[], int divisions, drgb_type start_color, drgb_type end_color){
  int step_size_red = (end_color.red - start_color.red) / (divisions-1);
  int step_size_green = (end_color.green - start_color.green) / (divisions-1);
  int step_size_blue = (end_color.blue - start_color.blue) / (divisions-1); 

  // set start_color for each color and then increment through all but the final division 
  // based on the step_size for each color.
  for (int i = 0; i < divisions - 1; i++)
  {
    passsed_grad[i].red = start_color.red + i * step_size_red;
    passsed_grad[i].green = start_color.green + i * step_size_green;
    passsed_grad[i].blue = start_color.blue + i * step_size_blue;  
  }
  
  // last division is set to the end_color for each color.
  passsed_grad[divisions-1].red = end_color.red;
  passsed_grad[divisions-1].green = end_color.green;
  passsed_grad[divisions-1].blue = end_color.blue;
}

/*================================================
 * printGradient function uses Serial.print statements to display values of the red, green, blue 
 * for each item in the LED array
 *===============================================*/ 
void printGradient(int divisions){
  Serial.println("");
  Serial.print("colorGradientMode = ");
  Serial.println(colorGradientMode);
  Serial.println("***********************");
  for (int i = 0; i < divisions; i++)
  {
    if ((i%(1+GRADIENT_COLOR_SPACING) == 0 ))
      Serial.println("GRADIENT COLOR");
    Serial.print("cylon_gradient[");
    Serial.print(i);
    Serial.print("].red = ");
    Serial.print(cylon_gradient[i].red);

    Serial.print(" cylon_gradient[");
    Serial.print(i);
    Serial.print("].green = ");
    Serial.print(cylon_gradient[i].green);

    Serial.print(" cylon_gradient[");
    Serial.print(i);
    Serial.print("].blue = ");
    Serial.println(cylon_gradient[i].blue);
    if ((i%(1+GRADIENT_COLOR_SPACING) == 0 || (i%(1+GRADIENT_COLOR_SPACING) == 3)))
      Serial.println("");
  }
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
