#ifndef __UTILS_HPP__
#define __UTILS_HPP__

//#define UTILS_NODEBUG
//#define UTILS_NOASSERT
//#define UTILS_NOPROFILE
//#define UTILS_LOGFILE "log.txt

#include <cstdio>
#include <cstring>
#include <string>
#define FONT_PATH "/.gAssets/fonts/"
/*****************************************************************************/
/* Common                                                                    */
/*****************************************************************************/
/// STR(X) turns X into a string literature
/// XSTR(X) turns X into string of expansion of macro X
#define STR(X) #X
#define XSTR(X) STR(X)

/// Supress unused warning
#define UNUSED(X) (void)(X)

/*****************************************************************************/
/* Logging                                                                   */
/*****************************************************************************/
/// General Formatting
#define _FMT_RESET_ "0"
#define _FMT_BRIGHT_ "1"
#define _FMT_DIM_ "2"
#define _FMT_UNDERSCORE_ "3"
#define _FMT_BLINK_ "4"
#define _FMT_REVERSE_ "5"
#define _FMT_HIDDEN_ "6"

/// Foreground Colors
#define _FG_BLACK_ "30"
#define _FG_RED_ "31"
#define _FG_GREEN_ "32"
#define _FG_YELLOW_ "33"
#define _FG_BLUE_ "34"
#define _FG_MAGENTA_ "35"
#define _FG_CYAN_ "36"
#define _FG_WHITE_ "37"

/// Background Colors
#define _BG_BLACK_ "40"
#define _BG_RED_ "41"
#define _BG_GREEN_ "42"
#define _BG_YELLOW_ "43"
#define _BG_BLUE_ "44"
#define _BG_MAGENTA_ "45"
#define _BG_CYAN_ "46"
#define _BG_WHITE_ "47"

/// Common formatters
#ifndef EMSCRIPTEN
#define FAIL_COLOR(X)                                                          \
  "\x1b[" _FMT_BRIGHT_ ";" _FG_RED_ "m" X "\x1b[" _FMT_RESET_ "m"
#define WARN_COLOR(X)                                                          \
  "\x1b[" _FMT_BRIGHT_ ";" _FG_YELLOW_ "m" X "\x1b[" _FMT_RESET_ "m"
#define INFO_COLOR(X)                                                          \
  "\x1b[" _FMT_BRIGHT_ ";" _FG_BLUE_ "m" X "\x1b[" _FMT_RESET_ "m"
#else
#define FAIL_COLOR(X) X
#define WARN_COLOR(X) X
#define INFO_COLOR(X) X
#endif

/// Log file
namespace UTILS {
static inline FILE *_log_file_() {
#ifdef UTILS_LOGFILE
  static FILE *fp = fopen(UTILS_LOGFILE, "a+");
  if (fp) {
    return fp;
  } else {
    return stderr;
  }
#else
#ifndef EMSCRIPTEN
  return stderr;
#else
  return stdout;
#endif
#endif
}
} // namespace UTILS

