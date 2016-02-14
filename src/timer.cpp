#include "timer.h"

Timer::Timer()
{
    setInterval(1000);
}

Timer::~Timer()
{

}

void Timer::execute()
{
    this->start();
}
