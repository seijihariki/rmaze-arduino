#ifndef MOVEMENT_H_
#define MOVEMENT_H_

#include <Herkulex.h>
#include "types.h"
#include "sensory.h"

const int idfr  = 253;//frente direita
const int idfl  = 16;//frente esquerda
const int idbr  = 10;//tras direita
const int idbl  = 20;//tras esquerda
const int idd   = 50;//dropper
const int idall = -1;//num if para mandar tudo andar

const int timef = 1000;
const int distf = 5;
const int speedfl = 1000;
const int speedfr = 1000;
const int speedbl = 1000;
const int speedbr = 1000;
const int stoptime = 400;

/* Aligns robot. Stops after tout (timeout) or if		*
 * shouldn't align. (Using function from sensory)		*
 *														*
 * Params:												*
 * tout: timeout (ms)									*
 * dst_sth: distance threshold (raw number)				*
 *														*/
void mv_align(int tout, int dst_sth);

/* Stops all motors.*/
void mv_stop();									

/* Applies walk front movement, returning sucessful.	*
 * Must check MLX while walking.						*
 * Params:												*
 * mv: movement											*
 * dst_sth: distance threshold (raw number)				*
 *														*
 * Ret:													*
 * Sucessful moves.										*/
Move mv_walkf(Move mv, int dst_sth);

/* Applies walk back movement, returning sucessful.		*
 * Params:												*
 * mv: movement											*
 * dst_sth: distance threshold (raw number)				*
 *														*
 * Ret:													*
 * Sucessful moves.										*/
Move mv_walkb(Move mv, int dst_sth);

/* Applies turn right movement, returning sucessful.	*
 *														*
 * Params:												*
 * mv: movement											*
 * dst_sth: distance threshold (raw number)				*
 *														*
 * Ret:													*
 * Sucessful moves.										*/
Move mv_turnl(Move mv, int dst_sth);

/* Applies turn left movement, returning sucessful.		*
 *														*
 * Params:												*
 * mv: movement											*
 * dst_sth: distance threshold (raw number)				*
 *														*
 * Ret:													*
 * Sucessful moves.										*/
Move mv_turnr(Move mv, int dst_sth);

/* Goes up ramp.										*/
void mv_upramp();

/* Goes down ramp.										*/
void mv_downramp();

/* Drop a kit.											 */
void mv_drop(int side);

Move mv_walkf(Move mv, int dst_sth){
  Move atual(mv.dir, 0);
	for(; atual.cnt < mv.cnt; atual.cnt++){
		if(sn_wf(distf))break;
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
#endif
