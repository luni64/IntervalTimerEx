#pragma once

#include "Arduino.h"
#include "callbackHelper.h"

using relay_t = void (*)();

class IntervalTimerEx : public IntervalTimer
{
 public:
    template <typename period_t>
    bool begin(void (*fptr)(), period_t period);  // pointer to free function or static member function

    template <typename T, typename period_t> // begin is implemented as template to avoid replication the various versions of IntervalTimer::begin
    bool begin(T callback, period_t period);

    void end();
    ~IntervalTimerEx();

 protected:
    template <typename period_t>
    bool doBegin(Callback* callback, period_t period);

    CallbackHelper<4> cbh;         // helps generating callbacks from various user input types (free functions, lambdas, functors...)
    static Callback* callbacks[4]; // storage for callbacks
    static relay_t relays[4];      // storage for relay functions
    int slot = -1;
};

// Inline implementation ===============================================

template <typename lambda_t, typename period_t>
bool IntervalTimerEx::begin(lambda_t lambda, period_t period)
{
    Callback* callback = cbh.makeCallback(lambda, slot);
    return doBegin(callback, period);
}

template <typename period_t>
bool IntervalTimerEx::begin(void (*fptr)(), period_t period)
{
    Callback* callback = cbh.makeCallback(fptr, slot);
    return doBegin(period);
}

template <typename period_t>
bool IntervalTimerEx::doBegin(Callback* callback, period_t period)
{
    if (slot != -1) end(); // timer is already set up, remove old callback first

    for (unsigned index = 0; index < 4; index++) // find the next free slot
    {
        if (callbacks[index] == nullptr) // ->free slot
        {
            if (IntervalTimer::begin(relays[index], period)) // we got a slot, now try to get a free timer channel (channels might be used by the stock IntervalTimer)
            {
                callbacks[slot] = callback;
                slot            = index;
                return true;
            }
        }
    }
    return false;
}
