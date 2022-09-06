#pragma once

#include "Arduino.h"
#include "CallbackHelper.h"

class IntervalTimerEx : public IntervalTimer
{
 public:
    template <typename T, typename period_t> // period_t is implemented as template to avoid replication the various versions of IntervalTimer::begin
    bool begin(T&& callback, period_t period);

    void end();
    ~IntervalTimerEx();

 protected:
    // type of relays to be attached to the stock IntervalTimer
    using relay_t = void (*)();

    // setup the callbackHelper
    using callbackHelper_t = CallbackHelper<void(void), 4>; // 4 slots for void(void) callbacks
    using callback_t       = callbackHelper_t::callback_t;
    callbackHelper_t cbh;

    int slot = -1;

    // Storage for callbacks and relays
    static callback_t* callbacks[4]; // storage for callbacks
    static relay_t relays[4];        // storage for relay functions
};

// Inline implementation ===============================================

template <typename T, typename period_t>
bool IntervalTimerEx::begin(T&& lambda, period_t period)
{
    if (slot != -1) end();                                                         // if slot is used, end corresponding timer channel and remove old callback
    for (unsigned index = 0; index < 4; index++)                                   // find the next free slot
    {                                                                              //
        if (callbacks[index] == nullptr)                                           // got a free slot
        {                                                                          //
            if (IntervalTimer::begin(relays[index], period))                       // now try to get a free timer channel (channels might be used by the stock IntervalTimer)
            {
                slot            = index;                                           // store the slot we put the callback in
                callbacks[slot] = cbh.makeCallback(std::forward<T>(lambda), slot); // generate callback and store in the free slot
                return true;
            }
        }
    }
    return false;
}
