#include <Adafruit_NeoPixel.h>

#define LED 1
#define SELECTION 0
#define ALTER 2
#define TIMEOUT 4000

uint32_t setColor(uint8_t c);
void setAnimation(uint8_t slide, uint8_t c, uint8_t p, uint32_t *slideTimer, uint8_t *i, bool *invert);
void specAnimation(bool specMode, uint32_t color1, uint32_t color2, uint32_t color3, uint32_t *slideTimer);
void changeMode(uint8_t c, uint8_t p);
void colorWipe(uint8_t slide, uint32_t c);
void colorPulse(uint8_t slide, uint32_t c, uint32_t *slideTimer);
void rainbow(uint8_t slide, uint32_t slideTimer);
void theaterChase(uint8_t slide, uint32_t c, uint32_t *slideTimer, bool *invert);
uint32_t Wheel(byte WheelPos);
void saveMode(uint8_t *c, uint8_t *p);
void runBadge();

// Parameter 1 = number of pixels in strip
// Parameter 2 = ATTINY85 pin number (hardwired to 1)
// Parameter 3 = pixel type flags, add together as needed:
Adafruit_NeoPixel strip = Adafruit_NeoPixel(15, LED, NEO_GRB + NEO_KHZ800);

bool flag_static = 0;
bool rainbow_invert = 0;
bool flag_selection = 0;
uint8_t c_count = 0;
bool flag_alter = 0;
uint8_t p_count = 0;
uint8_t flag_special = 0;
uint32_t timerDeb, timerSel, timerDel, timerS1, timerS2, timerS3 = millis();
uint8_t slide1[2], slide2[2], slide3[2] = {1,1};
bool invertS1, invertS2, invertS3 = 0;
uint8_t countS1, countS2, countS3 = 0;

void setup() {
  pinMode(SELECTION, INPUT_PULLUP);
  pinMode(ALTER, INPUT_PULLUP);
  strip.begin();

  //If SELECTION button is pressed while turning on then run 123123 animation
  if(!digitalRead(SELECTION) && digitalRead(ALTER)){
    flag_selection = 1;
    flag_special = 1;
  }
  //If ALTER button is pressed while turning on then run 12321 animation
  else if(digitalRead(SELECTION) && !digitalRead(ALTER)){
    flag_alter = 1;
    flag_special = 2;
  }
  //If both buttons are pressed while turning on then run normal operation
  else if(!digitalRead(SELECTION) && !digitalRead(ALTER)){
    flag_selection = 1;
    flag_alter = 1;
    flag_special = 0;
  }
  strip.show(); // Initialize all pixels to 'off'
}

