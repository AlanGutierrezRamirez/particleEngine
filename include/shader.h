#pragma once

#include <string>

enum class ShaderType
{
	Vertex,
	Fragment
};

class Shader
{
public:
	Shader() {};
	~Shader() {};


	static void getShaders(std::string& vertex, std::string& fragment);
	static int CompileShader(const std::string& shaderCode, ShaderType shaderType);

private:
	static std::string readFile(const std::string& filePath);

};