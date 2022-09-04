#pragma once

#include "Arduino.h"
#include "callbackHelper.h"

class IntervalTimerEx : public IntervalTimer
{
 public:
    template <typename period_t>
    bool begin(void (*fptr)(), period_t period); // pointer to free function or static member function

    template <typename T, typename period_t> // begin is implemented as template to avoid replication the various versions of IntervalTimer::begin
    bool begin(const T& callback, period_t period);

    void end();
    ~IntervalTimerEx();

 protected:
    using relay_t = void (*)();
    template <typename period_t>
    int doBegin(period_t period);

    CallbackHelper<4> cbh;           // helps generating callbacks from various user input types (free functions, lambdas, functors...)
    static callback_t* callbacks[4]; // storage for callbacks
    static relay_t relays[4];        // storage for relay functions
    int slot = -1;
};

// Inline implementation ===============================================

template <typename lambda_t, typename period_t>
bool IntervalTimerEx::begin(const lambda_t& lambda, period_t period)
{
    slot = doBegin(period);                                 // tries to get a free callback slot to store the callback and tries to start the timer
    if (slot >= 0) callbacks[slot] = cbh.makeCallback(lambda, slot); // slot = -1 if doBegin failed (no slot or no timer)
    return slot >= 0;
}

template <typename period_t>
bool IntervalTimerEx::begin(void (*fptr)(), period_t period)
{
    slot = doBegin(period);                               // tries to get a free callback slot to store the callback and tries to start the timer
    if (slot >= 0) callbacks[slot] = cbh.makeCallback(fptr, slot); // slot = -1 if doBegin failed (no slot or no timer)
    return slot >= 0;
}

template <typename period_t>
int IntervalTimerEx::doBegin(period_t period)
{
    if (slot != -1) end();                                   // if slot is used, end corresponging timer channel and remove old callback first
    for (unsigned index = 0; index < 4; index++)             // find the next free slot
    {                                                        //
        if (callbacks[index] == nullptr)                     // got a free slot
        {                                                    //
            if (IntervalTimer::begin(relays[index], period)) // we got a slot, now try to get a free timer channel (channels might be used by the stock IntervalTimer)
            {                                                //
                return index;                                // slot=-1 if we didn't get a timer
            }
        }
    }
    return -1;
}