void loop() {

  //Selection button pressed
  if(!digitalRead(SELECTION) && flag_selection && millis() - timerDeb > 300){
    timerDeb, timerSel = millis();
    flag_selection = 0;
    c_count++;
    p_count = 0;
    //Reset count if end of selections reached (animated badges only have 3 selections)
    if(c_count > 6 || (c_count > 3 && flag_special > 0))
      c_count = 1; 
    strip.clear();
  }

  //Selection button released
  if(digitalRead(SELECTION) && !flag_selection)
    flag_selection = 1;

  //Alter button pressed (only if in change mode)
  if(millis() - timerSel < TIMEOUT){
    if(!digitalRead(ALTER) && flag_alter && millis() - timerDeb > 300){
      timerDeb, timerSel = millis();
      flag_alter = 0;
      p_count++;
      if( (p_count > 10 && c_count <= 3) || (p_count > 4 && c_count > 3) )
        p_count = 1;
      strip.clear();
    }
  }

  //Alter button released
  if(digitalRead(ALTER) && !flag_alter)
    flag_alter = 1;

  //Changing a badge effect
  if(millis() - timerSel < 2000){
    changeMode(c_count, p_count);
  }
  //Save change and execute animation
  else{
    saveMode(&c_count, &p_count);
    runBadge();
  }
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

void setAnimation(uint8_t slide, uint8_t c, uint8_t p, uint32_t *slideTimer, uint8_t *i, bool *invert){
  switch (p) {
      case 1:
        //Static
        colorWipe(slide, setColor(c));
        break;
      case 2:
        //Pulse slow
        colorPulse(slide, setColor(c), 0, slideTimer, i, invert);
        break;
      case 3:
        //Pulse fast
        colorPulse(slide, setColor(c), 1, slideTimer, i, invert);
        break;
      case 4:
        //Rainbow (ignores color)
        rainbow(slide, slideTimer, i, invert);
        break;
  }
}

void specAnimation(bool specMode, uint32_t color1, uint32_t color2, uint32_t color3, uint32_t *slideTimer){
  //123123
  if(!specMode){
      //1
      if(!invertS1 && millis() - *slideTimer <= 250){
        strip.clear();
        strip.setPixelColor(0, color1);
        strip.setPixelColor(1, color1);
        strip.setPixelColor(2, color1);
        strip.setPixelColor(3, color1);
        strip.setPixelColor(4, color1);
        strip.show();
        invertS1 = 1;
      }
      //2
      if(!invertS2 && millis() - *slideTimer > 250 && millis() - *slideTimer <= 500){
        strip.clear();
        strip.setPixelColor(5, color2);
        strip.setPixelColor(6, color2);
        strip.setPixelColor(7, color2);
        strip.setPixelColor(8, color2);
        strip.setPixelColor(9, color2);
        strip.show();
        invertS2 = 1;
      }
      //3
      if(!invertS3 && millis() - *slideTimer > 500 && millis() - *slideTimer <= 750){
        strip.clear();
        strip.setPixelColor(10, color3);
        strip.setPixelColor(11, color3);
        strip.setPixelColor(12, color3);
        strip.setPixelColor(13, color3);
        strip.setPixelColor(14, color3);
        strip.show();
        invertS3 = 1;
      }
      //Reset
      if(millis() - *slideTimer > 750){
        *slideTimer = millis();
        invertS1 = 0;
        invertS2 = 0;
        invertS3 = 0;
      }
  }
  //12321232
  else{
    //1
    if(!invertS1 && millis() - *slideTimer <= 250){
      strip.clear();
      strip.setPixelColor(0, color1);
      strip.setPixelColor(1, color1);
      strip.setPixelColor(2, color1);
      strip.setPixelColor(3, color1);
      strip.setPixelColor(4, color1);
      strip.show();
      invertS1 = 1;
    }
    //2
    if(!invertS2 && millis() - *slideTimer > 250 && millis() - *slideTimer <= 500){
      strip.clear();
      strip.setPixelColor(5, color2);
      strip.setPixelColor(6, color2);
      strip.setPixelColor(7, color2);
      strip.setPixelColor(8, color2);
      strip.setPixelColor(9, color2);
      strip.show();
      invertS2 = 1;
    }
    //3
    if(!invertS3 && millis() - *slideTimer > 500 && millis() - *slideTimer <= 750){
      strip.clear();
      strip.setPixelColor(10, color3);
      strip.setPixelColor(11, color3);
      strip.setPixelColor(12, color3);
      strip.setPixelColor(13, color3);
      strip.setPixelColor(14, color3);
      strip.show();
      invertS3 = 1;
    }
    //2
    if(invertS2 && millis() - *slideTimer > 750 && millis() - *slideTimer <= 1000){
      strip.clear();
      strip.setPixelColor(5, color2);
      strip.setPixelColor(6, color2);
      strip.setPixelColor(7, color2);
      strip.setPixelColor(8, color2);
      strip.setPixelColor(9, color2);
      strip.show();
      invertS2 = 0;
    }
    //Reset
    if(millis() - *slideTimer > 1000){
      *slideTimer = millis();
      invertS1 = 0;
      invertS2 = 0;
      invertS3 = 0;
    }
  }
}

//Set animation
void changeMode(uint8_t c, uint8_t p){
  switch (c) {
      case 1:
        //Color slide 1
        colorWipe(1, setColor(p));
        break;
      case 2:
        //Color slide 2
        colorWipe(2, setColor(p));
        break;
      case 3:
        //Color slide 3
        colorWipe(3, setColor(p));
        break;
      case 4:
        //Animation slide 1
        setAnimation(1, setColor(c), p, &timerS1, &countS1, &invertS1);
        break;
      case 5:
        //Animation slide 2
        setAnimation(2, setColor(c), p, &timerS2, &countS2, &invertS2);
        break;
      case 6:
        //Animation slide 3
        setAnimation(3, setColor(c), p, &timerS3, &countS3, &invertS3);
        break;
  }
}

//Static color
void colorWipe(uint8_t slide, uint32_t c) {
  strip.setPixelColor((5*slide) - 5, c);
  strip.setPixelColor((5*slide) - 4, c);
  strip.setPixelColor((5*slide) - 3, c);
  strip.setPixelColor((5*slide) - 2, c);
  strip.setPixelColor((5*slide) - 1, c);
  strip.show();
}

//Pulse transition from color, off, color, etc.
void colorPulse(uint8_t slide, uint32_t c,  bool mode, uint32_t *slideTimer, uint8_t *i, bool *invert) {
  uint8_t wait;

  //Mode 1: Fast, Mode 0: Slow
  if(mode)
    wait = 2;
  else
    wait = 10;
 
  if(millis() - *slideTimer > wait){
    //Set pixel color
    strip.setPixelColor(0, map(*i, 0, 255, 0, c / 65536), map(*i, 0, 255, 0, (c % 65536) / 256), map(*i, 0, 255, 0, c % 256));
    strip.setPixelColor(1, map(*i, 0, 255, 0, c / 65536), map(*i, 0, 255, 0, (c % 65536) / 256), map(*i, 0, 255, 0, c % 256));
    strip.setPixelColor(2, map(*i, 0, 255, 0, c / 65536), map(*i, 0, 255, 0, (c % 65536) / 256), map(*i, 0, 255, 0, c % 256));
    strip.setPixelColor(3, map(*i, 0, 255, 0, c / 65536), map(*i, 0, 255, 0, (c % 65536) / 256), map(*i, 0, 255, 0, c % 256));
    strip.setPixelColor(4, map(*i, 0, 255, 0, c / 65536), map(*i, 0, 255, 0, (c % 65536) / 256), map(*i, 0, 255, 0, c % 256));
    strip.show();
    
    //Reset timer
    *slideTimer = millis();
  
    //Increment/Decrement i
    if(*invert)
      *i--;
    else
      *i++;
    
    //Is i maxed/minned? flip bool
    if(*i >= 255 || *i <= 0)
      *invert = !(*invert);
  }
}

//Rainbow
void rainbow(uint8_t slide, uint32_t *slideTimer, uint8_t *i, bool *invert) {
  if(millis() - *slideTimer > 50){    
    strip.setPixelColor(5*slide - 5, Wheel((0+*i) & 255));
    strip.setPixelColor(5*slide - 4, Wheel((1+*i) & 255));
    strip.setPixelColor(5*slide - 3, Wheel((2+*i) & 255));
    strip.setPixelColor(5*slide - 2, Wheel((3+*i) & 255));
    strip.setPixelColor(5*slide - 1, Wheel((4+*i) & 255));
    strip.show();
    
    //Increment/Decrement i
    if(*invert)
      *i--;
    else
      *i++;
    
    //Is i maxed/minned? flip bool
    if(*i <= 0)
      *invert = 0;
    if(*i >= 255)
      *invert = 1;

    *slideTimer = millis();
  }
}

//Theatre-style crawling lights.
void theaterChase(uint8_t slide, uint32_t c, uint32_t *slideTimer, bool *invert) {
      //Set pixel color
      if(millis() - *slideTimer > 80)
        *invert = !(*invert);
      
      if(invert){
        strip.setPixelColor(5*slide - 5, c);
        strip.setPixelColor(5*slide - 4, 0);
        strip.setPixelColor(5*slide - 3, c);
        strip.setPixelColor(5*slide - 2, 0);
        strip.setPixelColor(5*slide - 1, c);
        strip.show();
      }
      else{
        strip.setPixelColor(0+(5*slide - 5), 0);
        strip.setPixelColor(1+(5*slide - 5), c);
        strip.setPixelColor(2+(5*slide - 5), 0);
        strip.setPixelColor(3+(5*slide - 5), c);
        strip.setPixelColor(4+(5*slide - 5), 0);
        strip.show();
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

void saveMode(uint8_t *c, uint8_t *p){
  //Don't save if no modification made
  if(*p){
    switch(*c) {
      case 1:
        slide1[0] = *p;
        break;
      case 2:
        slide2[0] = *p;
        break;
      case 3:
        slide3[0] = *p;
        break;
      case 4:
        slide1[1] = *p;
        break;
      case 5:
        slide2[1] = *p;
        break;
      case 6:
        slide3[1] = *p;
        break;
    }
  }
  *p = 0;
  *c = 0;
}

void runBadge(){
  switch(flag_special){
    case 0:
      setAnimation(1, slide1[1], slide1[0], &timerS1, &countS1, &invertS1);
      setAnimation(2, slide2[1], slide2[0], &timerS2, &countS2, &invertS2);
      setAnimation(3, slide3[1], slide3[0], &timerS3, &countS3, &invertS3);
      break;
    case 1:
      specAnimation(0, setColor(slide1[0]), setColor(slide2[0]), setColor(slide3[0]), &timerS1);
      break;
    case 2:
      specAnimation(1, setColor(slide1[0]), setColor(slide2[0]), setColor(slide3[0]), &timerS1);
      break;
  }
}
