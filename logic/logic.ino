//#include "kernel.h"
#include <Adafruit_ADS1015.h>
#include <Herkulex.h>

struct Move
{
    Move(int _dir, int _cnt)
    {
      dir = _dir;
      cnt = _cnt;
    }
    int dir;
    int cnt;
};

const int idfr  = 253;//frente direita
const int idfl  = 16;//frente esquerda
const int idbr  = 10;//tras direita
const int idbl  = 20;//tras esquerda
const int idd   = 50;//dropper
const int idall = -1;//num if para mandar tudo andar

const int timef = 1000;
const int speedfl = 1000;
const int speedfr = 1000;
const int speedbl = 1000;
const int speedbr = 1000;
const int stoptime = 400;

Adafruit_ADS1015 fads(0x49);
Adafruit_ADS1015 rads(0x48);
Adafruit_ADS1015 lads(0x4a);

/* Returns raw distance for each useful sharp           */
int sn_distfr();
int sn_distfl();
  
int sn_distrf();
int sn_distrb();

int sn_distlf();
int sn_distlb();

/* Returns raw average distance of sharps to that side  */ 
float sn_distf();
float sn_distr();
float sn_distl();

/* Returns if has wall to that side.                    *
 *                                                      *
 * Params:                                              *
 * dst_sth: distance threshold for wall                 */
bool sn_wf(int dst_sth);
bool sn_wr(int dst_sth);
bool sn_wl(int dst_sth);

/* Returns if tile is black                             *
 *                                                      *
 * Params:                                              *
 * col_sth: color threshold for detecting black         */
bool sn_isblack(int col_sth);

/* Returns if should align                              *
 *                                                      *
 * Params:                                              *
 * lin_sth: error threshold for aligning linearly       *
 * rot_sth: error threshold for aligning rotationally   */
bool sn_shouldAlign(float lin_sth, float rot_sth);

/* Returns if on ramp                                   *
 *                                                      *
 * Params:                                              *
 * max_angle: max angulation to consider not on ramp    */
bool sn_onRamp(float max_angle);

int sn_distfr()
{
	return fads.readADC_SingleEnded(0);
}

int sn_distfl()
{
	return fads.readADC_SingleEnded(1);
}

int sn_distrf()
{
	return fadr.readADC_SingleEnded(2);
}

int sn_distrb()
{
	return fadr.readADC_SingleEnded(0);
}

int sn_distlf()
{
	return fadr.readADC_SingleEnded(1);
}

int sn_distlb()
{
	return fadr.readADC_SingleEnded(0);
}

float sn_distf()
{
	return (distfr() + distfl())/2;
}

float sn_distr()
{
	return (distrf() + distrb())/2;
}

float sn_distl()
{
	return (distlt() + distlb())/2;
}


bool sn_isblack(int col_sth)
{
	int val_read = rfads.readADC_SingleEnded(3);
	return val_read > col_sth;
}

bool sn_wf(int dst_sth)
{
	return sn_distf() > dst_sth;
}

bool sn_wr(int dst_sth)
{
	return sn_distr() > dst_sth;
}

bool sn_wl(int dst_sth)
{
	return sn_distl() > dst_sth;
}

bool sn_shouldAlign(float lin_sth, float rot_sth)
{
	return false;
}

/* Aligns robot. Stops after tout (timeout) or if       *
 * shouldn't align. (Using function from sensory)       *
 *                                                      *
 * Params:                                              *
 * tout: timeout (ms)                                   *
 * dst_sth: distance threshold (raw number)             *
 *                                                      */
void mv_align(int tout, int dst_sth);

/* Stops all motors.*/
void mv_stop();                                 

/* Applies walk front movement, returning sucessful.    *
 * Must check MLX while walking.                        *
 * Params:                                              *
 * mv: movement                                         *
 * dst_sth: distance threshold (raw number)             *
 *                                                      *
 * Ret:                                                 *
 * Sucessful moves.                                     */
Move mv_walkf(Move mv, int dst_sth);

/* Applies walk back movement, returning sucessful.     *
 * Params:                                              *
 * mv: movement                                         *
 * dst_sth: distance threshold (raw number)             *
 *                                                      *
 * Ret:                                                 *
 * Sucessful moves.                                     */
Move mv_walkb(Move mv, int dst_sth);

/* Applies turn right movement, returning sucessful.    *
 *                                                      *
 * Params:                                              *
 * mv: movement                                         *
 * dst_sth: distance threshold (raw number)             *
 *                                                      *
 * Ret:                                                 *
 * Sucessful moves.                                     */
Move mv_turnl(Move mv, int dst_sth);

/* Applies turn left movement, returning sucessful.     *
 *                                                      *
 * Params:                                              *
 * mv: movement                                         *
 * dst_sth: distance threshold (raw number)             *
 *                                                      *
 * Ret:                                                 *
 * Sucessful moves.                                     */
