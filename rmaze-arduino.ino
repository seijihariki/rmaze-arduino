#include <L3G4200D.h>

#include <ADXL345.h>

#include <Herkulex.h>
#include <Adafruit_MLX90614.h>

// Calib

const int dst_thr = 200;
const int col_thr = 900;
const int tmp_thr = 5;
const double ramp_thr = 17;

const int walkpot = 1000;
const int calib   = 60;
const int walk_t  = 1700;

const int turnpot = 1000;
const int turn_t  = 1000;
const double turn_gc = 80;

const int walldist = 500;
const int fwalldist = 360;
// Fixed

//Adafruit_MLX90614 fmlx = Adafruit_MLX90614();
Adafruit_MLX90614 rmlx = Adafruit_MLX90614(0x5a);
Adafruit_MLX90614 lmlx = Adafruit_MLX90614(0x59);
ADXL345 gy80 = ADXL345();
L3G4200D gyro = L3G4200D();

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

const int F_DSHARP  = 30;
const int R_DSHARP  = 50;
const int L_DSHARP  = 32;

const int QTR_AIN   = 15;

const int ledpin    = 22;

const int HKL_LED   = 0x06;

// Sensor functions

int check_victim()
{
	//if(fmlx.readObjectTempC() - fmlx.readAmbientTempC() > tmp_thr) return 0;
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
    return ((analogRead(FR_SHARP) + analogRead(FL_SHARP))/2 > dst_thr) || !digitalRead(F_DSHARP);
}

bool wallr()
{
    return ((analogRead(RF_SHARP) + analogRead(RB_SHARP))/2 > dst_thr) || !digitalRead(R_DSHARP);
}

bool walll()
{
    return ((analogRead(LF_SHARP) + analogRead(LB_SHARP))/2 > dst_thr) || !digitalRead(L_DSHARP);
}

bool isramp()
{
    double x, y, z;
    
    gy80.read(&x, &y, &z);
    double angle = atan2(z, y);
    angle = angle/PI*180;
    angle += 90;
    return abs(angle) > ramp_thr;
}

// Movement functions

void upramp()
{
    Herkulex.moveSpeedOne(RF_HKL, -1000, 1, HKL_LED);
    Herkulex.moveSpeedOne(RB_HKL, -1000, 1, HKL_LED);
    Herkulex.moveSpeedOne(LF_HKL,  1000, 1, HKL_LED);
    Herkulex.moveSpeedOne(LB_HKL,  1000, 1, HKL_LED);
    Serial.println(isramp());
    while(isramp())
    {
        delay(500);
    }
    delay(1500);
    Herkulex.moveSpeedOne(BROAD_HKL, 1, 1, HKL_LED);
}

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
        turnl(1);
        break;
    case 3:
        turnr(1);
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
        turnr(1);
        break;
    case 3:
        turnl(1);
        break;
    default:
        break;
    }
}

//aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa


void align(){
    unsigned long tout = 3500;

    if(wallf() || wallr() || walll()){
        unsigned long t0 = millis();
        int Kpr = 5;
        int Kpl = 5;
        int Kpf = 5;

        if (!walll())Kpl = 0;
        if (!wallf())Kpf = 0;

        while(millis() - t0 < tout){
            int errorl = 0;
            int errorf = ((analogRead(FL_SHARP) + analogRead(FR_SHARP))/2) - fwalldist;
            int cntr = 0;
            int cntl = 0;
            int errorr = 0;
            
            if(wallf())
            {
                errorr += analogRead(FR_SHARP) - analogRead(FL_SHARP);
                cntr++;
            } else errorf = 0;

            if(walll())
            {
                errorr += analogRead(LF_SHARP) - analogRead(LB_SHARP);
                cntr++;
                errorl += ((analogRead(LF_SHARP) + analogRead(LB_SHARP))/2) - walldist; 
                cntl++;
            }

            if(wallr())
            {
                errorr += analogRead(RB_SHARP) - analogRead(RF_SHARP);
                cntr++;
                errorl += walldist - ((analogRead(RF_SHARP) + analogRead(RB_SHARP))/2);
                cntl++;
            }

            if (cntr) errorr /= cntr;
            errorr *= Kpr;
            errorf *= Kpf;
            if (cntl) errorl /= cntl;
            errorl *= Kpl;

            if(abs(errorl) < 70 && abs(errorf) < 70 && abs(errorr) < 70) break;

            //if(!digitalRead(F_DSHARP)) errorf *= -.5;

            Herkulex.moveSpeedOne(RF_HKL, constrain((errorf) + (errorl) + errorr, -1000, 1000), 1, HKL_LED);
            Herkulex.moveSpeedOne(RB_HKL, constrain((errorf) - (errorl) + errorr, -1000, 1000), 1, HKL_LED);
            Herkulex.moveSpeedOne(LF_HKL, constrain(-(errorf) + (errorl) + errorr, -1000, 1000), 1, HKL_LED);
            Herkulex.moveSpeedOne(LB_HKL, constrain(-(errorf) - (errorl) + errorr, -1000, 1000), 1, HKL_LED);
        } 

        Herkulex.moveSpeedOne(RF_HKL,  1, 1, HKL_LED);
        Herkulex.moveSpeedOne(RB_HKL,  1, 1, HKL_LED);
        Herkulex.moveSpeedOne(LF_HKL,  1, 1, HKL_LED);
        Herkulex.moveSpeedOne(LB_HKL,  1, 1, HKL_LED);
    }
}

