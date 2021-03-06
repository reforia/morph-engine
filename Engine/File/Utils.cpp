#include "Utils.hpp"
#include <cstdlib>
#include <fstream>
#include "Path.hpp"
#include <filesystem>
namespace sfs {
  using namespace std::filesystem;
}

fs::path fs::workingPath() {
  return sfs::current_path();
}

fs::path fs::absolute(const path& path, const Path& base) {
  return sfs::absolute(base / path);
}

bool fs::exists(const path& file) {
  return sfs::exists(file);
}

bool fs::isDirectory(const path& path) {
  return is_directory(path);
}

Blob fs::read(const path& filePath) {
  std::ifstream file(filePath.c_str(), std::ios::binary | std::ios::ate);

  std::streamsize size = file.tellg();

  if (size == -1) {
    return Blob();
  }
  file.seekg(0, std::ios::beg);

  char* buffer = new char[(uint)size + 1];

  if (file.read(buffer, size)) {
    buffer[size] = 0;
    Blob b(buffer, (uint)size);
    delete[] buffer;
    return b;
  } else {
    return Blob();
  }
}

bool fs::read(const path& filePath, const char*& outBuffer, size_t& outSize) {
  std::ifstream file(filePath.c_str(), std::ios::binary | std::ios::ate);

  std::streamsize size = file.tellg();

  if (size == -1) {
    return false;
  }
  file.seekg(0, std::ios::beg);

  char* buffer = new char[(uint)size + 1];
  if (file.read(buffer, size)) {
    buffer[size] = 0;
    outBuffer = buffer;
    outSize = size;
    return true;
  } else {
    return false;
  }
}

void fs::write(const path& filePath, const void* buffer, size_t size) {
  std::ofstream file(filePath.c_str(), std::ios::binary | std::ofstream::out | std::ofstream::trunc);

  file.write((const char*)buffer, size);
  
  file.close();
}

void fs::append(const path& filePath, const void* buffer, size_t size) {
  std::ofstream file(filePath.c_str(), std::ofstream::out | std::ofstream::app);

  file.write((const char*)buffer, size);

  file.close();
}

fs::path fs::relative(const path& p, const path& base) {
  return sfs::relative(p, base);
}

const fs::path& fs::dotPath() {
  static fs::path dot = ".";

  return dot;
}

const fs::path& fs::dotdotPath() {
  static const fs::path dotdot("..");

  return dotdot;
}

bool fs::createDir(const path& path) {
  return create_directory(path);
}