Move mv_turnr(Move mv, int dst_sth);

/* Goes up ramp.                                        */
void mv_upramp();

/* Goes down ramp.                                      */
void mv_downramp();

/* Drop a kit.                                           */
void mv_drop(int side);

Move mv_walkf(Move mv, int dst_sth){
  Move atual(mv.dir, 0);
    for(; atual.cnt < mv.cnt; atual.cnt++){
        if(sn_wf(dst_sth))break;
        Herkulex.moveOne(idfl, speedfl, 1, 2);
        Herkulex.moveOne(idfr, speedfr, 1, 2);
        Herkulex.moveOne(idbl, speedbl, 1, 2);
        Herkulex.moveOne(idbr, speedbr, 1, 2);
        delay(timef);
    }
    mv.cnt = atual.cnt - mv.cnt;
    Herkulex.moveOne(idfl, 0, 1, 2);
    Herkulex.moveOne(idfr, 0, 1, 2);
    Herkulex.moveOne(idbl, 0, 1, 2);
    Herkulex.moveOne(idbr, 0, 1, 2);
    delay(stoptime);
    return atual;
}

Move mv_turnl(Move mv, int dst_sth){
    for(mv.cnt; mv.cnt > 0; mv.cnt--){
        Herkulex.moveOne(idfl, speedfl,  1, 2);
        Herkulex.moveOne(idfr, -speedfr, 1, 2);
        Herkulex.moveOne(idbl, speedbl,  1, 2);
        Herkulex.moveOne(idbr, -speedbr, 1, 2);
        delay(timef);
    }
}

Move mv_turnr(Move mv, int dst_sth){
    for(mv.cnt; mv.cnt > 0; mv.cnt--){
        Herkulex.moveOne(idfl, speedfl,  1, 2);
        Herkulex.moveOne(idfr, -speedfr, 1, 2);
        Herkulex.moveOne(idbl, speedbl,  1, 2);
        Herkulex.moveOne(idbr, -speedbr, 1, 2);
        delay(timef);
    }
}

const int dst_sth = 400;
const int col_sth = 800;
const int limangle = 10;

/* Applies movement, returning successful                   *
 *                                                          *
 * Params:                                                  *
 * mv: movement to apply                                    *
 *                                                          *
 * Ret:                                                     *
 * Returns sucessful movements.                             */
Move apply(Move mv);

/* Goes up the ramp. Just calls movement.h implementation   */
void upramp();

/* Goes down the ramp. Just calls movement.h implementation */
void downramp();

/* Aligns. Just calls movement.h implementaion              */
void align();

/* Returns if has wall to sides. Just calls sensory.h imp.  */
bool wallf();
bool wallr();
bool walll();

/* Returns if is black                                      */
bool isblack();

/* Returns if on ramp                                       */
bool isramp();

Move apply(Move mv)
{
    switch(mv.dir)
    {
    case 0:
        return mv_walkf(mv, dst_sth);
        break;
    case 1:
        return mv_turnr(mv, dst_sth);
        break;
    case 2:
        return mv_turnl(mv, dst_sth);
        break;
    default:
        mv.cnt = 0;
        return mv;
        break;
    }
}

void upramp()
{
    mv_upramp();
}

void downramp()
{
    mv_downramp();
}

void align()
{
    mv_align(1000, dst_sth);
}

bool wallf()
{
    return sn_wf(dst_sth);
}

bool wallr()
{
    return sn_wr(dst_sth);
}

bool walll()
{
    return sn_wl(dst_sth);
}

bool isblack()
{
    return sn_isblack(col_sth);
}

bool isramp()
{
    return sn_onRamp(limangle);
}

void setup()
{
	delay(2000);  //a delay to have time for serial monitor opening
    Serial.begin(115200);    // Open serial communications
    Serial.println("Begin");
    Herkulex.beginSerial1(115200); //open serial port 1 
  
    Herkulex.reboot(idfr); 
    Herkulex.reboot(idfl);
    Herkulex.reboot(idbr);
    Herkulex.reboot(idbl);
    Herkulex.reboot(idd);

    delay(500); 
    Herkulex.initialize(); //initialize motors
}

void loop()
{
    if(isblack())
    {
        apply(Move(2, 1));
        if(wallr())
        {
            apply(Move(1, 2));
        }
        else
        {
            apply(Move(1, 1));
            apply(Move(0, 1));
        }
    }
    else
    {
        if(!walll())
        {
            apply(Move(3, 1));
            apply(Move(0, 1));
        }
        else if(!wallf())
        {
            apply(Move(0, 1));
        }
        else if(!wallr())
        {
            apply(Move(1, 1));
            apply(Move(0, 1));
        }
        else
        {
            apply(Move(1, 2));
            apply(Move(0, 1));
        }
    }
}
