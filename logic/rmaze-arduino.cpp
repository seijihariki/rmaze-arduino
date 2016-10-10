//#include <>
#include <Herkulex.h>

const int BROAD_HKL = 0xfe;

void setup()
{
    Herkulex.beginSerial1(115200);

    Herkulex.reboot(BROAD_HKL);

    Herkulex.initialize();

    Herkulex.torqueON(BROAD_HKL);

    Herkulex.moveSpeedOne(BROAD_HKL, 1000, 1, 6);
}

void loop()
{
}
