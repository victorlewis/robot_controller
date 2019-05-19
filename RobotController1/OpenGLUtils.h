/**
\file minimalOpenGL/minimalOpenGL.h
\author Morgan McGuire, http://graphics.cs.williams.edu
Distributed with the G3D Innovation Engine http://g3d.cs.williams.edu

Minimal headers emulating a basic set of 3D graphics classes.
This does not depend on any vector math library.

This requires the headers and source (or static binary) from GLEW

http://glew.sourceforge.net/

and from GLFW

http://www.glfw.org/

(All of which are distributed with G3D)

All 3D math from http://graphicscodex.com
*/
#pragma once

#ifdef __APPLE__
#   define _OSX
#elif defined(_WIN64)
#   ifndef _WINDOWS
#       define _WINDOWS
#   endif
#elif defined(__linux__)
#   define _LINUX
#endif

#include <GL/glew.h>
#ifdef _WINDOWS
#   include <GL/wglew.h>
#elif defined(_LINUX)
#   include <GL/xglew.h>
#endif
#include <GLFW/glfw3.h> 


#ifdef _WINDOWS
// Link against OpenGL
#   pragma comment(lib, "opengl32")
//#   pragma comment(lib, "glew_x64") // already linked in properties
//#   pragma comment(lib, "glfw_x64") // already linked in properties
#endif

#include <cstring>
#include <cstdio>
#include <cstdint>
#include <cstdlib>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <iomanip>
#include <cmath>
#include <cassert>
#include <vector>

void APIENTRY debugCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* userParam) {
	if ((type == GL_DEBUG_TYPE_ERROR) || (type == GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR)) {
		fprintf(stderr, "GL Debug: %s\n", message);
	}
}

GLFWwindow* initOpenGL(int width, int height, const std::string& title) {
	if (!glfwInit()) {
		fprintf(stderr, "ERROR: could not start GLFW\n");
		::exit(1);
	}

	// Without these, shaders actually won't initialize properly
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);




#   ifdef _DEBUG
	glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);
#   endif

	GLFWwindow* window = glfwCreateWindow(width, height, title.c_str(), nullptr, nullptr);
	if (!window) {
		fprintf(stderr, "ERROR: could not open window with GLFW\n");
		glfwTerminate();
		::exit(2);
	}
	glfwMakeContextCurrent(window);

	// Start GLEW extension handler, with improved support for new features
	glewExperimental = GL_TRUE;
	glewInit();

	// Clear startup errors
	while (glGetError() != GL_NONE) {}

#   ifdef _DEBUG
	glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
	glEnable(GL_DEBUG_OUTPUT);
#       ifndef _OSX
	// Causes a segmentation fault on OS X
	glDebugMessageCallback(debugCallback, nullptr);
#       endif
#   endif

	// Negative numbers allow buffer swaps even if they are after the vertical retrace,
	// but that causes stuttering in VR mode
	glfwSwapInterval(0);

	fprintf(stderr, "GPU: %s (OpenGL version %s)\n", glGetString(GL_RENDERER), glGetString(GL_VERSION));

	// Bind a single global vertex array (done this way since OpenGL 3)
	{ GLuint vao; glGenVertexArrays(1, &vao); glBindVertexArray(vao); }

	// Check for errors
	{ const GLenum error = glGetError(); assert(error == GL_NONE); }

	return window;
}


std::string loadTextFile(const std::string& filename) {
	std::stringstream buffer;
	buffer << std::ifstream(filename.c_str()).rdbuf();
	return buffer.str();
}


GLuint compileShaderStage(GLenum stage, const std::string& source) {
	GLuint shader = glCreateShader(stage);
	const char* srcArray[] = { source.c_str() };

	glShaderSource(shader, 1, srcArray, NULL);
	glCompileShader(shader);

	GLint success = 0;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &success);

	if (success == GL_FALSE) {
		GLint logSize = 0;
		glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &logSize);

		std::vector<GLchar> errorLog(logSize);
		glGetShaderInfoLog(shader, logSize, &logSize, &errorLog[0]);

		fprintf(stderr, "Error while compiling\n %s\n\nError: %s\n", source.c_str(), &errorLog[0]);
		assert(false);

		glDeleteShader(shader);
		shader = GL_NONE;
	}

	return shader;
}


