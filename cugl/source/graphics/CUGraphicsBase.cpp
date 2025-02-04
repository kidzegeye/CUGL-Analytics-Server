//
//  CUGraphicsBase.cpp
//  Cornell University Game Library (CUGL)
//
//  This file contains some simple OpenGL debugging tools
//
//  CUGL MIT License:
//      This software is provided 'as-is', without any express or implied
//      warranty.  In no event will the authors be held liable for any damages
//      arising from the use of this software.
//
//      Permission is granted to anyone to use this software for any purpose,
//      including commercial applications, and to alter it and redistribute it
//      freely, subject to the following restrictions:
//
//      1. The origin of this software must not be misrepresented; you must not
//      claim that you wrote the original software. If you use this software
//      in a product, an acknowledgment in the product documentation would be
//      appreciated but is not required.
//
//      2. Altered source versions must be plainly marked as such, and must not
//      be misrepresented as being the original software.
//
//      3. This notice may not be removed or altered from any source distribution.
//
//  Author: Walker White
//  Version: 7/3/24 (CUGL 3.0 reorganization)
//
#include <cugl/graphics/CUGraphicsBase.h>

/**
 * Returns a string description of an OpenGL error type
 *
 * @param type  The OpenGL error type
 *
 * @return a string description of an OpenGL error type
 */
std::string cugl::graphics::gl_error_name(GLenum type) {
    std::string error = "UNKNOWN";
    
    switch(type) {
        case 0:
            error="NO ERROR";
            break;
        case GL_INVALID_OPERATION:
            error="INVALID_OPERATION";
            break;
        case GL_INVALID_ENUM:
            error="INVALID_ENUM";
            break;
        case GL_INVALID_VALUE:
            error="INVALID_VALUE";
            break;
        case GL_OUT_OF_MEMORY:
            error="OUT_OF_MEMORY";
            break;
        case GL_INVALID_FRAMEBUFFER_OPERATION:
            error="INVALID_FRAMEBUFFER_OPERATION";
            break;
        case GL_FRAMEBUFFER_UNDEFINED:
            error="FRAMEBUFFER_UNDEFINED";
            break;
        case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT:
            error="FRAMEBUFFER_INCOMPLETE_ATTACHMENT";
            break;
        case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT:
            error="FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT";
            break;
#if (CU_GL_PLATFORM == CU_GL_OPENGL)
        case GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER:
            error="FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER";
            break;
        case GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER:
            error="FRAMEBUFFER_INCOMPLETE_READ_BUFFER";
            break;
#endif
        case GL_FRAMEBUFFER_UNSUPPORTED:
            error="FRAMEBUFFER_UNSUPPORTED";
            break;
        case GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE:
            error="FRAMEBUFFER_INCOMPLETE_MULTISAMPLE";
            break;
#if (CU_GL_PLATFORM == CU_GL_OPENGL)
        case GL_FRAMEBUFFER_INCOMPLETE_LAYER_TARGETS:
            error="FRAMEBUFFER_INCOMPLETE_LAYER_TARGETS";
            break;
#endif
    }
    return error;
}

/**
 * Returns a string description of an OpenGL data type
 *
 * @param type  The OpenGL error type
 *
 * @return a string description of an OpenGL data type
 */
std::string cugl::graphics::gl_type_name(GLenum type) {
    switch(type) {
        case GL_FLOAT:
            return "GL_FLOAT";
        case GL_FLOAT_VEC2:
            return "GL_FLOAT";
        case GL_FLOAT_VEC3:
            return "GL_FLOAT_VEC2";
        case GL_FLOAT_VEC4:
            return "GL_FLOAT_VEC4";
        case GL_FLOAT_MAT2:
            return "GL_FLOAT_MAT2";
        case GL_FLOAT_MAT3:
            return "GL_FLOAT_MAT3";
        case GL_FLOAT_MAT4:
            return "GL_FLOAT_MAT4";
        case GL_FLOAT_MAT2x3:
            return "GL_FLOAT_MAT2x3";
        case GL_FLOAT_MAT2x4:
            return "GL_FLOAT_MAT2x4";
        case GL_FLOAT_MAT3x2:
            return "GL_FLOAT_MAT3x2";
        case GL_FLOAT_MAT3x4:
            return "GL_FLOAT_MAT3x4";
        case GL_FLOAT_MAT4x2:
            return "GL_FLOAT_MAT4x2";
        case GL_FLOAT_MAT4x3:
            return "GL_FLOAT_MAT4x3";
        case GL_INT:
            return "GL_INT";
        case GL_INT_VEC2:
            return "GL_INT_VEC2";
        case GL_INT_VEC3:
            return "GL_INT_VEC3";
        case GL_INT_VEC4:
            return "GL_INT_VEC4";
        case GL_UNSIGNED_INT:
            return "GL_UNSIGNED_INT";
        case GL_UNSIGNED_INT_VEC2:
            return "GL_UNSIGNED_INT_VEC2";
        case GL_UNSIGNED_INT_VEC3:
            return "GL_UNSIGNED_INT_VEC3";
        case GL_UNSIGNED_INT_VEC4:
            return "GL_UNSIGNED_INT_VEC4";
#if (CU_GL_PLATFORM == CU_GL_OPENGL)
        case GL_DOUBLE:
            return "GL_GL_DOUBLE";
        case GL_DOUBLE_VEC2:
            return "GL_DOUBLE_VEC2";
        case GL_DOUBLE_VEC3:
            return "GL_DOUBLE_VEC3";
        case GL_DOUBLE_VEC4:
            return "GL_DOUBLE_VEC4";
        case GL_DOUBLE_MAT2:
            return "GL_DOUBLE_MAT2";
        case GL_DOUBLE_MAT3:
            return "GL_DOUBLE_MAT3";
        case GL_DOUBLE_MAT4:
            return "GL_DOUBLE_MAT4";
        case GL_DOUBLE_MAT2x3:
            return "GL_DOUBLE_MAT2x3";
        case GL_DOUBLE_MAT2x4:
            return "GL_DOUBLE_MAT2x4";
        case GL_DOUBLE_MAT3x2:
            return "GL_DOUBLE_MAT3x2";
        case GL_DOUBLE_MAT3x4:
            return "GL_DOUBLE_MAT3x4";
        case GL_DOUBLE_MAT4x2:
            return "GL_DOUBLE_MAT4x2";
        case GL_DOUBLE_MAT4x3:
            return "GL_DOUBLE_MAT4x3";
#endif
        case GL_SAMPLER_2D:
            return "GL_SAMPLER_2D";
        case GL_SAMPLER_3D:
            return "GL_SAMPLER_3D";
        case GL_SAMPLER_CUBE:
            return "GL_SAMPLER_CUBE";
        case GL_SAMPLER_2D_SHADOW:
            return "GL_SAMPLER_2D_SHADOW";
        case GL_UNIFORM_BUFFER:
            return "GL_UNIFORM_BUFFER";
    }
    return "GL_UNKNOWN";
}
