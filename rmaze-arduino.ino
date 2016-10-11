//#include <GY80.h>
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
//GY80 gy80 = GY80();

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
/*
bool isramp()
{
    GY80_single_scaled accel = a_read_scaled();
    float mod = sqrt(accel.a_x*accel.a_x + accel.a_y*accel.a_y + accel.a_z*accel.a_z);
    float dot = -accel.a_z;
    dot /= mod;
    
}
*/
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

//aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa


void align(){
  //alinhar angularmente
  unsigned long tout = 2000;
  if(wallf() || wallr() || walll()){
      unsigned long t0 = millis();
      if(wallf()){//alinhar pela frente
          const int Kpf = 5;
          while(millis() - t0 < tout){
              int error = analogRead(FR_SHARP) - analogRead(FL_SHARP);
              error *= Kpf;
              constrain(error, -1000, 1000);
              Herkulex.moveSpeedOne(RF_HKL, -error, 1, HKL_LED);
              Herkulex.moveSpeedOne(RB_HKL, -error, 1, HKL_LED);
              Herkulex.moveSpeedOne(LF_HKL, -error, 1, HKL_LED);
              Herkulex.moveSpeedOne(LB_HKL, -error, 1, HKL_LED);
          }
      }
    
      else if(walll()){//alinhar pela esquerda
          const int Kpl= 10;
          while(millis() - t0 < tout){
              int error = analogRead(LF_SHARP) - analogRead(LB_SHARP);
              error *= Kpl;
              constrain(error, -1000, 1000);
              Herkulex.moveSpeedOne(RF_HKL, -error, 1, HKL_LED);
              Herkulex.moveSpeedOne(RB_HKL, -error, 1, HKL_LED);
              Herkulex.moveSpeedOne(LF_HKL, -error, 1, HKL_LED);
              Herkulex.moveSpeedOne(LB_HKL, -error, 1, HKL_LED);
          }
      }
      
      else{//alinhar pela direita
          const int Kpd = 10;
          while(millis() - t0 < tout){
              int error = analogRead(RF_SHARP) - analogRead(RB_SHARP);
              error *= Kpd;
              constrain(error, -1000, 1000);
              Herkulex.moveSpeedOne(RF_HKL,  error, 1, HKL_LED);
              Herkulex.moveSpeedOne(RB_HKL,  error, 1, HKL_LED);
              Herkulex.moveSpeedOne(LF_HKL,  error, 1, HKL_LED);
              Herkulex.moveSpeedOne(LB_HKL,  error, 1, HKL_LED);
          }
      }

      Herkulex.moveSpeedOne(RF_HKL,  1, 1, HKL_LED);
      Herkulex.moveSpeedOne(RB_HKL,  1, 1, HKL_LED);
      Herkulex.moveSpeedOne(LF_HKL,  1, 1, HKL_LED);
      Herkulex.moveSpeedOne(LB_HKL,  1, 1, HKL_LED);
    //alinhar linearmente

  }
}

//aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa

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

void setup()
{
    Serial.begin(9600);

    Serial.println("Starting setup...");

    Serial.println("Initializing MLX and GY80...");

    fmlx.begin();
    rmlx.begin();
    lmlx.begin();
    //gy80.begin();

    Serial.println("Initializing  motors...");

    Herkulex.beginSerial1(115200);
    Herkulex.reboot(BROAD_HKL);
    delay(500);
    Herkulex.initialize();
    Herkulex.torqueON(BROAD_HKL);

    Serial.println("Setup ended.");
    delay(2000);
}

int cnt = 0;
int knt = 0;
void loop()
{
    cnt ++;
    knt ++;
    if(isblack())
    {
        walkb(1);
        if(wallr())
            turnr(2);
        else
        {
            turnr(1);
            walkf(1);
        }
    }
    else
    {
        if(!walll())
        {
            turnl(1);
            walkf(1);
        }
        else if (!wallf())
            walkf(1);
        else if (!wallr())
        {
            turnr(1);
            walkf(1);
        }
        else
            turnr(2);
    }
    if(!(cnt %= 3) && knt > 12 && !isblack()) drop(3);
}
