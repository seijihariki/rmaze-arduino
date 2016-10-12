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
const int walk_t  = 1600;

const int turnpot = 1000;
const int turn_t  = 1000;

const int walldist = 500;
// Fixed

//Adafruit_MLX90614 fmlx = Adafruit_MLX90614();
Adafruit_MLX90614 rmlx = Adafruit_MLX90614(0x5a);
Adafruit_MLX90614 lmlx = Adafruit_MLX90614(0x59);
ADXL345 gy80 = ADXL345();

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
            int errorf = ((analogRead(FL_SHARP) + analogRead(FR_SHARP))/2) - walldist;
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

            if(abs(errorl) < 50 && abs(errorf) < 50 && abs(errorr) < 50) break;

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
    return i;
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
    return i;
}

int turnr(int cnt)
{
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
    return i;
}

int turnl(int cnt)
{
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
    return i;
}

void setup()
{
    Serial.begin(9600);

    Serial.println("Starting setup...");

    Serial.println("Initializing MLX and GY80...");

    //fmlx.begin();
    rmlx.begin();
    lmlx.begin();
    gy80.begin();

    Serial.println("Initializing  motors...");

    Herkulex.beginSerial1(115200);
    Herkulex.reboot(BROAD_HKL);
    delay(500);
    Herkulex.initialize();
    Herkulex.torqueON(BROAD_HKL);

    Serial.println("Setup ended.");
    delay(2000);
}

const int HOR = 0x01;
const int VER = 0x02;
const int COL = 0x04;
const int VIS = 0x08;

const int mapsz = 10;
int x[2], y[2], d[2];
int Map[mapsz*2 + 2][mapsz*2 + 2][3];
int f = 0;

int &curr_tile(int ex = 0, int ey = 0, int floor = -1)
{
    return Map[mapsz + x[f] + ex][mapsz + y[f] + ey][floor < 0?f:floor]; 
}

bool m_l_to_g(int loc)
{
    return (loc + d[f])%4;
}

bool m_visited(int tileto = -1)
{
    if(tileto == -1) return curr_tile() & VIS;
    switch(m_l_to_g(tileto))
    {
    case 0:
        return curr_tile(0, -1) & VIS; 
        break;
    case 1:
        return curr_tile(1, 0) & VIS; 
        break;
    case 2:
        return curr_tile(0, 1) & VIS;
        break;
    case 3:
        return curr_tile(-1, 0) & VIS;
        break;
    default:
        return false;
        break;
    }
}

bool m_wallf()
{
    switch(m_l_to_g(0))
    {
    case 0:
        return curr_tile(0, 0) & HOR;
        break;
    case 1:
        return curr_tile(1, 0) & VER;
        break;
    case 2:
        return curr_tile(0, 1) & HOR;
        break;
    case 3:
        return curr_tile(0, 0) & VER;
        break;
    default:
        return true;
        break;
    }
}

bool m_walll()
{
    switch(m_l_to_g(3))
    {
    case 0:
        return curr_tile(0, 0) & HOR;
        break;
    case 1:
        return curr_tile(1, 0) & VER;
        break;
    case 2:
        return curr_tile(0, 1) & HOR;
        break;
    case 3:
        return curr_tile(0, 0) & VER;
        break;
    default:
        return true;
        break;
    }
}

bool m_wallr()
{
    switch(m_l_to_g(1))
    {
    case 0:
        return curr_tile(0, 0) & HOR;
        break;
    case 1:
        return curr_tile(1, 0) & VER;
        break;
    case 2:
        return curr_tile(0, 1) & HOR;
        break;
    case 3:
        return curr_tile(0, 0) & VER;
        break;
    default:
        return true;
        break;
    }
}

bool m_wallb()
{
    switch(m_l_to_g(2))
    {
    case 0:
        return curr_tile(0, 0) & HOR;
        break;
    case 1:
        return curr_tile(1, 0) & VER;
        break;
    case 2:
        return curr_tile(0, 1) & HOR;
        break;
    case 3:
        return curr_tile(0, 0) & VER;
        break;
    default:
        return true;
        break;
    }
}

bool m_isblack()
{
    return curr_tile() & COL;
}

void m_walkf(int cnt)
{
    switch(d[f])
    {
    case 0:
        y[f] -= cnt;
        break;
    case 1:
        x[f] += cnt;
        break;
    case 2:
        y[f] += cnt;
        break;
    case 3:
        x[f] -= cnt;
        break;
    default:
        break;
    }
}

void m_walkb(int cnt)
{
    cnt = -cnt;
    switch(d[f])
    {
    case 0:
        y[f] -= cnt;
        break;
    case 1:
        x[f] += cnt;
        break;
    case 2:
        y[f] += cnt;
        break;
    case 3:
        x[f] -= cnt;
        break;
    default:
        break;
    }
}

void m_turnr(int cnt)
{
    d[f] += cnt;
    d[f] %= 4;
}

void m_turnl(int cnt)
{
    d[f] -= cnt + 4*cnt;
    d[f] %= 4;
}

