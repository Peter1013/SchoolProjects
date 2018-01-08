/* Written by John Stone & Pedro De Los Reyes */

#include <motordriver_4wd.h>
#include <seeed_pwm.h>
#include <ChainableLED.h>

int counter = 0;
//-------- PIE constants

const double PIE    = 3.14159265;
const double PIE_O2 = PIE/2.0;
const double PIE2 = PIE*2.0;

//-------- LED crap

#define NUM_LEDS  1
ChainableLED leds(A0, A1, NUM_LEDS);

//-------- Ultrasonic Sensor carp

const int pingpin = 11; 

//-------- motor control 

void TurnLeft90();
void TurnRight90();
void Straight( int speed, int dirn );

//-------- dead reckoning 

// ticks per rotation
#define TPR 72
 
// robot measurements (mm)
#define RW    42.5  // radius wheel
#define D     158.0

// robot config variables
double x = 100.0, y = 100.0, dx = 0.0, dy = 0.0;
double theta =  PI/2.0;

// encoder variables
volatile long left_encoder_count = 0, right_encoder_count = 0;   
int left_dirn = 1, right_dirn = 1;


//-------- robot state 

enum {WANDER, GOHOME, INCLINE} state;

enum {FWD, REV} wander_state;


//-------- Grid state
// -1 = unvisited
// 0 = visited and empty
// 99 = visited and occupied
char grid[5][5];

//-------- model of environment 

double LEFT = 0.0;
double RIGHT = 1450.0;
double BOTTOM = 0.0;
double TOP = 1450.0;


//-------- servo stuffs
int neckPos = 0;
int servoPin=13;
long dir=-1;
int i=0;

void Pan(int pos){
  for(int i=0; i<100;i++)
  {
    digitalWrite(servoPin,HIGH);
    delayMicroseconds(pos);
    digitalWrite(servoPin,LOW);
    delayMicroseconds(20000-pos);
  }
}

//======================================================================================
// setup
//======================================================================================
void setup()
{
    Serial.begin(9600);
    for(int j = 0; j<5; j++ ){
      for(int k = 0; k<5; k++){
        grid[j][k] = '?';
      }
    }
    grid[0][0] = 'o';
    leds.init();
    MOTOR.init();
    Pan(1300);
    attachInterrupt(0, LeftEncoder, CHANGE);
    attachInterrupt(1, RightEncoder, CHANGE);
        
    // go straight
    Straight( 20, 1 ); 
    leds.setColorRGB(0, 0, 100, 0);  // green
    state = WANDER;
    wander_state = FWD;
       
    //Serial.begin(9600);
}

//======================================================================================
// Loop
//======================================================================================
void loop()
{
  switch(state) {
    case WANDER:
      Wander();
      break;
    case GOHOME:
      GoHomeHelen();
      break;
    case INCLINE:
      Serial.print("Incline\n");
      break;
    default:
      Serial.print("Oops\n");
  }
}


//======================================================================================
// TurnLeft90
//======================================================================================
void
TurnLeft90()
{
    right_encoder_count = left_encoder_count = 0;
    
    left_dirn = -1; right_dirn = 1;
    MOTOR.setSpeedDir1(25, DIRR); MOTOR.setSpeedDir2(25, DIRR); 
    while (right_encoder_count < 66)
    {
      delayMicroseconds(1);
    }

    MOTOR.setSpeedDir1(0, DIRF); MOTOR.setSpeedDir2(0, DIRR);
    theta = fmod(theta + PIE_O2, PIE2);
}

//======================================================================================
// TurnRight90
// dirn is 1 for right, -1 for left
//======================================================================================
void
TurnRight90()
{
    right_encoder_count = left_encoder_count = 0;
    
    left_dirn = 1; right_dirn = -1;
    MOTOR.setSpeedDir1(25, DIRF); MOTOR.setSpeedDir2(25, DIRF); 
    while (left_encoder_count < 70)
    {
      delayMicroseconds(1);
    }

    MOTOR.setSpeedDir1(0, DIRF); MOTOR.setSpeedDir2(0, DIRR);
    theta = fmod(theta - PIE_O2, PIE2);
}

