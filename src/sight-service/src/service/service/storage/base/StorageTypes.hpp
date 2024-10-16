#pragma once

namespace service::storage::base {

enum class StorageError {
  eDefault,
  eOk,
  eNok
};

enum class StorageDataTypes {
  eDefault,
  eInt,
  eFloat,
  eString,
  eObject,
  eArray
};

} // service::storage::base
