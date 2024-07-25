#pragma once

#include <glad/glad.h>
#include <iostream>

static GLenum __glCheckError(const char* file, int line, const char* msg)
{
    GLenum errorCode;
    while ((errorCode = glGetError()) != GL_NO_ERROR)
    {
	std::string error;
	switch (errorCode)
	{
	    case GL_INVALID_ENUM:		    error = "INVALID_ENUM";			break;
	    case GL_INVALID_VALUE:		    error = "INVALID_VALUE";			break;
	    case GL_INVALID_OPERATION:		    error = "INVALID_OPERATION";		break;
	    case GL_INVALID_FRAMEBUFFER_OPERATION:  error = "INVALID_FRAMEBUFFER_OPERATION";	break;
	    /* case GL_STACK_OVERFLOW:		    error = "STACK_OVERFLOW";			break; */
	    /* case GL_STACK_UNDERFLOW:		    error = "STACK_UNDERFLOW";			break; */
	    case GL_OUT_OF_MEMORY:		    error = "OUT_OF_MEMORY";			break;
	}

	std::cerr << error << " | " << file << " (" << line << "): " << msg << '\n';
    }
    return errorCode;
}

#ifndef glCheckError
#define glCheckError(MSG) __glCheckError(__FILE__, __LINE__, MSG);
#endif