//======================================================================================
// Straight
// dirn is 1 for fwd, -1 for bwd
//======================================================================================
void
Straight( int speed, int dirn )
{
    //---- setup encoder variables
    left_dirn = dirn; right_dirn = dirn;
    
    if (speed == 0)       //-- stop
    {
      MOTOR.setSpeedDir1(0, DIRF); MOTOR.setSpeedDir2(0, DIRR);   return;
    }
    else if (dirn == 1)   //-- fwd
    {
      MOTOR.setSpeedDir1(speed-0.1*speed, DIRF); MOTOR.setSpeedDir2(speed, DIRR); 
    }
    else                  //-- bwd
    {
      MOTOR.setSpeedDir1(speed-0.1*speed, DIRR); MOTOR.setSpeedDir2(speed, DIRF); 
    }
}
//======================================================================================
// Ping
// dirn is 1 for fwd, -1 for bwd
//======================================================================================
int Ping(int pingPin)
{
  long duration, cm;
  pinMode(pingPin,OUTPUT);
  digitalWrite(pingPin,LOW);
  delayMicroseconds(2);
  digitalWrite(pingPin,HIGH);
  delayMicroseconds(5);
  digitalWrite(pingPin,LOW);
  pinMode(pingPin,INPUT);
  duration = pulseIn(pingPin,HIGH);
  cm = duration/29.0/2.0;
  delay(100);
  return cm;
}


//======================================================================================
// Interrupt Service Routines for encoders
//======================================================================================
void LeftEncoder()
{
  left_encoder_count = left_encoder_count + left_dirn;
}

void RightEncoder()
{
  right_encoder_count = right_encoder_count + right_dirn;
}

//======================================================================================
// Print Grid
//======================================================================================
void printGrid() {
  for(int i=0; i<5; i++){
    for(int j=0; j<5; j++){
      Serial.print(grid[i][j]); Serial.print(" ");
    }
  Serial.print("\n");
  }
}

