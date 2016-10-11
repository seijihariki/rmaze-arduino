//#include <>
#include <Herkulex.h>
#include <Adafruit_MLX90614.h>

// Calib

const int dst_thr = 200;
const int col_thr = 850;
const int tmp_thr = 6;

const int walkpot = 1000;
const int calib   = 60;
const int walk_t  = 1800;

const int turnpot = 1000;
const int turn_t  = 1100;

// Fixed

Adafruit_MLX90614 fmlx = Adafruit_MLX90614();
Adafruit_MLX90614 rmlx = Adafruit_MLX90614();
Adafruit_MLX90614 lmlx = Adafruit_MLX90614();

const int LF_HKL    = 0xfd;
const int RF_HKL    = 10;
const int RB_HKL    = 38;
const int LB_HKL    = 20;
const int DR_HKL    = 50;
const int BROAD_HKL = 0xfe;

const int FR_SHARP  = 10;
const int FL_SHARP  = 12;
const int RF_SHARP  = 14;
const int RB_SHARP  = 13;
const int LF_SHARP  = 9;
const int LB_SHARP  = 11;

const int QTR_AIN   = 15;

const int ledpin    = 22;

const int HKL_LED   = 0x06;

// Sensor functions

int check_victim()
{
  if(fmlx.readObjectTempC() - fmlx.readAmbientTempC() > tmp_thr) return 0;
  if(rmlx.readObjectTempC() - rmlx.readAmbientTempC() > tmp_thr) return 1;
  if(lmlx.readObjectTempC() - lmlx.readAmbientTempC() > tmp_thr) return 3;
  return -1;
}

bool isblack()
{
    return analogRead(QTR_AIN) > col_thr;
}

bool wallf()
{
    return (analogRead(FR_SHARP) + analogRead(FL_SHARP))/2 > dst_thr;
}

bool wallr()
{
    return (analogRead(RF_SHARP) + analogRead(RB_SHARP))/2 > dst_thr;
}

bool walll()
{
    return (analogRead(LF_SHARP) + analogRead(LB_SHARP))/2 > dst_thr;
}

// Movement functions

void drop(int side)
{
    for(int i = 0; i < 6; i++)
    {
        digitalWrite(ledpin, HIGH);
        delay(500);
        digitalWrite(ledpin, LOW);
        delay(500);
    }
    switch(side)
    {
    case 0:
        turnr(2);
        break;
    case 1:
        turnr(1);
        break;
    case 3:
        turnl(1);
        break;
    default:
        break;
    }

    Herkulex.moveOneAngle(DR_HKL, 0, 1, 0x06);
    delay(1000);
    Herkulex.moveOneAngle(DR_HKL, -95, 1, 0x06);
    delay(1000);

    switch(side)
    {
    case 0:
        turnr(2);
        break;
    case 1:
        turnl(1);
        break;
    case 3:
        turnr(1);
        break;
    default:
        break;
    }
}

void align()
{

}

int walkf(int cnt)
{
    int victim = -1;
    int i;
    for(i = 0; i < cnt && !wallf() && victim < 0; i++)
    {
        unsigned long tile_init = millis();
        Herkulex.moveSpeedOne(RF_HKL, -walkpot, 1, HKL_LED);
        Herkulex.moveSpeedOne(RB_HKL, -walkpot, 1, HKL_LED);
        Herkulex.moveSpeedOne(LF_HKL,  walkpot - calib, 1, HKL_LED);
        Herkulex.moveSpeedOne(LB_HKL,  walkpot - calib, 1, HKL_LED);
        while(millis() - tile_init < walk_t)
        {
            if(victim < 0) victim = check_victim();
        }
    }

    Herkulex.moveSpeedOne(BROAD_HKL, 1, 1, HKL_LED);

    if (victim >= 0) drop(victim);
}

int walkb(int cnt)
{
    int i;
    for(i = 0; i < cnt; i++)
    {
        unsigned long tile_init = millis();
        Herkulex.moveSpeedOne(RF_HKL,  walkpot, 1, HKL_LED);
        Herkulex.moveSpeedOne(RB_HKL,  walkpot, 1, HKL_LED);
        Herkulex.moveSpeedOne(LF_HKL, -walkpot + calib, 1, HKL_LED);
        Herkulex.moveSpeedOne(LB_HKL, -walkpot + calib, 1, HKL_LED);
        while(millis() - tile_init < walk_t);
    }

    Herkulex.moveSpeedOne(BROAD_HKL, 1, 1, HKL_LED);
}