int walkf(int cnt)
{
    int i;
    for(i = 0; i < cnt && !wallf(); i++)
    {
        unsigned long tile_init = millis();
        Herkulex.moveSpeedOne(RF_HKL, -walkpot, 1, HKL_LED);
        Herkulex.moveSpeedOne(RB_HKL, -walkpot, 1, HKL_LED);
        Herkulex.moveSpeedOne(LF_HKL,  walkpot - calib, 1, HKL_LED);
        Herkulex.moveSpeedOne(LB_HKL,  walkpot - calib, 1, HKL_LED);
        while(millis() - tile_init < walk_t);
    }

    Herkulex.moveSpeedOne(BROAD_HKL, 1, 1, HKL_LED);
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

int turnr_gyro(int cnt)
{
    double sum = 0;
    Herkulex.moveSpeedOne(RF_HKL, walkpot, 1, HKL_LED);
    Herkulex.moveSpeedOne(RB_HKL, walkpot, 1, HKL_LED);
    Herkulex.moveSpeedOne(LF_HKL, walkpot - calib , 1, HKL_LED);
    Herkulex.moveSpeedOne(LB_HKL, walkpot - calib, 1, HKL_LED);
    while(sum < turn_gc*cnt)
    {
        sum += gyro.readNormalize().ZAxis*.01;
        delay(10);
    }
    Herkulex.moveSpeedOne(BROAD_HKL, 1, 1, HKL_LED);
}

int turnl_gyro(int cnt)
{
    double sum = 0;
    Herkulex.moveSpeedOne(RF_HKL, -walkpot, 1, HKL_LED);
    Herkulex.moveSpeedOne(RB_HKL, -walkpot, 1, HKL_LED);
    Herkulex.moveSpeedOne(LF_HKL, -walkpot + calib, 1, HKL_LED);
    Herkulex.moveSpeedOne(LB_HKL, -walkpot + calib, 1, HKL_LED);
    while(sum < turn_gc*cnt)
    {
        sum -= gyro.readNormalize().ZAxis*.01;
        delay(10);
    }
    Herkulex.moveSpeedOne(BROAD_HKL, 1, 1, HKL_LED);
}

int turnr(int cnt)
{
    delay(500);
    turnr_gyro(cnt);
    delay(500);
    return cnt;
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
    delay(500);
    turnl_gyro(cnt);
    delay(500);
    return cnt;
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

    Serial.println("Initializing MLX and GY80. Do not disturb IMU!");

    //fmlx.begin();
    rmlx.begin();
    lmlx.begin();
    gy80.begin();
    gyro.begin();
    gyro.calibrate();
    Serial.print("Gyroscope callibrated to ");
    Serial.println(gyro.getThreshold());

    Serial.println("Initializing  motors...");

    Herkulex.beginSerial1(115200);
    Herkulex.reboot(BROAD_HKL);
    delay(500);
    Herkulex.initialize();
    Herkulex.torqueON(BROAD_HKL);

    Serial.println("Setup ended.");
    delay(2000);
    Serial.println("Starting program");
    turnr(2);
    while(1);
}

void loop()
{
    Serial.println(isramp());
    if(isramp()) upramp();
    int victim = check_victim();
    if(victim >= 0) drop(victim);
    if(isblack())
    {
        walkb(1);
        align();
        if(wallr())
        {
            turnr(2);
        }
        else
        {
            turnr(1);
            align();
            walkf(1);
        }
    }
    else
    {
        align();
        if(!walll())
        {
            turnl(1);
            align();
            victim = check_victim();
            if(victim >= 0) drop(victim);
            walkf(1);
        }
        else if (!wallf())
        {
            walkf(1);
        }
        else if (!wallr())
        {
            turnr(1);
            victim = check_victim();
            if(victim >= 0) drop(victim);
            align();
            walkf(1);
        }
        else
        {
            turnr(2);
            align();
            walkf(1);
        }
    }
}
