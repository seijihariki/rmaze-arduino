#ifndef KERNEL_H_
#define KERNEL_H_

#include "types.h"

namespace kernel
{
    /* Applies movement, returning successful                   *
     *                                                          *
     * Params:                                                  *
     * mv: movement to apply                                    *
     *                                                          *
     * Ret:                                                     *
     * Returns sucessful movements.                             */
    move apply(move mv);

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
}

#endif