int PF(bool first)
{
    bool found = false;
    int tx = 0, ty = 0;
    int i = 0;

    for(int ix = 0; ix < mapsz*2 + 2; ix++)
    {
        for(int iy = 0; iy < mapsz*2 + 2; iy++)
        {
            Map[ix][iy][2] = 0;
        }
    }

    bool done = true;

    while(!found && done)
    {
        i++;
        done = false;
        for(int ix = 0; ix < mapsz*2 + 2; ix++)
        {
            for(int iy = 0; iy < mapsz*2 + 2; iy++)
            {
                if(Map[ix][iy][2] == i)
                {
                    if((first && !(Map[ix][iy][f] & VIS))
                            || (!first && ix == mapsz && iy == mapsz))
                    {
                        tx = ix;
                        ty = iy;
                        found = true;
                    }

                    if(ix > 0 && !Map[ix - 1][iy][2]
                            && (first || Map[ix - 1][iy][f] & VIS)
                            && !(Map[ix][iy][f] & VER))
                    {
                        Map[ix - 1][iy][2] = i + 1;
                        done = true;
                    }

                    if(ix < mapsz*2 + 1 && !Map[ix + 1][iy][2]
                            && (first || Map[ix + 1][iy][f] & VIS)
                            && !(Map[ix + 1][iy][f] & VER))
                    {
                        Map[ix + 1][iy][2] = i + 1;
                        done = true;
                    }

                    if(iy > 0 && !floor[ix][iy - 1][2]
                            && (first || Map[ix][iy - 1][f] & VIS)
                            && !(Map[ix][iy][f] & HOR))
                    {
                        Map[ix][iy - 1][2] = i + 1;
                        done = true;
                    }

                    if(iy < mapsz*2 + 1 && !Map[ix][iy + 1][2]
                            && (first || Map[ix][iy + 1][f] & VIS)
                            && !(Map[ix][iy + 1][f] & HOR))
                    {
                        Map[ix][iy + 1][2] = i + 1;
                        done = true;
                    }
                }
            }
        }
    }

    for(; i > 2;)
    {
        i--;
        if(tx > 0 && Map[tx - 1][ty][2] == i
                && !(Map[tx][ty][f] & VER))
            tx--;
        if(tx < mapsz*2 + 1 && Map[tx + 1][ty][2] == i
                && !(Map[tx + 1][ty][f] & VER))
            tx++;
        if(ty > 0 && Map[tx][ty - 1][2] == i
                && !(Map[tx][ty][f] & HOR))
            ty--;
        if(ty < mapsz*2 + 1 && Map[tx][ty + 1][2] == i
                && !(Map[tx][ty + 1][f] & HOR))
            ty++;
    }

    if(tx > 0 && Map[tx - 1][ty][2] == 1)
        return 3;
    if(tx < mapsz*2 + 1 && Map[tx + 1][ty][2] == 1)
        return 1;
    if(ty > 0 && Map[tx][ty - 1][2] == 1)
        return 0;
    if(ty < mapsz*2 + 1 && Map[tx][ty + 1][2] == 1)
        return 2;
    return -1;
}

void scan_tile()
{
    curr_tile() |= VIS;
    if(isblack()) curr_tile() |= COL;
    else curr_tile() &= ~COL;

    switch(d[f])
    {
    case 0:
        if(wallf()) curr_tile(0, 0) |= HOR;
        else curr_tile(0, 0) &= ~HOR;
        
        if(wallr()) curr_tile(1, 0) |= VER;
        else curr_tile(1, 0) &= ~VER;
        
        if(walll()) curr_tile(0, 0) |= VER;
        else curr_tile(0, 0) &= ~VER;

        break;
    case 1:
        if(wallf()) curr_tile(1, 0) |= VER;
        else curr_tile(1, 0) &= ~VER;
        
        if(wallr()) curr_tile(0, 1) |= HOR;
        else curr_tile(0, 1) &= ~HOR;
        
        if(walll()) curr_tile(0, 0) |= HOR;
        else curr_tile(0, 0) &= ~HOR;

        break;
    case 2:
        if(wallf()) curr_tile(0, 1) |= HOR;
        else curr_tile(0, 1) &= ~HOR;
        
        if(wallr()) curr_tile(1, 0) |= VER;
        else curr_tile(1, 0) &= ~VER;
        
        if(walll()) curr_tile(0, 0) |= VER;
        else curr_tile(0, 0) &= ~VER;

        break;
    case 3:
        if(wallf()) curr_tile(0, 0) |= VER;
        else curr_tile() &= ~VER;
        
        if(wallr()) curr_tile(0, 0) |= HOR;
        else curr_tile() &= ~HOR;
        
        if(walll()) curr_tile(0, 1) |= HOR;
        else curr_tile() &= ~HOR;

        break;
    default:
        break;
    }
}

int best_move()
{
    if(m_isblack()) return 2; // Pullback
    
    // Immediate
    bool wf = m_wallf();
    bool wfv = m_visited(0);
    bool wl = m_walll();
    bool wlv = m_visited(3);
    bool wr = m_wallr();
    bool wrv = m_visited(1);
    bool wb = m_wallb();
    bool wbv = m_visited(2);

    if(!wf && !wfv) return 0;
    if(!wl && !wlv) return 3;
    if(!wr && !wrv) return 1;
    if(!wb && !wbv) return 1;

    // PathFinding
    int res = PF(1);
    if(res < 0) return PF(0);
    return res;
}

void loop()
{
}
