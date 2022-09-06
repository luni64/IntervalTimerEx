# IntervalTimerEx

IntervalTimerEx is a lightweight extension of the stock IntervalTimer.
It extends the standard Teensy IntervalTimer to accept more or less anything which can be called as callback. E.g. free functions, static member functions, lambda expressions and functors. IntervalTimerEx uses the underlying Teensyduino mechanisms and bookkeeping and can be mixed with the standard IntervalTimer.

Installation through the Arduino Library Manager or the PIO registry.

### Example:
```c++
#include "Arduino.h"
#include "IntervalTimerEx.h"

// Free function callback ------------------------------------------------------
void freeFunction()
{
    Serial.println("Free function callback");
}

// Using a non static member function as callback -------------------------------
class Test
{
 public:
    Test(int _i){
        i = _i;
    }

    void someMemberFunction(float f){
        Serial.printf("non static member function i=%f\n", i * f);
    }

    int i;
};
Test test(42);

//------------------------------------------------------
class MyFunctor_t
{
 public:
    void set(int _i){
        i = _i;
    }

    void operator()() const {
        Serial.printf("Functor i=%d\n", i);
    }

 protected:
    int i = 2;
};
MyFunctor_t functor;

//------------------------------------------------------

IntervalTimerEx t1, t2, t3, t4;

void setup()
{
    // generate various callbacks
    t1.begin(freeFunction, 250'000);                                    // free function
    t2.begin(functor, 250'000);                                         // functor
    t3.begin([] { Serial.printf("non capturing lambda\n"); }, 250'000); // simple, non capturing lambda
    t4.begin([] { test.someMemberFunction(3.1415); }, 250'000);         // non static member function
}

elapsedMillis stopwatch = 0;
void loop()
{
    // change state of functor and test class on the fly
    if (stopwatch > 1000)
    {
        functor.set(random(0, 1000));
        test.i    = random(0, 10000);
        stopwatch = 0;
    }
}
```