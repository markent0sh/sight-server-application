cmake_path(
  SET
    follower_proto
      ${CMAKE_CURRENT_SOURCE_DIR}/follower/v1/follower.proto
)

cmake_path(
  SET
    follower_grpcxx_header
      ${CMAKE_CURRENT_BINARY_DIR}/follower/v1/follower.grpcxx.pb.h
)

cmake_path(
  SET
    follower_header
      ${CMAKE_CURRENT_BINARY_DIR}/follower/v1/follower.pb.h
)

cmake_path(
  SET
    follower_source
      ${CMAKE_CURRENT_BINARY_DIR}/follower/v1/follower.pb.cc
)

set(
  protos
	  ${follower_proto}
)

set(
  headers
	  ${follower_header}
      ${follower_grpcxx_header}
)

set(
  sources
	  ${follower_source}
)

add_custom_command(
	OUTPUT  ${headers} ${sources}
	DEPENDS ${protos}
	COMMAND ${Protobuf_PROTOC_EXECUTABLE}
	ARGS
		--proto_path=${CMAKE_CURRENT_SOURCE_DIR}
		--proto_path=${Protobuf_INCLUDE_DIR}
		--cpp_out=${CMAKE_CURRENT_BINARY_DIR}
		--grpcxx_out=${CMAKE_CURRENT_BINARY_DIR}
		--plugin=protoc-gen-grpcxx=$<TARGET_FILE:libgrpcxx::protoc-gen>
		${protos}
)

add_library(
  follower-proto
    ${sources}
)

target_include_directories(
  follower-proto
	  PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>
)

target_link_libraries(
  follower-proto
	  protobuf::libprotobuf
)
