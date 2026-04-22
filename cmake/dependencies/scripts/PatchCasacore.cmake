cmake_minimum_required(VERSION 3.15)

if(NOT DEFINED source_dir OR NOT DEFINED patch_dir)
  message(FATAL_ERROR "source_dir and patch_dir must be provided")
endif()

file(COPY_FILE
  "${patch_dir}/CasacoreHostInfoDarwin.h"
  "${source_dir}/casacore/casa/OS/HostInfoDarwin.h"
  ONLY_IF_DIFFERENT)

set(host_info_cc "${source_dir}/casa/OS/HostInfo.cc")
file(READ "${host_info_cc}" host_info_contents)

set(old_define "#define _XOPEN_SOURCE 600       //For clock_gettime")
set(new_define "#if !defined(__APPLE__)\n#define _XOPEN_SOURCE 600       //For clock_gettime\n#endif")
string(REPLACE "${old_define}" "${new_define}" host_info_contents "${host_info_contents}")

file(WRITE "${host_info_cc}" "${host_info_contents}")

set(array2math_cc "${source_dir}/casa/Arrays/Array2Math.cc")
file(READ "${array2math_cc}" array2math_contents)

string(REPLACE
  "  arrayTransform (carray, rarray, std::abs<float>);"
  "  arrayTransform (carray, rarray, [](std::complex<float> v) { return std::abs(v); });"
  array2math_contents
  "${array2math_contents}")
string(REPLACE
  "  arrayTransform (carray, rarray, std::abs<double>);"
  "  arrayTransform (carray, rarray, [](std::complex<double> v) { return std::abs(v); });"
  array2math_contents
  "${array2math_contents}")

file(WRITE "${array2math_cc}" "${array2math_contents}")
