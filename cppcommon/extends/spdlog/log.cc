#include "log.h"

#include <spdlog/logger.h>
#include <spdlog/sinks/stdout_sinks.h>

#include <memory>

namespace cppcommon::extends {
const std::shared_ptr<spdlog::logger> kConsoleLogger = spdlog::stdout_logger_mt("stdout");
}
