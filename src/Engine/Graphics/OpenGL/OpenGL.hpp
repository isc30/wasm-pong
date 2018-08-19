#pragma once

#include <iostream>
#include <vector>
#include <string>
#include <SDL.h>

#ifdef __EMSCRIPTEN__
    #include <GLES3/gl3.h>
#else
    #include <glad/glad.h> // OpenGL ES 3.0
#endif

#include <Engine/Integrations/Emscripten.hpp>

#if defined(DEBUG) && defined(DEBUG_OPENGL)
    #define GL(glCall) glCall; do {} while (isc::gl::checkError(__FILE__, __LINE__, #glCall))
    #define GL_CHECK() do {} while (isc::gl::checkError(__FILE__, __LINE__, "GL_CHECK()"))
#else
    #define GL(glCall) glCall;
    #define GL_CHECK()
#endif

namespace isc
{
    namespace gl
    {
        class OpenglException
            : public std::exception
        {
        public:

            const std::string file;
            const unsigned int line;
            const std::string call;

            const int errorCode;
            const std::string errorDescription;

            const std::string message;

            OpenglException(const std::string& file, const int line, const std::string& call, const int errorCode, const std::string& errorDescription)
                : file(file)
                , line(line)
                , call(call)
                , errorCode(errorCode)
                , errorDescription(errorDescription)
                , message("GL error " + std::to_string(errorCode) + " `"
                    + errorDescription + "` @ " + file + ":" + std::to_string(line)
                    + (call.size() > 0 ? " - `" + call + "`" : ""))
            {
            }

            const char* what() const noexcept override
            {
                return message.c_str();
            }
        };

        bool checkError(const char* file, const int line, const char* call);

        void link();
        void printContext();
        GLuint compileProgram(const char* vertexSource, const char* fragmentSource);
    }
}