GLuint createShaderProgram(const std::string& vertexShaderSource, const std::string& geometryShaderSource, const std::string& pixelShaderSource) {
	GLuint shader = glCreateProgram();

	glAttachShader(shader, compileShaderStage(GL_VERTEX_SHADER, vertexShaderSource));
	glAttachShader(shader, compileShaderStage(GL_GEOMETRY_SHADER, geometryShaderSource));
	glAttachShader(shader, compileShaderStage(GL_FRAGMENT_SHADER, pixelShaderSource));
	glLinkProgram(shader);

	return shader;
}

GLuint createShaderProgram(const std::string& vertexShaderSource, const std::string& pixelShaderSource) {
	GLuint shader = glCreateProgram();

	glAttachShader(shader, compileShaderStage(GL_VERTEX_SHADER, vertexShaderSource));
	glAttachShader(shader, compileShaderStage(GL_FRAGMENT_SHADER, pixelShaderSource));
	glLinkProgram(shader);

	return shader;
}
namespace Cube {
	const float position[][3] = { -.5f, .5f, -.5f, -.5f, .5f, .5f, .5f, .5f, .5f, .5f, .5f, -.5f, -.5f, .5f, -.5f, -.5f, -.5f, -.5f, -.5f, -.5f, .5f, -.5f, .5f, .5f, .5f, .5f, .5f, .5f, -.5f, .5f, .5f, -.5f, -.5f, .5f, .5f, -.5f, .5f, .5f, -.5f, .5f, -.5f, -.5f, -.5f, -.5f, -.5f, -.5f, .5f, -.5f, -.5f, .5f, .5f, -.5f, -.5f, .5f, .5f, -.5f, .5f, .5f, .5f, .5f, -.5f, -.5f, .5f, -.5f, -.5f, -.5f, .5f, -.5f, -.5f, .5f, -.5f, .5f };
	const float normal[][3] = { 0.f, 1.f, 0.f, 0.f, 1.f, 0.f, 0.f, 1.f, 0.f, 0.f, 1.f, 0.f, -1.f, 0.f, 0.f, -1.f, 0.f, 0.f, -1.f, 0.f, 0.f, -1.f, 0.f, 0.f, 1.f, 0.f, 0.f, 1.f, 0.f, 0.f, 1.f, 0.f, 0.f, 1.f, 0.f, 0.f, 0.f, 0.f, -1.f, 0.f, 0.f, -1.f, 0.f, 0.f, -1.f, 0.f, 0.f, -1.f, 0.f, 0.f, 1.f, 0.f, 0.f, 1.f, 0.f, 0.f, 1.f, 0.f, 0.f, 1.f, 0.f, -1.f, 0.f, 0.f, -1.f, 0.f, 0.f, -1.f, 0.f, 0.f, -1.f, 0.f };
	const float tangent[][4] = { 1.f, 0.f, 0.f, 1.f, 1.f, 0.f, 0.f, 1.f, 1.f, 0.f, 0.f, 1.f, 1.f, 0.f, 0.f, 1.f, 0.f, 0.f, 1.f, 1.f, 0.f, 0.f, 1.f, 1.f, 0.f, 0.f, 1.f, 1.f, 0.f, 0.f, 1.f, 1.f, 0.f, 0.f, -1.f, 1.f, 0.f, 0.f, -1.f, 1.f, 0.f, 0.f, -1.f, 1.f, 0.f, 0.f, -1.f, 1.f, -1.f, 0.f, 0.f, 1.f, -1.f, 0.f, 0.f, 1.f, -1.f, 0.f, 0.f, 1.f, -1.f, 0.f, 0.f, 1.f, 1.f, 0.f, 0.f, 1.f, 1.f, 0.f, 0.f, 1.f, 1.f, 0.f, 0.f, 1.f, 1.f, 0.f, 0.f, 1.f, 1.f, 0.f, 0.f, 1.f, 1.f, 0.f, 0.f, 1.f, 1.f, 0.f, 0.f, 1.f, 1.f, 0.f, 0.f, 1.f };
	const float texCoord[][2] = { 0.f, 0.f, 0.f, 1.f, 1.f, 1.f, 1.f, 0.f, 0.f, 0.f, 0.f, 1.f, 1.f, 1.f, 1.f, 0.f, 0.f, 0.f, 0.f, 1.f, 1.f, 1.f, 1.f, 0.f, 0.f, 0.f, 0.f, 1.f, 1.f, 1.f, 1.f, 0.f, 0.f, 0.f, 0.f, 1.f, 1.f, 1.f, 1.f, 0.f, 0.f, 0.f, 0.f, 1.f, 1.f, 1.f, 1.f, 0.f };
	const int   index[] = { 0, 1, 2, 0, 2, 3, 4, 5, 6, 4, 6, 7, 8, 9, 10, 8, 10, 11, 12, 13, 14, 12, 14, 15, 16, 17, 18, 16, 18, 19, 20, 21, 22, 20, 22, 23 };
};