int turnr(int cnt)
{
    int victim = -1;
    int i;
    for(i = 0; i < cnt; i++)
    {
    Herkulex.moveSpeedOne(RF_HKL, walkpot, 1, HKL_LED);
    Herkulex.moveSpeedOne(RB_HKL, walkpot, 1, HKL_LED);
    Herkulex.moveSpeedOne(LF_HKL, walkpot - calib , 1, HKL_LED);
    Herkulex.moveSpeedOne(LB_HKL, walkpot - calib, 1, HKL_LED);
        unsigned long tile_init = millis();
        while(millis() - tile_init < turn_t);
    }
    Herkulex.moveSpeedOne(BROAD_HKL, 1, 1, HKL_LED);
}

int turnl(int cnt)
{
    int victim = -1;
    int i;
    for(i = 0; i < cnt; i++)
    {
    Herkulex.moveSpeedOne(RF_HKL, -walkpot, 1, HKL_LED);
    Herkulex.moveSpeedOne(RB_HKL, -walkpot, 1, HKL_LED);
    Herkulex.moveSpeedOne(LF_HKL, -walkpot + calib, 1, HKL_LED);
    Herkulex.moveSpeedOne(LB_HKL, -walkpot + calib, 1, HKL_LED);
 
        unsigned long tile_init = millis();
        while(millis() - tile_init < turn_t);
    }

    Herkulex.moveSpeedOne(BROAD_HKL, 1, 1, HKL_LED);
}

void test_motors()
{
    Serial.println("Testing motors...");
    
    Serial.println("WARNING: Robot WILL walk. Put it on a safe place.");

    Serial.println("Starting in 5 seconds...");
    delay(5000);

    Serial.println("Testing individual motors... Please lift the robot. You have 5 seconds.");
    
    delay(5000);

    Serial.println("Testing Front right motor. Setting speed 1000 for 2 seconds...");

    Herkulex.moveSpeedOne(RF_HKL, 1000, 1, HKL_LED);
    delay(2000);
    Herkulex.moveSpeedOne(RF_HKL, 1, 1, HKL_LED);

    Serial.println("Testing Front left motor. Setting speed 1000 for 2 seconds...");

    Herkulex.moveSpeedOne(LF_HKL, 1000, 1, HKL_LED);
    delay(2000);
    Herkulex.moveSpeedOne(LF_HKL, 1, 1, HKL_LED);

    Serial.println("Testing Back right motor. Setting speed 1000 for 2 seconds...");

    Herkulex.moveSpeedOne(RB_HKL, 1000, 1, HKL_LED);
    delay(2000);
    Herkulex.moveSpeedOne(RB_HKL, 1, 1, HKL_LED);

    Serial.println("Testing Back left motor. Setting speed 1000 for 2 seconds...");

    Herkulex.moveSpeedOne(LB_HKL, 1000, 1, HKL_LED);
    delay(2000);
    Herkulex.moveSpeedOne(LB_HKL, 1, 1, HKL_LED);
    

    Serial.println("Testing Front right motor. Setting speed 500 for 2 seconds...");

    Herkulex.moveSpeedOne(RF_HKL, 500, 1, HKL_LED);
    delay(2000);
    Herkulex.moveSpeedOne(RF_HKL, 1, 1, HKL_LED);

    Serial.println("Testing Front left motor. Setting speed 500 for 2 seconds...");

    Herkulex.moveSpeedOne(LF_HKL, 500, 1, HKL_LED);
    delay(2000);
    Herkulex.moveSpeedOne(LF_HKL, 1, 1, HKL_LED);

    Serial.println("Testing Back right motor. Setting speed 500 for 2 seconds...");

    Herkulex.moveSpeedOne(RB_HKL, 500, 1, HKL_LED);
    delay(2000);
    Herkulex.moveSpeedOne(RB_HKL, 1, 1, HKL_LED);

    Serial.println("Testing Back left motor. Setting speed 500 for 2 seconds...");

    Herkulex.moveSpeedOne(LB_HKL, 500, 1, HKL_LED);
    delay(2000);
    Herkulex.moveSpeedOne(LB_HKL, 1, 1, HKL_LED);

    Serial.println("Put the robot on the floor. You have 5 seconds.");

    delay(5000);

    Serial.println("Walking 1 tile to the front...");

    walkf(1);
    
    Serial.println("Walking 1 more tile to the front...");

    walkf(1);

    Serial.println("Walking 2 tiles to the back...");

    walkf(2);

    Serial.println("Turning right 180 deg....");

    turnr(2);
    
    Serial.println("Turning right more 90 deg....");

    turnr(2);

    Serial.println("Turning left 270 deg....");
    
    turnl(3);

    Serial.println("Ended testing motors.");
}

