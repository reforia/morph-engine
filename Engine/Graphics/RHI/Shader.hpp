﻿#pragma once
#include "Engine/Core/common.hpp"
#include "Engine/File/Blob.hpp"
#include <unordered_map>
#include "Engine/Graphics/RHI/RHIType.hpp"

class Shader: public std::enable_shared_from_this<Shader> {
  friend class Program;
public:
  using sptr_t = S<Shader>;
  void define(std::string key);
  void define(std::string_view key, std::string_view value);

  bool isReady() const { return mBinary.valid(); }
  void setType(eShaderType type);
  void setFromFile(const std::string_view file, std::string_view entry);
  void setFromString(const std::string_view source, std::string_view entry);
  void* handle() const;
  size_t size() const;
  static sptr_t create(const std::string_view file, std::string_view entry, eShaderType type);

  Shader(const std::string_view file, std::string_view entry, eShaderType type);
  Shader() = default;
protected:
  void compile();
  std::string mFilePath;
  Blob mSource;
  std::string mEntryPoint;
  eShaderType mShaderType = SHADER_TYPE_UNKNOWN;
  Blob mBinary;
  std::unordered_map<std::string, std::string> mDefineDirectives;
};