//======================================================================================
// Wander
//======================================================================================
void Wander() {
  // If we have reached our goal state, DISCO FEVER~
  if(x >1300 && y > 1300) {
    Straight(0, 0);
    state = GOHOME;
    int dance = 0;
    while(dance<50) {
      dance++;
      leds.setColorRGB(0, random(256), random(256), random(256));
      leds.setColorHSB(0, random(1, 1000)/1000.0, random(1, 1000)/1000.0, random(1, 1000)/1000.0);
      delay(100);
    } 
    TurnRight90();
    delay(500);
    TurnRight90();
    delay(500);
    Straight(25,1);
  }

  // Checking if obstructions
  int dist_cm = Ping(pingpin);
  Serial.println(dist_cm);
  if(dist_cm < 17)
  {
    int gx = (int) (x + PIE * RW * cos(theta) * ((double)((left_encoder_count+72) + (right_encoder_count+72)) / TPR))/300;
    int gy = (int) (y + PIE * RW * sin(theta) * ((double)((left_encoder_count+72) + (right_encoder_count+72)) / TPR))/300;
    if(gx<5 && gy <5 && gx>=0 && gy>=0){
      grid[gx][gy]='X';
    }
    
    Straight(0, 0);
    leds.setColorRGB(0, 255, 0, 0);
    leds.setColorHSB(0, 0.5,  1.0, 0.5);
    delay(100);
    if(theta <= PIE && theta >= PIE_O2){
      if(x>y) {
        
      }
      TurnRight90();
      Straight(20,1);
    }
    else{
      TurnLeft90();
      Straight(20,1);
    }
    /*Pan(2600);
    int distL=Ping(11);
    if(distL<=25){
      gx = (int) (x + PIE * RW * cos(theta+PIE_O2) * ((double)((left_encoder_count+72) + (right_encoder_count+72)) / TPR))/300;
      gy = (int) (y + PIE * RW * sin(theta+PIE_O2) * ((double)((left_encoder_count+72) + (right_encoder_count+72)) / TPR))/300;
      if(gx<5 && gy <5 && gx>=0 && gy>=0){
        grid[gx][gy]='X';
      }
    }
    Serial.println(distL);
    delay(100);
    Pan(1300);
    Pan(0);
    int distR=Ping(11);
    if(distR<=25){
      gx = (int) (x + PIE * RW * cos(theta-PIE_O2) * ((double)((left_encoder_count+72) + (right_encoder_count+72)) / TPR))/300;
      gy = (int) (y + PIE * RW * sin(theta-PIE_O2) * ((double)((left_encoder_count+72) + (right_encoder_count+72)) / TPR))/300;
      if(gx<5 && gy <5 && gx>=0 && gy>=0){
        grid[gx][gy]='X';
      }
    }
    Serial.println(distR);
    delay(100);
    Pan(1300);
    delay(50);
    int glx = (int) (x + PIE * RW * cos(theta+PIE_O2) * ((double)((left_encoder_count+72) + (right_encoder_count+72)) / TPR));
    int gly = (int) (y + PIE * RW * sin(theta+PIE_O2) * ((double)((left_encoder_count+72) + (right_encoder_count+72)) / TPR));
    int grx = (int) (x + PIE * RW * cos(theta-PIE_O2) * ((double)((left_encoder_count+72) + (right_encoder_count+72)) / TPR));
    int gry = (int) (y + PIE * RW * sin(theta-PIE_O2) * ((double)((left_encoder_count+72) + (right_encoder_count+72)) / TPR));
    if((distR > distL && distR > 25) || (grx < RIGHT && grx > LEFT && gry < TOP && gry > BOTTOM) ){
      Serial.println("right");
      delay(10);
      TurnRight90();
      Straight(20,1);
    }
    else if((distL > distR && distL > 25) || (glx < RIGHT && glx > LEFT && gly < TOP && gly > BOTTOM)){
      Serial.println("left");
      delay(10);
      TurnLeft90();
      Straight(20,1);
    }
    else{
      Serial.println("reverse");
      delay(10);
      TurnLeft90();
      TurnLeft90();
      Straight(20,1);
    }*/
  }

  
  
  delay(50);
  
  //---- update robot config (x,y,theta)
  dx = PIE * RW * cos(theta) * ((double)(left_encoder_count + right_encoder_count) / TPR);
  Serial.print("theta: "); Serial.println(theta);
  x = x + dx;
  
  dy = PIE * RW * sin(theta) * ((double)(left_encoder_count + right_encoder_count) / TPR);
  y = y + dy;
  
  Serial.print("x: "); Serial.print(x); Serial.print(" y: "); Serial.print(y); Serial.print("\n");
  Serial.print("x: "); Serial.print((int)x/300); Serial.print(" y: "); Serial.print((int)y/300); Serial.print("\n");
  grid[(int)(x/300)][(int)(y/300)] = 'o';

  counter++;
  if(counter % 20 == 0)
    printGrid();
  
  right_encoder_count = left_encoder_count = 0;
  
  //---- a simple two-state behavior to stay in the box
  
  if ((wander_state == FWD) && (x >= RIGHT || x <= LEFT || y >= TOP || y <= BOTTOM))
  {
    //---- stop
    Straight( 0, 0 );    
    delay(100);

    //---- back up
    leds.setColorRGB(0, 100, 0, 0);  // red
    Straight( 15, -1 );    
    delay(500);

    //---- update state
    wander_state = REV;    
  }
  else
  if ((wander_state == REV) && (x < RIGHT && x > LEFT && y < TOP && y > BOTTOM))
  {
    //---- stop
    Straight( 0, 0 );    

    //---- turn right 90
    leds.setColorRGB(0, 0, 0, 100);
    if(theta <= PIE && theta >= PIE_O2){
      TurnRight90();
      Straight(20,1);
    }
    else{
      TurnLeft90();
      Straight(20,1);
    }
    //---- update robot config (theta)
    //theta  = fmod(theta - PIE_O2, PIE2);
    delay(100);

    //---- go straight
    leds.setColorRGB(0, 0, 100, 0);  // green
    //Straight( 20, 1 );   

    //---- update state
    wander_state = FWD;    
  }
  dist_cm = 100000;
}

