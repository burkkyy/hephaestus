#include <fstream>
#include <hep/shader.hpp>
#include <shaderc/shaderc.hpp>

#include "util/logger.hpp"

namespace hep {

class Shader::Impl {
 public:
  Impl(const Impl&) = delete;
  Impl& operator=(const Impl&) = delete;

  Impl() = default;
  ~Impl() = default;

  void compile(const std::string& path, ShaderStage shaderStage) {
    switch (shaderStage) {
      case ShaderStage::VERTEX:
        this->stage = shaderc_vertex_shader;
        break;
      case ShaderStage::FRAGMENT:
        this->stage = shaderc_fragment_shader;
        break;
      case ShaderStage::COMPUTE:
        this->stage = shaderc_compute_shader;
        break;
      default:
        throw std::invalid_argument("Unsupported shader stage");
    }

    std::ifstream file(path, std::ios::ate | std::ios::binary);

    if (!file.is_open()) {
      log::error("failed to open shader: " + path);
      throw std::runtime_error("failed to open: " + path);
    }

    size_t fileSize = static_cast<size_t>(file.tellg());
    std::vector<char> fileText(fileSize);

    file.seekg(0);
    file.read(fileText.data(), fileSize);
    file.close();

    log::info("Compiling shader...");

    shaderc::Compiler compiler;
    shaderc::SpvCompilationResult result =
        compiler.CompileGlslToSpv(fileText.data(), this->stage, "user-shader");

    if (result.GetCompilationStatus() != shaderc_compilation_status_success) {
      log::error("failed to compile shader: ", result.GetErrorMessage());
    }

    this->spirv.assign(result.cbegin(), result.cend());

    log::info("Successfully compiled shader: " + path + " (" +
              std::to_string(spirv.size() * 4) + " bytes)");
  }

 private:
  shaderc_shader_kind stage;
  std::vector<u32> spirv;
};

Shader::Shader(const std::string& path, ShaderStage stage)
    : pImpl(std::make_unique<Impl>()) {
  this->pImpl->compile(path, stage);
}

Shader::~Shader() = default;

}  // namespace hep
