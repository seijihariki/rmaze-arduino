#include "kernel/kernel.h"

void setup()
{

}

void loop()
{
    if(kernel::isblack())
    {
        kernel::apply(move(2, 1));
        if(kernel::wallr())
        {
            kernel::apply(move(1, 2));
        }
        else
        {
            kernel::apply(move(1, 1));
            kernel::apply(move(0, 1));
        }
    }
    else
    {
        if(!kernel::walll())
        {
            kernel::apply(move(3, 1));
            kernel::apply(move(0, 1));
        }
        else if(!kernel::wallf())
        {
            kernel::apply(move(0, 1));
        }
        else if(!kernel::wallr())
        {
            kernel::apply(move(1, 1));
            kernel::apply(move(0, 1));
        }
        else
        {
            kernel::apply(move(1, 2));
            kernel::apply(move(0, 1));
        }
    }
}
