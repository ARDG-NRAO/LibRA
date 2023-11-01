#include <iostream>

#include <casa_sakura/SakuraUtils.h>

int main(int argc, char *argv[]) {

  // Initialize Sakura
  auto initialize_success = casa::SakuraUtils::InitializeSakura();

  // Query if Sakura is initialized
  auto is_sakura_initialized = casa::SakuraUtils::IsSakuraInitialized();

  // Clean up Sakura
  casa::SakuraUtils::CleanUpSakura();
  
  return 0;
}
