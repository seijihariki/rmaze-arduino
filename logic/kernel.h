#ifndef KERNEL_H_
#define KERNEL_H_

#include "types.h"
#include "movement.h"

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

#endif
