#define CYLONSIZE 10


typedef struct
{
  int red;
  int green;
  int blue;
} drgb_type;

drgb_type color_gradient[CYLONSIZE];


void setup() {
  
  drgb_type start_color, mid_color1, mid_color2, end_color;

  start_color.red = 255;
  start_color.green = 0;
  start_color.blue = 0;

  end_color.red = 255;
  end_color.green = 0;
  end_color.blue=0;

  mid_color1.red = 0;
  mid_color1.green = 255;
  mid_color1.blue = 0;

  mid_color2.red = 0;
  mid_color2.green = 0;
  mid_color2.blue = 255;
  
   
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

  //makeGradient(color_gradient, CYLONSIZE, start_color, end_color);
  //printGradient(CYLONSIZE);

  //In this example we have 10 pixels transitioning between 4 drgb colors.
  //Each call to makeGradient is going to have overlapping end and start references to the whole array,
  //so that the drgb colors don't repeat between two pixels and rather a single inflection points of the colors as they transition
  //For any array we need to know how many reference colors and how many pixels in between
  //In this example we have 4 reference colors with 2 LEDs in between each. 
  // (# ref colors + size of spacing * (# ref colors - 1))
  // 4 + 2 * 3
  // 10 total pixels
  makeGradient(color_gradient, 4, start_color, mid_color1);  // pixels 0 - 3
  makeGradient(&(color_gradient[3]), 4, mid_color1, mid_color2); // pixels 3 - 6
  makeGradient(&(color_gradient[6]), 4, mid_color2, end_color); // pixels 6 - 9
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
