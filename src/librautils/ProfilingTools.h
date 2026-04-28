#ifndef LIBRA_PROFILING_TOOLS_H_
#define LIBRA_PROFILING_TOOLS_H_

#include <string>
#include <memory>

#ifdef LIBRA_ENABLE_NVTX

#include <nvtx3/nvToolsExt.h>

namespace libracore {

class NVTXScope {
 public:
  explicit NVTXScope(const std::string& name, uint32_t color = 0xFF00FF00)
      : name_(name) {
    nvtxEventAttributes_t eventAttrib = {0};
    eventAttrib.version = NVTX_VERSION;
    eventAttrib.size = NVTX_EVENT_ATTRIB_STRUCT_SIZE;
    eventAttrib.colorType = NVTX_COLOR_ARGB;
    eventAttrib.color = color;
    eventAttrib.messageType = NVTX_MESSAGE_TYPE_ASCII;
    eventAttrib.message.ascii = name_.c_str();
    nvtxRangePushEx(&eventAttrib);
  }

  ~NVTXScope() {
    nvtxRangePop();
  }

 private:
  std::string name_;
};

// Convenience macro for function-level profiling
#define LIBRA_PROFILE_FUNCTION() \
  libracore::NVTXScope libra_nvtx_scope_(__func__)

// Convenience macro for named scope profiling
#define LIBRA_PROFILE_SCOPE(name) \
  libracore::NVTXScope libra_nvtx_scope_(name)

// Convenience macro with custom color
#define LIBRA_PROFILE_SCOPE_COLOR(name, color) \
  libracore::NVTXScope libra_nvtx_scope_(name, color)

}  // namespace libracore

#else

// No-op implementations when profiling is disabled
#define LIBRA_PROFILE_FUNCTION()
#define LIBRA_PROFILE_SCOPE(name)
#define LIBRA_PROFILE_SCOPE_COLOR(name, color)

#endif  // LIBRA_ENABLE_PROFILING

#endif  // LIBRA_PROFILING_TOOLS_H_