void test_drop()
{
    Serial.println("Testing dropper...");
    
    Serial.println("WARNING: Robot WILL walk. Put it on a safe place.");

    Serial.println("Starting in 5 seconds...");
    delay(5000);
    
    Serial.println("Dropping rescue kit to the front...");

    drop(0);

    Serial.println("Dropping rescue kit to the right...");

    drop(1);

    Serial.println("Dropping rescue kit to the left...");

    drop(3);
}

void test_sensors()
{
    Serial.println("Testing sensors...");

    Serial.println("Testing sharps...");

    Serial.println("Reading 10 values from Front right sharp...");
    
    for(int i = 0; i < 10; i++) Serial.println(analogRead(FR_SHARP));

    Serial.println("Reading 10 values from Front left sharp...");
    
    for(int i = 0; i < 10; i++) Serial.println(analogRead(FL_SHARP));

    Serial.println("Reading 10 values from Right front sharp...");
    
    for(int i = 0; i < 10; i++) Serial.println(analogRead(RF_SHARP));
    
    Serial.println("Reading 10 values from Right back sharp...");
    
    for(int i = 0; i < 10; i++) Serial.println(analogRead(RB_SHARP));
    
    Serial.println("Reading 10 values from Left front sharp...");
    
    for(int i = 0; i < 10; i++) Serial.println(analogRead(LF_SHARP));
    
    Serial.println("Reading 10 values from Left back sharp...");
    
    for(int i = 0; i < 10; i++) Serial.println(analogRead(LB_SHARP));

    Serial.println("Checking for wall existence...");

    Serial.print("Has wall to the front: ");
    Serial.println(wallf());
    
    Serial.print("Has wall to the right: ");
    Serial.println(wallr());
    
    Serial.print("Has wall to the left: ");
    Serial.println(walll());
    

    Serial.println("Testing mlx...");

    Serial.println("Reading 10 values from front mlx...");
    for(int i = 0; i < 10; i++)
    {
      Serial.print("Ambient: ");
      Serial.print(fmlx.readAmbientTempC());
      Serial.print(" C; Object: ");
      Serial.print(fmlx.readObjectTempC());
      Serial.println(" C");
    }
    
    Serial.println("Reading 10 values from right mlx...");
    for(int i = 0; i < 10; i++)
    {
      Serial.print("Ambient: ");
      Serial.print(rmlx.readAmbientTempC());
      Serial.print(" C; Object: ");
      Serial.print(rmlx.readObjectTempC());
      Serial.println(" C");
    }
    
    Serial.println("Reading 10 values from left mlx...");
    for(int i = 0; i < 10; i++)
    {
      Serial.print("Ambient: ");
      Serial.print(lmlx.readAmbientTempC());
      Serial.print(" C; Object: ");
      Serial.print(lmlx.readObjectTempC());
      Serial.println(" C");
    }
}

void setup()
{
    Serial.begin(9600);

    Serial.println("Starting setup...");

    Serial.println("Initializing MLX...");

    fmlx.begin();
    rmlx.begin();
    lmlx.begin();

    Serial.println("Initializing  motors...");

    Herkulex.beginSerial1(115200);
    Herkulex.reboot(BROAD_HKL);
    delay(500);
    Herkulex.initialize();
    Herkulex.torqueON(BROAD_HKL);

    Serial.println("Setup ended.");
/*
    for(int i = 1;i < 254; i++)
    {
        Serial.print("Motor id: ");
        Serial.println(i);
        Herkulex.moveSpeedOne(i, 1000, 1, 0x06);
        delay(1000);
    }
*/
}

void loop()
{
}
