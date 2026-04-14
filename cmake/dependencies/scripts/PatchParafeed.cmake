cmake_minimum_required(VERSION 3.15)

if(NOT DEFINED source_dir)
  message(FATAL_ERROR "source_dir must be provided")
endif()

set(clget_base_code_h "${source_dir}/code/clgetBaseCode.h")
file(READ "${clget_base_code_h}" clget_base_code_contents)

string(REPLACE
  "Symbol* clgetBaseCode(const string& Name, T& val, int& n, SMap &smap=SMap(), bool dbg=false)"
  "Symbol* clgetBaseCode(const string& Name, T& val, int& n, const SMap &smap=SMap(), bool dbg=false)"
  clget_base_code_contents
  "${clget_base_code_contents}")

file(WRITE "${clget_base_code_h}" "${clget_base_code_contents}")