//======================================================================================
// GO Home
//======================================================================================
void GoHomeHelen() {
  // If we have reached our goal state, DISCO FEVER~
  if(x <150 && y < 150) {
    Straight(0, 0);
    state = INCLINE;
    int dance = 0;
    while(dance<50) {
      dance++;
      leds.setColorRGB(0, random(256), random(256), random(256));
      leds.setColorHSB(0, random(1, 1000)/1000.0, random(1, 1000)/1000.0, random(1, 1000)/1000.0);
      delay(100);
    } 
  }

  // Checking if question mark
  int gx = (int) (x + PIE * RW * cos(theta) * ((double)((left_encoder_count+30) + (right_encoder_count+30)) / TPR))/300;
  int gy = (int) (y + PIE * RW * sin(theta) * ((double)((left_encoder_count+30) + (right_encoder_count+30)) / TPR))/300;
    
  if(gx<5 && gy <5 && gx>=0 && gy>=0)
  {
    if(grid[gx][gy]=='?'){
      Straight(0, 0);
      leds.setColorRGB(0, 255, 0, 0);
      leds.setColorHSB(0, 0.5,  1.0, 0.5);
      delay(100);
      if(theta <= 0 && theta >= -PIE_O2){
        TurnRight90();
        Straight(20,1);
      }
      else{
        TurnLeft90();
        Straight(20,1);
      }
    }
  }
  
  delay(50);
  
  //---- update robot config (x,y,theta)
  dx = PIE * RW * cos(theta) * ((double)(left_encoder_count + right_encoder_count) / TPR);
  Serial.print("theta: "); Serial.println(theta);
  x = x + dx;
  
  dy = PIE * RW * sin(theta) * ((double)(left_encoder_count + right_encoder_count) / TPR);
  y = y + dy;
  
  Serial.print("x: "); Serial.print(x); Serial.print(" y: "); Serial.print(y); Serial.print("\n");
  Serial.print("x: "); Serial.print((int)x/300); Serial.print(" y: "); Serial.print((int)y/300); Serial.print("\n");
  grid[(int)(x/300)][(int)(y/300)] = 'o';

  counter++;
  if(counter % 20 == 0)
    printGrid();
  
  right_encoder_count = left_encoder_count = 0;
  
  //---- a simple two-state behavior to stay in the box
  
  if ((wander_state == FWD) && (x >= RIGHT || x <= LEFT || y >= TOP || y <= BOTTOM))
  {
    //---- stop
    Straight( 0, 0 );    
    delay(100);

    //---- back up
    leds.setColorRGB(0, 100, 0, 0);  // red
    Straight( 15, -1 );    
    delay(500);

    //---- update state
    wander_state = REV;    
  }
  else
  if ((wander_state == REV) && (x < RIGHT && x > LEFT && y < TOP && y > BOTTOM))
  {
    //---- stop
    Straight( 0, 0 );    

    //---- turn right 90
    leds.setColorRGB(0, 0, 0, 100);
    if(theta <= 0 && theta >= -PIE_O2){
        TurnRight90();
        Straight(25,1);
      }
      else{
        TurnLeft90();
        Straight(25,1);
      }
    //---- update robot config (theta)
    //theta  = fmod(theta - PIE_O2, PIE2);
    delay(100);

    //---- go straight
    leds.setColorRGB(0, 0, 100, 0);  // green
    //Straight( 25, 1 );   

    //---- update state
    wander_state = FWD;    
  }
}

