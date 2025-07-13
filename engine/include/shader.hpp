#include <fstream>
#include <shaderc/shaderc.hpp>

#include "types.hpp"

namespace hep {

enum class ShaderStage { VERTEX, FRAGMENT, COMPUTE };

class Shader {
 public:
  Shader(const Shader&) = delete;
  Shader& operator=(const Shader&) = delete;

  Shader() = default;
  ~Shader() = default;

  void compile(const std::string& path, ShaderStage shaderStage);

 private:
  shaderc_shader_kind stage;
  std::vector<u32> spirv;
};

}  // namespace hep
