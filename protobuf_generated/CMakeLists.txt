# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#

# Macro for the gen_grpc command
macro(gen_grpc _proto _hdrs _srcs)
  get_filename_component(_proto_path "${_proto}" PATH)
  get_filename_component(_proto_name "${_proto}" NAME_WE)
  set(_proto_srcs "${CMAKE_CURRENT_BINARY_DIR}/${_proto_name}.pb.cc")
  set(_proto_hdrs "${CMAKE_CURRENT_BINARY_DIR}/${_proto_name}.pb.h")
  set(_grpc_srcs "${CMAKE_CURRENT_BINARY_DIR}/${_proto_name}.grpc.pb.cc")
  set(_grpc_hdrs "${CMAKE_CURRENT_BINARY_DIR}/${_proto_name}.grpc.pb.h")
  add_custom_command(
    OUTPUT "${_proto_srcs}" "${_proto_hdrs}" "${_grpc_srcs}" "${_grpc_hdrs}"
    COMMAND ${Protobuf_PROTOC_EXECUTABLE}
    ARGS --grpc_out "${CMAKE_CURRENT_BINARY_DIR}"
    --cpp_out "${CMAKE_CURRENT_BINARY_DIR}"
    -I "${_proto_path}"
    --plugin=protoc-gen-grpc="${_GRPC_CPP_PLUGIN_EXECUTABLE}"
    "${_proto}"
    DEPENDS "${_proto}")
  list(APPEND ${_srcs} "${_proto_srcs}" "${_grpc_srcs}")
  list(APPEND ${_hdrs} "${_proto_hdrs}" "${_grpc_hdrs}")
endmacro()

# List the files that are going to be compiled. It is a recursive search
file(GLOB_RECURSE protobuf_files "../../protos/*.proto")

# Generate C++ headers and sources from the .proto definitions 
#protobuf_generate_cpp(proto_generated_srcs proto_generated_hdrs  ${protobuf_files})
foreach(protobuf_file ${protobuf_files})
  gen_grpc(${protobuf_file} proto_generated_hdrs proto_generated_srcs)
endforeach()

# Create a library with the generated source files
add_library(casacpp_protobuf SHARED)

# Specify which files belong to the target
target_sources(casacpp_protobuf
  PRIVATE
    ${proto_generated_srcs} ${proto_generated_hdrs})

# Add external GRPC dependency.
if (gRPC_USES_FIND_PACKAGE)
  # GRPC has been detected using native cmake support
  target_link_libraries(casacpp_protobuf PUBLIC gRPC::grpc++)
else()
  # GRPC has been detected using pkgconfig
  target_link_libraries(casacpp_protobuf PUBLIC PkgConfig::GRPC)
  target_link_libraries(casacpp_protobuf PUBLIC ${Protobuf_LIBRARIES})
endif()

# Install the library
install(TARGETS casacpp_protobuf
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})
# Install the header and template header files
# Note that this preserves the directory structure of the files
install(DIRECTORY ${CMAKE_SOURCE_DIR}/protobuf_generated
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/casacpp
    FILES_MATCHING
    REGEX "/.*h$")

