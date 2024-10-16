add_library(
  follower-server-public
    INTERFACE
      ${CMAKE_CURRENT_SOURCE_DIR}/public/FollowerServer.hpp
      ${CMAKE_CURRENT_SOURCE_DIR}/public/FollowerTypes.hpp
)

target_include_directories(
  follower-server-public
    INTERFACE
      ${CMAKE_CURRENT_SOURCE_DIR}/public/
)

add_library(
  follower-server-private
    STATIC
      ${CMAKE_CURRENT_SOURCE_DIR}/private/FollowerServerImpl.cpp
)

target_include_directories(
  follower-server-private
    PUBLIC
      ${CMAKE_CURRENT_SOURCE_DIR}/public
)

target_link_libraries(
  follower-server-private
    PUBLIC
		  follower-proto
      sight-common-public
		  libgrpcxx::grpcxx
      spdlog::spdlog
)

add_subdirectory(proto)
