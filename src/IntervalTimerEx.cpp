#include "callbackHelper.h"
#include "IntervalTimerEx.h"

IntervalTimerEx::~IntervalTimerEx()
{
    end();
}

void IntervalTimerEx::end()
{
    if (slot == -1) return;

    callbacks[slot] = nullptr;
    slot            = -1;
    IntervalTimer::end();
}

// generate and preset the callback storage
Callback* IntervalTimerEx::callbacks[4]{
    nullptr,
    nullptr,
    nullptr,
    nullptr,
};

// those functions will be attached to the underlying intervalTimer
relay_t IntervalTimerEx::relays[4]{
    [] { callbacks[0]->invoke(); },
    [] { callbacks[1]->invoke(); },
    [] { callbacks[2]->invoke(); },
    [] { callbacks[3]->invoke(); },
};

