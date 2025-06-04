#include "arrow_utils.h"

#include <memory>
#include <vector>

namespace cppcommon::os {
arrow::Result<std::shared_ptr<arrow::RecordBatch>> MergeRecordBatchesByColumns(
    const std::vector<std::shared_ptr<arrow::RecordBatch>>& batches) {
  if (batches.empty()) {
    return arrow::Status::Invalid("No RecordBatches provided");
  }

  int64_t num_rows = batches[0]->num_rows();

  std::vector<std::shared_ptr<arrow::Array>> all_columns;
  std::vector<std::shared_ptr<arrow::Field>> all_fields;

  for (const auto& batch : batches) {
    if (batch->num_rows() != num_rows) {
      return arrow::Status::Invalid("All RecordBatches must have the same number of rows");
    }

    for (int i = 0; i < batch->num_columns(); ++i) {
      all_columns.push_back(batch->column(i));
      all_fields.push_back(batch->schema()->field(i));
    }
  }

  auto merged_schema = arrow::schema(all_fields);
  return arrow::RecordBatch::Make(merged_schema, num_rows, all_columns);
}
}  // namespace cppcommon::os