/// Private logging macros
#define _MSG_(type, log, msg, ...)                                             \
  do {                                                                         \
    fprintf((log), "[" type "] " msg "\n", ##__VA_ARGS__);                     \
    fflush((log));                                                             \
  } while (0)

/// Public logging macros
#define FAIL(msg, ...)                                                         \
  do {                                                                         \
    _MSG_("FAIL", UTILS::_log_file_(), WARN_COLOR(__FILE__ ":%d "), __LINE__); \
    _MSG_("FAIL", UTILS::_log_file_(), FAIL_COLOR(msg), ##__VA_ARGS__);        \
  } while (0)
#define FAILED(msg, ...)                                                       \
  do {                                                                         \
    FAIL(msg, ##__VA_ARGS__);                                                  \
    exit(EXIT_FAILURE);                                                        \
  } while (0)
#define WARN(msg, ...)                                                         \
  do {                                                                         \
    _MSG_("WARN", UTILS::_log_file_(), WARN_COLOR(msg), ##__VA_ARGS__);        \
  } while (0)
#define WARNL(msg, ...)                                                        \
  do {                                                                         \
    _MSG_("WARN", UTILS::_log_file_(), WARN_COLOR("(" __FILE__ ":%d) " msg),   \
          __LINE__, ##__VA_ARGS__);                                            \
  } while (0)
#define INFO(msg, ...)                                                         \
  do {                                                                         \
    _MSG_("INFO", UTILS::_log_file_(), INFO_COLOR(msg), ##__VA_ARGS__);        \
  } while (0)
#ifndef UTILS_NODEBUG
#define DEBUG(msg, ...)                                                        \
  do {                                                                         \
    _MSG_("DEBUG", UTILS::_log_file_(), msg, ##__VA_ARGS__);                   \
  } while (0)
#else
#define DEBUG(msg, ...)
#endif

/*****************************************************************************/
/* Assertion                                                                 */
/*****************************************************************************/
/// Private assert macros
#ifndef UTILS_NOASSERT
#define _ASSERT_(x, msg, ...)                                                  \
  do {                                                                         \
    if (!(x)) {                                                                \
      if (strcmp((msg), "")) {                                                 \
        FAILED(STR(x) " : " msg, ##__VA_ARGS__);                               \
      } else {                                                                 \
        FAILED(STR(x));                                                        \
      }                                                                        \
    }                                                                          \
  } while (0)
#define _EXEC_(x, cb)                                                          \
  do {                                                                         \
    if (!(x)) {                                                                \
      cb();                                                                    \
    }                                                                          \
  } while (0)
#else
#define _ASSERT_(x, msg, ...)                                                  \
  do {                                                                         \
    UNUSED(x);                                                                 \
  } while (0)
#define _EXEC_(x, cb)
#endif

/// Public assert macros
#define ASSERT(x)                                                              \
  do {                                                                         \
    _ASSERT_(x, "");                                                           \
  } while (0)
#define ASSERT_MSG(x, msg, ...)                                                \
  do {                                                                         \
    _ASSERT_(x, FAIL_COLOR(msg), ##__VA_ARGS__);                               \
  } while (0)
#define ASSERT_CB(x, cb, msg, ...)                                             \
  do {                                                                         \
    if (!(x)) {                                                                \
      _EXEC_(1, cb);                                                           \
      _ASSERT_(1, FAIL_COLOR(msg), ##__VA_ARGS__);                             \
    }                                                                          \
  } while (0)

/*****************************************************************************/
/* Profile                                                                   */
/*****************************************************************************/
#ifndef UTILS_NOPROFILE
#include <chrono>
#include <unordered_map>
namespace UTILS {
using Clock = std::chrono::high_resolution_clock;
using TimePoint = std::chrono::time_point<Clock>;
using TimeMap = std::unordered_map<std::string, TimePoint>;
using Precision = std::chrono::milliseconds;

static TimeMap _timer_map_;
static int _padding_counter_{1};

static inline void _timer_start_(const std::string &id) {
  _padding_counter_ += 2;
  _timer_map_[id] = Clock::now();
}

static inline double _timer_stop_(const std::string &id) {
  _padding_counter_ = (_padding_counter_ > 3) ? (_padding_counter_ - 2) : 1;
  TimeMap::iterator it = _timer_map_.find(id);
  if (it == _timer_map_.end()) {
    return -1;
  }
  const TimePoint now = Clock::now();
  return std::chrono::duration_cast<Precision>(now - it->second).count();
}
} // namespace UTILS
#define PROFILE(id, ...)                                                       \
  do {                                                                         \
    _MSG_("PROFILE >>>", UTILS::_log_file_(), "%*s----- %s",                   \
          UTILS::_padding_counter_, "{", id, ##__VA_ARGS__);                   \
    UTILS::_timer_start_(id);                                                  \
  } while (0)
#define PROFILE_END(id, ...)                                                   \
  do {                                                                         \
    const double c = UTILS::_timer_stop_(id);                                  \
    if (c >= 0) {                                                              \
      _MSG_("*TIME COST*", UTILS::_log_file_(), "%*s%.3lfs",                   \
            UTILS::_padding_counter_ - 1, "", c / 1000);                       \
    } else {                                                                   \
      WARN(__FILE__ ":%d NO PROFILE START FOUND", __LINE__);                   \
    }                                                                          \
    _MSG_("PROFILE END", UTILS::_log_file_(), "%*s----} %s",                   \
          UTILS::_padding_counter_, "-", id, ##__VA_ARGS__);                   \
  } while (0)
#else
#define PROFILE(id, ...)
#define PROFILE_END(id, ...)
#endif

#endif //__UTILS_HPP__
