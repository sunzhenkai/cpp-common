#pragma once

#include <spdlog/logger.h>

#include <memory>

#include "spdlog/async.h"
#include "spdlog/sinks/rotating_file_sink.h"
#include "spdlog/spdlog.h"

namespace cppcommon::extends {
extern const std::shared_ptr<spdlog::logger> kConsoleLogger;

#ifndef SPDLOG_LEVEL
#ifdef ENABLE_DEBUG
#define SPDLOG_LEVEL spdlog::level::debug
#else
#define SPDLOG_LEVEL spdlog::level::info
#endif
#endif

#ifndef CCM_LOG_DIR
#define CCM_LOG_DIR "log/"
#endif

#ifndef DECLARE_LOGGER
#define GENERAL_LOG_FORMAT "[%m%d-%H:%M:%S:%e][%s:%#][%l][P%P-T%t] %v"
#define SIMPLE_LOG_FORMAT "%v"
// size: mb, file: log file name without .log suffix
#define DECLARE_LOGGER(name, file, size, backup, level, format)                                   \
  struct name##Logger {                                                                           \
    std::shared_ptr<spdlog::logger> logger;                                                       \
    name##Logger() {                                                                              \
      logger = spdlog::rotating_logger_mt<spdlog::async_factory>(#name, CCM_LOG_DIR #file ".log", \
                                                                 (size) * 1024 * 1024, backup);   \
      logger->set_pattern(format);                                                                \
      logger->set_level(level);                                                                   \
      spdlog::flush_every(std::chrono::seconds(1));                                               \
    }                                                                                             \
    static spdlog::logger *GetLogger() {                                                          \
      static name##Logger logger;                                                                 \
      return logger.logger.get();                                                                 \
    }                                                                                             \
  }
#endif

DECLARE_LOGGER(Runtime, runtime, 1024, 5, SPDLOG_LEVEL, GENERAL_LOG_FORMAT);
DECLARE_LOGGER(Record, record, 1024, 5, SPDLOG_LEVEL, GENERAL_LOG_FORMAT);

//< runtime log for normal infomation printing
#define RUNTIME_DEBUG(format, args...) \
  SPDLOG_LOGGER_DEBUG(cppcommon::extends::RuntimeLogger::GetLogger(), format, ##args)
#define RUNTIME_INFO(format, args...) SPDLOG_LOGGER_INFO(cppcommon::extends::RuntimeLogger::GetLogger(), format, ##args)
#define RUNTIME_WARN(format, args...) SPDLOG_LOGGER_WARN(cppcommon::extends::RuntimeLogger::GetLogger(), format, ##args)
#define RUNTIME_ERROR(format, args...) \
  SPDLOG_LOGGER_ERROR(cppcommon::extends::RuntimeLogger::GetLogger(), format, ##args)
#define RUNTIME_CRITICAL(format, args...) \
  SPDLOG_LOGGER_CRITICAL(cppcommon::extends::RuntimeLogger::GetLogger(), format, ##args)

//< RINFO, RWARN, RERROR, RCRITICAL for important log printing
//< these logs will be sinked into stdout and record.log
#ifndef RINFO
#define RINFO(format, args...)                                                       \
  SPDLOG_LOGGER_INFO(cppcommon::extends::RecordLogger::GetLogger(), format, ##args); \
  cppcommon::extends::kConsoleLogger->info(format, ##args);
#endif  // !RINFO
#ifndef RWARN
#define RWARN(format, args...)                                                       \
  SPDLOG_LOGGER_WARN(cppcommon::extends::RecordLogger::GetLogger(), format, ##args); \
  cppcommon::extends::kConsoleLogger->warn(format, ##args);
#endif  // !RWARN
#ifndef RERROR
#define RERROR(format, args...)                                                       \
  SPDLOG_LOGGER_ERROR(cppcommon::extends::RecordLogger::GetLogger(), format, ##args); \
  cppcommon::extends::kConsoleLogger->error(format, ##args);
#endif  // !RERROR
#ifndef RCRITICAL
#define RCRITICAL(format, args...)                                                       \
  SPDLOG_LOGGER_CRITICAL(cppcommon::extends::RecordLogger::GetLogger(), format, ##args); \
  cppcommon::extends::kConsoleLogger->critical(format, ##args);
#endif  // !RCRITICAL

//< c=console, these logs will be sinked into stdout
#ifndef CINFO
#define CINFO(format, args...) cppcommon::extends::kConsoleLogger->info(format, ##args);
#endif  // !CINFO
#ifndef CWARN
#define CWARN(format, args...) cppcommon::extends::kConsoleLogger->warn(format, ##args);
#endif  // !CWARN
#ifndef CERROR
#define CERROR(format, args...) cppcommon::extends::kConsoleLogger->error(format, ##args);
#endif  // !CERROR
#ifndef CCRITICAL
#define CCRITICAL(format, args...) cppcommon::extends::kConsoleLogger->critical(format, ##args);
#endif  // !CCRITICAL

// Normal Logger
#ifndef LOG_ERR
#define LOG_ERR(format, args...) spdlog::error(format, ##args)
#endif
#ifndef LOG_INFO
#define LOG_INFO(format, args...) spdlog::info(format, ##args)
#endif
#ifndef LOG_WARN
#define LOG_WARN(format, args...) spdlog::warn(format, ##args)
#endif
#ifndef LOG_DBG
#define LOG_DBG(format, args...) spdlog::debug(format, ##args)
#endif
#ifndef LOG_CRITICAL
#define LOG_CRITICAL(format, args...) spdlog::critical(format, ##args)
#endif
}  // namespace cppcommon::extends
