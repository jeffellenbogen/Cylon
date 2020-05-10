#define CYLONSIZE 6

int start_red = 255;
int start_green = 0;
int start_blue = 0;

int end_red = 127;
int end_green = 255;
int end_blue = 0;

typedef struct
{
  int red;
  int green;
  int blue;
} drgb_type;

drgb_type color_gradient[CYLONSIZE];

drgb_type start_color={start_red,start_green,start_blue};
drgb_type end_color={end_red,end_green,end_blue};

void setup() {
  Serial.begin(9600);
  Serial.print("start_color: start_red = ");
  Serial.print(start_color.red);  
  Serial.print(" start_green = ");
  Serial.print(start_color.green);  
  Serial.print(" start_blue = ");
  Serial.println(start_color.blue);  
  Serial.print("end_color: end_red = ");
  Serial.print(end_color.red);  
  Serial.print(" end_green = ");
  Serial.print(end_color.green);  
  Serial.print(" end_blue = ");
  Serial.println(end_color.blue); 
  Serial.println();

  makeGradient(CYLONSIZE);
  printGradient(CYLONSIZE);
}

/*
 * No loop() used
 */
void loop() {

}


/* makeGrandient function takes the number of LEDs for the gradient array and 
 * calculated the step_size for each color. Then we use a for loop to add that
 * step_size to the start_color for each color. 
 * 
 */
void makeGradient(int divisions){
  int step_size_red = (end_color.red - start_color.red) / (divisions-1);
  int step_size_green = (end_color.green - start_color.green) / (divisions-1);
  int step_size_blue = (end_color.blue - start_color.blue) / (divisions-1); 

  // set start_color for each color and then increment through all but the final division 
  // based on the step_size for each color.
  for (int i = 0; i < divisions - 1; i++)
  {
    color_gradient[i].red = start_color.red + i * step_size_red;
    color_gradient[i].green = start_color.green + i * step_size_green;
    color_gradient[i].blue = start_color.blue + i * step_size_blue;  
  }
  
  // last division is set to the end_color for each color.
  color_gradient[divisions-1].red = end_color.red;
  color_gradient[divisions-1].green = end_color.green;
  color_gradient[divisions-1].blue = end_color.blue;
}

/*
 * printGradient function traverses through the color_gradient array and Serial
 * prints each value of red, green, and blue for the item in drgb datatype array
 */

void printGradient(int divisions){
  for (int i = 0; i < divisions; i++)
  {
    Serial.print("color_gradient[");
    Serial.print(i);
    Serial.print("].red = ");
    Serial.print(color_gradient[i].red);

    Serial.print(" color_gradient[");
    Serial.print(i);
    Serial.print("].green = ");
    Serial.print(color_gradient[i].green);

    Serial.print(" color_gradient[");
    Serial.print(i);
    Serial.print("].blue = ");
    Serial.println(color_gradient[i].blue);
  }
}
