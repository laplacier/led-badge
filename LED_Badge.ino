#include <Adafruit_NeoPixel.h>

#define LED 1
#define COLOR 0
#define PULSE 2

uint32_t setColor(uint8_t c);
void changeMode(uint8_t c, uint8_t p);
void colorWipe(uint32_t c, uint8_t wait);
void colorPulse(uint32_t c, uint8_t wait);
void rainbow(uint8_t wait);
void theaterChase(uint32_t c, uint8_t wait);
uint32_t Wheel(byte WheelPos);

// Parameter 1 = number of pixels in strip
// Parameter 2 = ATTINY85 pin number (hardwired to 1)
// Parameter 3 = pixel type flags, add together as needed:
Adafruit_NeoPixel strip = Adafruit_NeoPixel(5, LED, NEO_GRB + NEO_KHZ800);

bool flag_color = 0;
uint8_t c_count = 1;
bool flag_pulse = 0;
uint8_t p_count = 1;
uint32_t timerDeb = millis();

void setup() {
  pinMode(COLOR, INPUT_PULLUP);
  pinMode(PULSE, INPUT_PULLUP);
  strip.begin();

  //If either button is pressed while turning on then set brightness max
  if(digitalRead(COLOR) && digitalRead(PULSE)){
    strip.setBrightness(64);
  }
  else{
    strip.setBrightness(255);
    flag_color = 1;
    flag_pulse = 1;
  }
  strip.show(); // Initialize all pixels to 'off'
}

void loop() {

  //Color button pressed
  if(!digitalRead(COLOR) && flag_color && millis() - timerDeb > 200){
    timerDeb = millis();
    flag_color = 0;
    c_count++;
    if(c_count > 10){
      c_count = 1; 
    }
  }

  //Color button released
  if(digitalRead(COLOR) && !flag_color){
    flag_color = 1;
  }

  //Pulse button pressed
  if(!digitalRead(PULSE) && flag_pulse && millis() - timerDeb > 200){
    timerDeb = millis();
    flag_pulse = 0;
    p_count++;
    if(p_count > 5){
      p_count = 1; 
    }
  }

  //Pulse button released
  if(digitalRead(PULSE) && !flag_pulse){
    flag_pulse = 1;
  }

  //Execute color and animation
  changeMode(c_count, p_count);
}

//Return a 32 bit color value
uint32_t setColor(uint8_t c){
  switch (c) {
      case 1:
        //White
        return strip.Color(255, 255, 255);
        break;
      case 2:
        //Red
        return strip.Color(255, 0, 0);
        break;
      case 3:
        //Magenta
        return strip.Color(255, 0, 30);
        break;
      case 4:
        //Purple
        return strip.Color(127, 0, 255);
        break;
      case 5:
        //Blue
        return strip.Color(0, 0, 255);
        break;
      case 6:
        //Teal
        return strip.Color(0, 127, 30);
        break;
      case 7:
        //Turquoise
        return strip.Color(0, 255, 255);
        break;
      case 8:
        //Green
        return strip.Color(0, 255, 0);
        break;
      case 9:
        //Yellow
        return strip.Color(255, 255, 0);
        break;
      case 10:
        //Orange
        return strip.Color(255, 60, 0);
        break;
    }
}

//Set animation
void changeMode(uint8_t c, uint8_t p){
  switch (p) {
      case 1:
        //Static
        colorWipe(setColor(c), 50);
        break;
      case 2:
        //Pulse
        colorPulse(setColor(c), 10);
        break;
      case 3:
        //Pulse fast
        colorPulse(setColor(c), 2);
        break;
      case 4:
        //Theater chase
      
        theaterChase(setColor(c), 80);
        break;
      case 5:
        //Rainbow (ignores color)
        rainbow(50);
        break;
  }
}

//Static color, with slight starting animation
void colorWipe(uint32_t c, uint8_t wait) {
  uint32_t timerStart = millis();
  uint16_t i = 0;
  
  //While no buttons are pressed
  while( !(!digitalRead(COLOR) && flag_color) && !(!digitalRead(PULSE) && flag_pulse) ){
    //Color button released
    if(digitalRead(COLOR) && !flag_color){
      flag_color = 1;
    }

    //Pulse button released
    if(digitalRead(PULSE) && !flag_pulse){
      flag_pulse = 1;
    }
    
    if(millis() - timerStart > wait){
      strip.setPixelColor(i, c);
      strip.show();

      //Reset timer
      timerStart = millis();

      i++;

      //Reset i if reached end of strip
      if(i >= 5){
        i = 0;
      }
    }
  }
}

