#include <Adafruit_CircuitPlayground.h>
#include <AsyncDelay.h>

//My game idea is to have a guessing game as to whether the scale or song played is 
//in major or minor. There will be two game modes; one where it is songs that are
// played and one that has scales get played. The positive button will correspond to
// a major scale, while the negative button will correspond to the minor scale. 
//Once you get 5 points in either game mode you win, and you get points if you guess the scale right. 
//When you guess the scale right it will flash green, and when you get it wrong the lights will flash red. 
//Score will be kept in white on the leds whenever the game isn't flashing. 
//Maybe have a time limit on how much time you get to guess.

AsyncDelay delay_10s;

volatile bool rButtFlag;
volatile bool lButtFlag;
volatile bool slideFlag;
volatile bool playScale = true;

const int leftButtPin = 4;
const int rightButtPin = 5;
const int switchPin = 7;

unsigned long int i;

int switchState; //if the game is on or off
int guess;
volatile bool guessState = false;//if true they have guessed, if false they haven't guessed
int correct;
int points; // how many points player has
int pointsThresh = 10; // how many points a player can get


//music stuff
float midi[127];
int A_four = 440;
//Setting up arrays of scales to then randomize which scale gets chosen
int majorArray[7][8] = {
  {69, 71, 73, 74, 76, 78, 80, 81}, {71, 73, 75, 76, 78, 80, 82, 83}, {60, 62, 64, 65, 67, 69, 71, 72}, {62, 64, 66, 67, 69, 71, 73, 74}, {64, 66, 68, 69, 71, 73, 75, 76}, {65, 67, 69, 70, 72, 74, 76, 77}, {67, 69, 71, 72, 74, 76, 78, 79}};
//                      A major                    B Major                               C major                       D major                           E major                                     F major                             G major
int minorArray[7][8] = {{69, 71, 72, 74, 76, 77, 79 ,81}, {71, 73, 74, 76, 78, 79, 81, 83}, {60, 62, 63, 65, 67, 68, 70, 72}, {62, 64, 65, 67, 69, 70, 72, 74}, {64, 66, 67, 69, 71, 72, 74, 76}, {65, 67, 68, 70, 72, 73, 75, 77}, {67, 69, 70, 72, 74, 75, 77, 79}};
//                              A minor                           B minor                           C minor                         D minor                                 E minor                           F minor                       G minor     
int sadSong[][2] = {
    {69, 100},  // song[0][0]], song[0][1]]
    {65, 100},  // song[1][0]], song[1][1]]
    {62, 100},  // song[2][0]], song[2][1]]
  };

int happySong[][2] = {
    {71, 100},  // song[0][0]], song[0][1]]
    {75, 100},  // song[1][0]], song[1][1]]
    {78, 100},  // song[2][0]], song[2][1]]
  };

void setup() {
  Serial.begin(9600);
  CircuitPlayground.begin();

  while(!Serial); // Pause program till serial opens
  Serial.println("Starting timers");
  generateMIDI();
  randomSeed(analogRead(0));
  //add switch pins and attach inturrupt

  attachInterrupt(leftButtPin, leftbuttonISR, RISING);
  attachInterrupt(rightButtPin, rightbuttonISR, RISING);
  attachInterrupt(switchPin, slideswitchISR, CHANGE);
    //read switchpin and save it to a variable
  switchState = digitalRead(7); 
}


void loop() {
//what to do if the switch changes
  if(slideFlag){
    delay(500);
    switchState = digitalRead(7);
    points = 0;
//if the switch state is true turn on
    if(switchState){
      Serial.println("On");
      delay(100);
// add startup function here
      //pointDisplay();
      Serial.println(points);
      delay_10s.start(10000, AsyncDelay::MILLIS);
      for(int j = 0; j < 10;){
        if(points < pointsThresh){
          Serial.print("Round: ");
          Serial.println(j);
          delay(100);
          guessState = false;
          
          if(!guessState && !delay_10s.isExpired()){
            if(!guessState && playScale){
              playRandomScale();
              playScale = false;
            }
            if (lButtFlag){
              guessState = true;
              playScale = true;
              handleGuess(1);
              lButtFlag =  false;
              j++;
              delay_10s.restart();
            }
            if (rButtFlag) {
              guessState = true;
              playScale = true;
              handleGuess(2);
              rButtFlag =  false;
              j++;
              delay_10s.restart();
            }
          }
          else{
            delay(100);
          }
        }

        //add in a shut down function

        // Add in a start up funtion with rainbowness to indicate it starts
        if(delay_10s.isExpired()){
          wrongGuess();
          points--;
          guessState = 1;
          CircuitPlayground.clearPixels();
          pointDisplay();
          delay_10s.repeat();
        }
        
      }
    }
    else{
      Serial.println("Off");
    }
  }
}

void wrongGuess() {
  CircuitPlayground.clearPixels();
  i = 0xFF0000;
  setColors();
  for(int q = 0; q < sizeof(sadSong) / sizeof(sadSong[0]); q++){
    CircuitPlayground.playTone(midi[sadSong[q][0]], sadSong[q][1]);
  }
  CircuitPlayground.clearPixels();
}

void correctGuess() {
  CircuitPlayground.clearPixels();
  i = 0x00FF00;
  setColors();
  for(int q = 0; q < sizeof(happySong) / sizeof(happySong[0]); q++){
    CircuitPlayground.playTone(midi[happySong[q][0]], happySong[q][1]);
  }
  CircuitPlayground.clearPixels();
}

void handleGuess(int guess) {
  if (guess == correct) {
    correctGuess();
    points++;
    Serial.println(points);
    playScale = true;
  } else {
    wrongGuess();
    points--;
    Serial.println(points);
    playScale = true;
  }
}

void randomMajorScale(){
  Serial.println("major scale");
  int randomIndex = random(0, 7);  
  for (int i = 0; i < 8; i++) {
    CircuitPlayground.playTone(midi[majorArray[randomIndex][i]], 100); 
    }
}

void randomMinorScale(){
  Serial.println("minor scale");
  int randomIndex = random(0, 7);  
  for (int i = 0; i < 8; i++) {
    CircuitPlayground.playTone(midi[minorArray[randomIndex][i]], 100); 
    }
}

void playRandomScale() {
  int randomScale = random(0, 2); // 0 for minor, 1 for major
  Serial.println(randomScale);
  if (randomScale == 1) {
    correct = 1; // Major scale
    Serial.println("Major Scale, correct button is right");
    randomMajorScale();
  } else {
    correct = 2; // Minor scale
    Serial.println("Minor Scale, correct button is left");
    randomMinorScale();
  }
}

void pointDisplay(){

}

//function sets all pixles one color
void setColors(){

CircuitPlayground.setPixelColor(1, i);
CircuitPlayground.setPixelColor(2, i);
CircuitPlayground.setPixelColor(3, i);
CircuitPlayground.setPixelColor(4, i);
CircuitPlayground.setPixelColor(5, i);
CircuitPlayground.setPixelColor(6, i);
CircuitPlayground.setPixelColor(7, i);
CircuitPlayground.setPixelColor(8, i);
CircuitPlayground.setPixelColor(9, i);
CircuitPlayground.setPixelColor(10, i);

}

void generateMIDI()
{
  for (int x = 0; x < 127; ++x)
  {
    midi[x] = (A_four / 32.0) * pow(2.0, ((x - 9.0) / 12.0));
    Serial.println(midi[x]);
  }
}

void slideswitchISR(){
  slideFlag = true;
}

void rightbuttonISR(){
  rButtFlag = true;
}

void leftbuttonISR(){
  lButtFlag = true;
}