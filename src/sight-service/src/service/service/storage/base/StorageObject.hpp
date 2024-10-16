#pragma once

#include <any>
#include <utility>
#include <string>
#include <optional>
#include <vector>
#include <variant>

#include "StorageTypes.hpp"

namespace service::storage::base {

class IObject {
  public:
    virtual ~IObject() = default;

    using Key = std::string;
    using ValueVariant = std::variant<
      std::string,
      std::vector<std::unique_ptr<IObject>>,
      std::unique_ptr<IObject>>;
    using Value = std::pair<StorageDataTypes, ValueVariant>;
    using Values = std::vector<std::pair<Key, Value>>;

    virtual StorageError AddPair(
      const std::string& key,
      Value value) = 0;
    virtual std::any GetObject() = 0;
    virtual std::optional<Values> GetObjectValues() = 0;
};

} // service::storage::base