//Pulse transition from color, off, color, etc.
void colorPulse(uint32_t c, uint8_t wait) {
  uint16_t i = 0;
  uint64_t timerStart = millis();
  bool is_reverse = 0;
  
  //While no buttons are pressed
  while( !(!digitalRead(COLOR) && flag_color) && !(!digitalRead(PULSE) && flag_pulse) ){    

    //Color button released
    if(digitalRead(COLOR) && !flag_color){
      flag_color = 1;
    }

    //Pulse button released
    if(digitalRead(PULSE) && !flag_pulse){
      flag_pulse = 1;
    }
    
    if(millis() - timerStart > wait){
      //Set pixel color
      strip.setPixelColor(0, map(i, 0, 255, 0, c / 65536), map(i, 0, 255, 0, (c % 65536) / 256), map(i, 0, 255, 0, c % 256));
      strip.setPixelColor(1, map(i, 0, 255, 0, c / 65536), map(i, 0, 255, 0, (c % 65536) / 256), map(i, 0, 255, 0, c % 256));
      strip.setPixelColor(2, map(i, 0, 255, 0, c / 65536), map(i, 0, 255, 0, (c % 65536) / 256), map(i, 0, 255, 0, c % 256));
      strip.setPixelColor(3, map(i, 0, 255, 0, c / 65536), map(i, 0, 255, 0, (c % 65536) / 256), map(i, 0, 255, 0, c % 256));
      strip.setPixelColor(4, map(i, 0, 255, 0, c / 65536), map(i, 0, 255, 0, (c % 65536) / 256), map(i, 0, 255, 0, c % 256));
      strip.show();
      
      //Reset timer
      timerStart = millis();
    
      //Increment/Decrement i
      if(is_reverse){
        i--;
      }
      else{
        i++;
      }
      
      //Is i maxed/minned? flip bool
      if(i >= 255 || i <= 0){
        is_reverse = !is_reverse;
      }
    }  
  }
}

//Rainbow
void rainbow(uint8_t wait) {
  uint16_t i = 0;
  uint64_t timerStart = millis();
  bool is_reverse = 0;
  
  //While no buttons are pressed
  while( !(!digitalRead(COLOR) && flag_color) && !(!digitalRead(PULSE) && flag_pulse) ){    

    //Color button released
    if(digitalRead(COLOR) && !flag_color){
      flag_color = 1;
    }

    //Pulse button released
    if(digitalRead(PULSE) && !flag_pulse){
      flag_pulse = 1;
    }
    
    if(millis() - timerStart > wait){
      //Set pixel color
      strip.setPixelColor(0, Wheel((0+i) & 255));
      strip.setPixelColor(1, Wheel((1+i) & 255));
      strip.setPixelColor(2, Wheel((2+i) & 255));
      strip.setPixelColor(3, Wheel((3+i) & 255));
      strip.setPixelColor(4, Wheel((4+i) & 255));
      strip.show();
      
      //Reset timer
      timerStart = millis();
    
      //Increment/Decrement i
      if(is_reverse){
        i--;
      }
      else{
        i++;
      }
      
      //Is i maxed/minned? flip bool
      if(i >= 255 || i <= 0){
        is_reverse = !is_reverse;
      }
    }  
  }
}

//Theatre-style crawling lights.
void theaterChase(uint32_t c, uint8_t wait) {
  uint64_t timerStart = millis();
  bool invert = 0;
  
  //While no buttons are pressed
  while( !(!digitalRead(COLOR) && flag_color) && !(!digitalRead(PULSE) && flag_pulse) ){
    //Color button released
    if(digitalRead(COLOR) && !flag_color){
      flag_color = 1;
    }

    //Pulse button released
    if(digitalRead(PULSE) && !flag_pulse){
      flag_pulse = 1;
    }
    
    if(millis() - timerStart > wait){

      //Set pixel color
      if(invert){
        strip.setPixelColor(0, c);
        strip.setPixelColor(1, 0);
        strip.setPixelColor(2, c);
        strip.setPixelColor(3, 0);
        strip.setPixelColor(4, c);
        strip.show();
      }
      else{
        strip.setPixelColor(0, 0);
        strip.setPixelColor(1, c);
        strip.setPixelColor(2, 0);
        strip.setPixelColor(3, c);
        strip.setPixelColor(4, 0);
        strip.show();
      }

      //Reset timer
      timerStart = millis();

      //Invert light states
      invert = !invert;
    }
  }
}

// Input a value 0 to 255 to get a color value.
// The colours are a transition r - g - b - back to r.
uint32_t Wheel(byte WheelPos) {
  WheelPos = 255 - WheelPos;
  if(WheelPos < 85) {
   return strip.Color(255 - WheelPos * 3, 0, WheelPos * 3);
  } else if(WheelPos < 170) {
    WheelPos -= 85;
   return strip.Color(0, WheelPos * 3, 255 - WheelPos * 3);
  } else {
   WheelPos -= 170;
   return strip.Color(WheelPos * 3, 255 - WheelPos * 3, 0);
  }
}

