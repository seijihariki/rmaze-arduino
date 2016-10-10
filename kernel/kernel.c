#include "kernel.h"
#include "movement.h"
#include "sensory.h"

const int dst_sth = 400;

move kernel::apply(move mv)
{
    switch(mv.dir)
    {
    case 0:
        return movement::walkf(mv, dst_sth);
        break;
    case 1:
        return movement::turnr(mv, dst_sth);
        break;
    case 2:
        return movement::turnl(mv, dst_sth);
        break;
    default:
        mv.cnt = 0;
        return mv;
        break;
    }
}

void kernel::upramp()
{
    movement::upramp();
}

void kernel::downramp()
{
    movement::downramp();
}

void kernel::align()
{
    movement::align(1000, dst_sth);
}

bool kernel::wallf()
{
    return sensory::wf(dst_sth);
}

bool kernel::wallr()
{
    return sensory::wr(dst_sth);
}

bool kernel::walll()
{
    return sensory::wl(dst_sth);
}
