# cpptimer
This is a very simple c++ timer class, which allows you to create two kinds of timer task to execute task periodically. Besides, it provides several functions about time.<br />
C++17 or higher is required.
## Documentation
### timer
The class `TimerTask` can be used as two kinds of timers. One is to execute task periodically, the other is to execute task periodically on a specific time everyday, such as 13:00:00.
Initialize an object `TimerTask` and then call its member function `run`, which accepts a function with parameters. Now the function starts to be executed periodically. Here is an example,
```
void func(int a) { std::cout << "common func " << a << std::endl; }
class Test {
public:
    void func(int a) { std::cout << "member func " << a << std::endl; }
};

int main() {
    TimerTask<3, std::chrono::seconds> timer_3_sec;
    timer_3_sec.run(&func, 1);
    Test t;
    TimerTask<13, std::chrono::hours, true> timer_13_oclock_everyday; // true makes it become the other kind of timer
    timer_13_oclock_everyday.run(&Test::func, &t, 1);
    std::cin.get();

    return 0;
}
```
Attention:
1. There is neither `join` nor `detach` member function for `TimerTask`, the inner thread is detached after starting the timer.
2. You must consider exception handling on your own.
### time functions
`getCurrentTimePoint(int)`
It returns a time point. The parameter is time zone, which can be negative.
`convertIntToTimePoint`
It converts an integer to a time point.
`getCurrentHour(int)`
It returns the hour of current time. The parameter is time zone, which can be negative.
`durationSinceMidnight(int)`
It returns the duration since the midnight of today. The parameter is time zone, which can be negative.