/** Loads a 24- or 32-bit BMP file into memory */
void loadBMP(const std::string& filename, int& width, int& height, int& channels, std::vector<std::uint8_t>& data) {
	std::fstream hFile(filename.c_str(), std::ios::in | std::ios::binary);
	if (!hFile.is_open()) { throw std::invalid_argument("Error: File Not Found."); }

	hFile.seekg(0, std::ios::end);
	size_t len = hFile.tellg();
	hFile.seekg(0, std::ios::beg);
	std::vector<std::uint8_t> header(len);
	hFile.read(reinterpret_cast<char*>(header.data()), 54);

	if ((header[0] != 'B') && (header[1] != 'M')) {
		hFile.close();
		throw std::invalid_argument("Error: File is not a BMP.");
	}

	if ((header[28] != 24) && (header[28] != 32)) {
		hFile.close();
		throw std::invalid_argument("Error: File is not uncompressed 24 or 32 bits per pixel.");
	}

	const short bitsPerPixel = header[28];
	channels = bitsPerPixel / 8;
	width = header[18] + (header[19] << 8);
	height = header[22] + (header[23] << 8);
	std::uint32_t offset = header[10] + (header[11] << 8);
	std::uint32_t size = ((width * bitsPerPixel + 31) / 32) * 4 * height;
	data.resize(size);

	hFile.seekg(offset, std::ios::beg);
	hFile.read(reinterpret_cast<char*>(data.data()), size);
	hFile.close();

	// Flip the y axis
	std::vector<std::uint8_t> tmp;
	const size_t rowBytes = width * channels;
	tmp.resize(rowBytes);
	for (int i = height / 2 - 1; i >= 0; --i) {
		const int j = height - 1 - i;
		// Swap the rows
		memcpy(tmp.data(), &data[i * rowBytes], rowBytes);
		memcpy(&data[i * rowBytes], &data[j * rowBytes], rowBytes);
		memcpy(&data[j * rowBytes], tmp.data(), rowBytes);
	}

	// Convert BGR[A] format to RGB[A] format
	if (channels == 4) {
		// BGRA
		std::uint32_t* p = reinterpret_cast<std::uint32_t*>(data.data());
		for (int i = width * height - 1; i >= 0; --i) {
			const unsigned int x = p[i];
			p[i] = ((x >> 24) & 0xFF) | (((x >> 16) & 0xFF) << 8) | (((x >> 8) & 0xFF) << 16) | ((x & 0xFF) << 24);
		}
	}
	else {
		// BGR
		for (int i = (width * height - 1) * 3; i >= 0; i -= 3) {
			std::swap(data[i], data[i + 2]);
		}
	}
}

struct standardFrameBuffer
{
	GLuint* buffer;
	GLuint* colorRenderTarget;
	GLuint* depthRenderTarget;

	uint32_t width;
	uint32_t height;

	uint32_t numBuffers;

	standardFrameBuffer(uint32_t in_width, uint32_t in_height, uint32_t in_numBuffers) :
		buffer(new GLuint[in_numBuffers]),
		depthRenderTarget(new GLuint[in_numBuffers]),
		colorRenderTarget(new GLuint[in_numBuffers])
	{
		width = in_width;
		height = in_height;
		numBuffers = in_numBuffers;

		// GENERATE THE FRAME BUFFER AND TEXTURES
		glGenFramebuffers(numBuffers, buffer);
		glGenTextures(numBuffers, colorRenderTarget);
		glGenTextures(numBuffers, depthRenderTarget);

		for (int i = 0; i < numBuffers; ++i)
		{
			glBindTexture(GL_TEXTURE_2D, colorRenderTarget[i]);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR); // check later if nearest filter is best
			//glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, width, height, 0, GL_RGBA, GL_FLOAT, nullptr);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);

			glBindTexture(GL_TEXTURE_2D, depthRenderTarget[i]);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT24, width, height, 0, GL_DEPTH_COMPONENT, GL_UNSIGNED_INT, nullptr);

