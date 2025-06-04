
/**
 * @file arrow_utils.h
 * @brief
 * @author zhenkai.sun
 * @date 2025-06-04 13:58:40
 */
#pragma once
#include <memory>
#include <vector>

#include "arrow/api.h"

namespace cppcommon::os {
arrow::Result<std::shared_ptr<arrow::RecordBatch>> MergeRecordBatchesByColumns(
    const std::vector<std::shared_ptr<arrow::RecordBatch>>& batches);
}
