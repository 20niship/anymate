#pragma once
#include <spdlog/spdlog.h>

#include <chrono>
#include <sstream>
#ifdef MYLOGGER_OUTPUT_FILENAME
#include <fstream>
#else
#include <iostream>
#endif

#if defined(__Clang__) || defined(__GNUC__)
#define MYLOGGER_GET_FUNCTION __PRETTY_FUNCTION__
#else
#define MYLOGGER_GET_FUNCTION __func__
#endif


/* #define LOGI ::myLogger::log_intermediate::make_info(__FILE__, __LINE__, MYLOGGER_GET_FUNCTION) */
/* #define LOGD ::myLogger::log_intermediate::make_debug(__FILE__, __LINE__, MYLOGGER_GET_FUNCTION) */
/* #define LOGW ::myLogger::log_intermediate::make_warn(__FILE__, __LINE__, MYLOGGER_GET_FUNCTION) */
/* #define LOGE ::myLogger::log_intermediate::make_error(__FILE__, __LINE__, MYLOGGER_GET_FUNCTION) */

#define LOGI ::myLogger::log_intermediate::make_info(__FILE__, __LINE__, "")
#define LOGD ::myLogger::log_intermediate::make_debug(__FILE__, __LINE__, "")
#define LOGW ::myLogger::log_intermediate::make_warn(__FILE__, __LINE__, "")
#define LOGE ::myLogger::log_intermediate::make_error(__FILE__, __LINE__, "")

namespace myLogger {

struct log_intermediate {
  std::stringstream buffer;

  const char* prefix;
  const char* suffix;
  const char* source;
  const size_t line;
  const char* function;
  const char* type;

#ifdef MYLOGGER_OUTPUT_FILENAME
  std::ofstream myFile;
#endif

#ifdef MYLOGGER_OUTPUT_FILENAME
  static constexpr auto prefix_i = "";
  static constexpr auto prefix_d = "";
  static constexpr auto prefix_w = "";
  static constexpr auto prefix_e = "";

  static constexpr auto suffix_i = "";
  static constexpr auto suffix_d = "";
  static constexpr auto suffix_w = "";
  static constexpr auto suffix_e = "";
#else
  static constexpr auto prefix_i = "";
  static constexpr auto prefix_d = "\x1b[32m";
  static constexpr auto prefix_w = "\x1b[33m";
  static constexpr auto prefix_e = "\x1b[31m";

  static constexpr auto suffix_i = "";
  static constexpr auto suffix_d = "\x1b[0m";
  static constexpr auto suffix_w = "\x1b[0m";
  static constexpr auto suffix_e = "\x1b[0m";
#endif

  static constexpr auto type_i = "[INF]";
  static constexpr auto type_d = "[DBG]";
  static constexpr auto type_w = "[WAR]";
  static constexpr auto type_e = "[ERR]";


  log_intermediate(log_intermediate&& a) : buffer(std::move(a.buffer)), prefix(a.prefix), suffix(a.suffix), source(a.source), line(a.line), function(a.function), type(a.type) {}

  log_intermediate(const char* p, const char* s, const char* t, const char* source_, const size_t line_, const char* function_)
    : prefix(p),
      suffix(s),
      source(source_),
      line(line_),
      function(function_),
      type(t)
#ifdef MYLOGGER_OUTPUT_FILENAME
  {
    if(!myFile.is_open()) {
      myFile = std::ofstream(MYLOGGER_OUTPUT_FILENAME, std::ios_base::app);
    }
  }
#else
  {
  }
#endif

  static log_intermediate make_info(const char* s, const size_t l, const char* f) { return log_intermediate(prefix_i, suffix_i, type_i, s, l, f); }
  static log_intermediate make_debug(const char* s, const size_t l, const char* f) { return log_intermediate(prefix_d, suffix_d, type_d, s, l, f); }
  static log_intermediate make_warn(const char* s, const size_t l, const char* f) { return log_intermediate(prefix_w, suffix_w, type_w, s, l, f); }
  static log_intermediate make_error(const char* s, const size_t l, const char* f) { return log_intermediate(prefix_e, suffix_e, type_e, s, l, f); }

  template <typename T> log_intermediate& operator<<(const T& in) noexcept {
    try {
      buffer << in;
      return *this;
    } catch(std::exception& e) {
      buffer << "<<<<<exception on " << MYLOGGER_GET_FUNCTION << " what=" << e.what() << ">>>>>";
      return *this;
    } catch(...) {
      buffer << "<<<<<exception on " << MYLOGGER_GET_FUNCTION << " uknown>>>>>";
      return *this;
    }
  }

  ~log_intermediate() noexcept {
    try {
      const auto now      = std::chrono::system_clock::now();
      const time_t tt     = std::chrono::system_clock::to_time_t(now);
      const auto timeinfo = localtime(&tt);
      const auto nowMs    = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()) % 1000;
      const int msec      = nowMs.count();
      std::stringstream s;
      s << prefix << timeinfo->tm_year + 1900 << "/" << timeinfo->tm_mon << "/" << timeinfo->tm_mday << " " << timeinfo->tm_hour << ":" << timeinfo->tm_min << ":" << timeinfo->tm_sec << "." << msec << " " << type << " " << function << " " << buffer.str() << suffix << std::endl;

#ifdef MYLOGGER_OUTPUT_FILENAME
      myFile << s.str();
#else
      std::cout << s.str() << ::std::flush;
#endif
    } catch(const std::exception& e) {
      std::cerr << "\n\n<<<<<\nexception on " << MYLOGGER_GET_FUNCTION << "\nwhat=" << e.what() << "\n>>>>>\n\n";
    } catch(...) {
      std::cerr << "\n\n<<<<<\nexception on " << MYLOGGER_GET_FUNCTION << "\nunknown\n>>>>>\n\n";
    }
  }
};
} // namespace myLogger
