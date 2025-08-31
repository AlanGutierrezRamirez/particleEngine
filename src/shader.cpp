#include <shader.h>

//OpenGL
#include <glew.h>
#include <gl/GL.h>

#include <fstream>
#include <string>
#include <iostream>
#include <sstream>

#define SHADERS_PATH "shaders/"

std::string Shader::readFile(const std::string& filePath) {
  std::ifstream fileStream;
  std::cout << "Shader path: " << filePath << std::endl;
  fileStream.open(filePath);

  if (!fileStream)
  {
    std::cout << "ERROR: Failed to open shader file" << std::endl;
    return "";
  }

  std::stringstream buffer;

  buffer << fileStream.rdbuf();

  return buffer.str();

}

void Shader::getShaders(std::string& vertex, std::string& fragment)
{
  vertex =  std::string(SHADERS_PATH) + vertex;
  fragment = std::string(SHADERS_PATH) + fragment;

  vertex = readFile(vertex);
  fragment = readFile(fragment);
}

int Shader::CompileShader(const std::string& shaderCode, ShaderType shaderType) 
{
  GLuint shaderID;
  
  switch (shaderType)
  {
  case ShaderType::Vertex:
    shaderID = glCreateShader(GL_VERTEX_SHADER);
    break;
  case ShaderType::Fragment:
    shaderID = glCreateShader(GL_FRAGMENT_SHADER);
    break;
  default:
    break;
  }

  const char* src = shaderCode.c_str();
  glShaderSource(shaderID, 1, &src, nullptr);

  glCompileShader(shaderID);

  GLint success;
  glGetShaderiv(shaderID, GL_COMPILE_STATUS, &success);
  if (!success)
  {
    char infoLog[512];
    glGetShaderInfoLog(shaderID, 512, NULL, infoLog);
    std::cout << "ERROR::SHADER::COMPILATION_FAILED\n" << infoLog << std::endl;
  }


  return shaderID;
}