			glBindFramebuffer(GL_FRAMEBUFFER, buffer[i]);
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, colorRenderTarget[i], 0);
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthRenderTarget[i], 0);
		}
		glBindFramebuffer(GL_FRAMEBUFFER, 0);

	}


	standardFrameBuffer(uint32_t in_width, uint32_t in_height, uint32_t in_numBuffers, GLint colorFormat) :
		buffer(new GLuint[in_numBuffers]),
		depthRenderTarget(new GLuint[in_numBuffers]),
		colorRenderTarget(new GLuint[in_numBuffers])
	{
		width = in_width;
		height = in_height;
		numBuffers = in_numBuffers;

		// GENERATE THE FRAME BUFFER AND TEXTURES
		glGenFramebuffers(numBuffers, buffer);
		glGenTextures(numBuffers, colorRenderTarget);
		glGenTextures(numBuffers, depthRenderTarget);

		for (int i = 0; i < numBuffers; ++i)
		{
			glBindTexture(GL_TEXTURE_2D, colorRenderTarget[i]);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR); // check later if nearest filter is best
			glTexImage2D(GL_TEXTURE_2D, 0, colorFormat, width, height, 0, GL_RGBA, GL_FLOAT, nullptr);

			glBindTexture(GL_TEXTURE_2D, depthRenderTarget[i]);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT24, width, height, 0, GL_DEPTH_COMPONENT, GL_UNSIGNED_INT, nullptr);

			glBindFramebuffer(GL_FRAMEBUFFER, buffer[i]);
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, colorRenderTarget[i], 0);
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthRenderTarget[i], 0);
		}
		glBindFramebuffer(GL_FRAMEBUFFER, 0);

	}

};

struct standardFrameBuffer2DArray
{
	GLuint* buffer;
	GLuint* colorRenderTarget;
	GLuint* depthRenderTarget;

	uint32_t width;
	uint32_t height;
	uint32_t depth;

	uint32_t numBuffers;

	standardFrameBuffer2DArray(uint32_t in_width, uint32_t in_height, uint32_t in_depth, uint32_t in_numBuffers) :
		buffer(new GLuint[in_numBuffers]),
		depthRenderTarget(new GLuint[in_numBuffers]),
		colorRenderTarget(new GLuint[in_numBuffers])
	{
		width = in_width;
		height = in_height;
		depth = in_depth;

		numBuffers = in_numBuffers;

		// GENERATE THE FRAME BUFFER AND TEXTURES
		glGenFramebuffers(numBuffers, buffer);
		glGenTextures(numBuffers, colorRenderTarget);
		glGenTextures(numBuffers, depthRenderTarget);

		for (int i = 0; i < numBuffers; ++i)
		{
			glBindTexture(GL_TEXTURE_2D_ARRAY, colorRenderTarget[i]);
			glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_LINEAR); // check later if nearest filter is best
			glTexImage3D(GL_TEXTURE_2D_ARRAY, 0, GL_RGBA32F, width, height, depth, 0, GL_RGBA, GL_FLOAT, nullptr);

			glBindTexture(GL_TEXTURE_2D_ARRAY, depthRenderTarget[i]);
			glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
			glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
			glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			glTexImage3D(GL_TEXTURE_2D_ARRAY, 0, GL_DEPTH_COMPONENT24, width, height, depth, 0, GL_DEPTH_COMPONENT, GL_UNSIGNED_INT, nullptr);

			glBindFramebuffer(GL_FRAMEBUFFER, buffer[i]);
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D_ARRAY, colorRenderTarget[i], 0);
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D_ARRAY, depthRenderTarget[i], 0);
		}
		glBindFramebuffer(GL_FRAMEBUFFER, 0);

	}
};