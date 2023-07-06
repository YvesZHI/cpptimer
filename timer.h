#ifndef TIMER_H_
#define TIMER_H_

#include <chrono>
#include <thread>

using namespace std::literals::chrono_literals;

/**
 * std::chrono tools.
 */
// time point type
template <typename T = std::chrono::milliseconds>
using Clock = std::chrono::time_point<std::chrono::system_clock, T>;
using TimePointMs = Clock<>;
using TimePointSec = Clock<std::chrono::seconds>;
using TimePointHour = Clock<std::chrono::hours>;

// get current time point
// param timeZone can be negative
template <typename T = std::chrono::milliseconds>
inline Clock<T> getCurrentTimePoint(int8_t timeZone = 0) {
  return std::chrono::time_point_cast<T>(std::chrono::system_clock::now()) +
         std::chrono::hours{timeZone};
}

template <typename T = std::chrono::milliseconds>
inline Clock<T> convertIntToTimePoint(typename T::rep t) {
  return Clock<T>{T{t}};
}

inline int getCurrentHour(int8_t timeZone = 0) {
  const std::time_t time_point =
      std::chrono::system_clock::to_time_t(getCurrentTimePoint(timeZone));
  return std::localtime(&time_point)->tm_hour;
}

inline std::chrono::system_clock::duration durationSinceMidnight(int8_t timeZone = 0) {
  auto now = std::chrono::system_clock::now() + std::chrono::hours{timeZone};
  std::time_t tnow = std::chrono::system_clock::to_time_t(now);
  std::tm* date = std::localtime(&tnow);
  date->tm_hour = 0;
  date->tm_min = 0;
  date->tm_sec = 0;
  auto midnight = std::chrono::system_clock::from_time_t(std::mktime(date));
  return now - midnight;
}

/**
 * @tparam IS_DAILY
 * - false: a common timer;
 * - true: a timer whose task is executed at specific time everyday.
 * @tparam VAL - if IS_DAILY is true, VAL means the time of a day, such as 13 for 13:00:00.
 */
template <int VAL, typename D = std::chrono::seconds, bool IS_DAILY = false> class TimerTask {
public:
  // common function
  template <typename F, typename... Args> void run(F *f, Args&&... args) {
    task(std::integral_constant<bool, IS_DAILY>{}, f, std::forward<Args>(args)...);
  }

  // member function
  template <typename C, typename F, typename... Args> void run(F C::*f, C* c, Args&&... args) {
    task(std::integral_constant<bool, IS_DAILY>{}, f, c, std::forward<Args>(args)...);
  }

private:
  std::thread td_;

  // common funciton
  template <typename F, typename... Args>
  void task(std::true_type, F *f, Args&&... args) {
    td_ = std::thread([f = f, args_lambda = std::make_tuple(std::forward<Args>(args)...)]() {
      auto interval = std::chrono::duration_cast<D>(durationSinceMidnight(8)) - D{VAL};
      if (interval > D{0}) {
        std::this_thread::sleep_for(std::chrono::duration_cast<D>(24h) - interval);
      } else if (interval < D{0}) {
        std::this_thread::sleep_for(std::chrono::duration_cast<D>(-interval));
      }
      do {
        std::apply(f, args_lambda);
        std::this_thread::sleep_for(24h);
      } while (true);
    });
    td_.detach();
  }

  template <typename F, typename... Args>
  void task(std::false_type, F *f, Args&&... args) {
    td_ = std::thread([f = f, args_lambda = std::make_tuple(std::forward<Args>(args)...)]() {
      do {
        std::apply(f, args_lambda);
        std::this_thread::sleep_for(D{VAL});
      } while (true);
    });
    td_.detach();
  }

  // member funciton
  template <typename C, typename F, typename... Args>
  void task(std::true_type, F C::*f, C* c, Args&&... args) {
    td_ = std::thread([f = f, args_lambda = std::make_tuple(c, std::forward<Args>(args)...)]() {
      auto interval = std::chrono::duration_cast<D>(durationSinceMidnight(8)) - D{VAL};
      if (interval > D{0}) {
        std::this_thread::sleep_for(std::chrono::duration_cast<D>(24h) - interval);
      } else if (interval < D{0}) {
        std::this_thread::sleep_for(std::chrono::duration_cast<D>(-interval));
      }
      do {
        std::apply(f, args_lambda);
        std::this_thread::sleep_for(24h);
      } while (true);
    });
    td_.detach();
  }

  template <typename C, typename F, typename... Args>
  void task(std::false_type, F C::*f, C* c, Args&&... args) {
    td_ = std::thread([f = f, args_lambda = std::make_tuple(c, std::forward<Args>(args)...)]() {
      do {
        std::apply(f, args_lambda);
        std::this_thread::sleep_for(D{VAL});
      } while (true);
    });
    td_.detach();
  }
};

#endif
