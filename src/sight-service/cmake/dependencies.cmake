find_package(
  Protobuf 3.15.0
    REQUIRED
)

find_package(
  mongocxx
    CONFIG
      REQUIRED
)

find_package(
  bsoncxx
    CONFIG
      REQUIRED
)

find_package(
  cpr
    REQUIRED
)

include(FetchContent)

# grpcxx
FetchContent_Declare(
  grpcxx
    URL https://github.com/uatuko/grpcxx/archive/refs/tags/v0.1.4.tar.gz
)

FetchContent_MakeAvailable(grpcxx)

# googletest
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/refs/tags/v1.14.0.tar.gz
)

FetchContent_MakeAvailable(googletest)

# spdlog
FetchContent_Declare(
  spdlog
  URL https://github.com/gabime/spdlog/archive/refs/tags/v1.13.0.tar.gz
)

FetchContent_MakeAvailable(spdlog)

# json

FetchContent_Declare(
  json
  URL https://github.com/nlohmann/json/releases/download/v3.11.3/json.tar.xz
)

FetchContent_MakeAvailable(json)
