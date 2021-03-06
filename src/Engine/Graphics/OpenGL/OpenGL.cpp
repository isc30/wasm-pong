#include "OpenGL.hpp"

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

namespace isc
{
    namespace gl
    {
        bool checkError(const char* file, const int line, const char* call)
        {
            const GLenum error = glGetError();

            if (error == GL_NO_ERROR)
            {
                return false;
            }

            const char* description;

            switch (error)
            {
                case GL_INVALID_ENUM:                   description = "Invalid enum"; break;
                case GL_INVALID_VALUE:                  description = "Invalid value"; break;
                case GL_INVALID_OPERATION:              description = "Invalid operation"; break;
                case GL_OUT_OF_MEMORY:                  description = "Out of memory"; break;
                case GL_INVALID_FRAMEBUFFER_OPERATION:  description = "Invalid framebuffer operation"; break;
                default:                                description = "Unknown error"; break;
            }

            OpenglException exception(file, line, (call == nullptr ? "" : call), error, description);

            std::cout << exception.what() << std::endl;

            throw exception;

            return true;
        }

        void link()
        {
#ifndef __EMSCRIPTEN__
            if (!gladLoadGLES2Loader(SDL_GL_GetProcAddress))
            {
                std::cout << "Error initializing OpenGL ES 3.0" << std::endl;
                std::abort();
            }
#endif

#ifdef __EMSCRIPTEN__
            emscripten_webgl_enable_extension(
                emscripten_webgl_get_current_context(),
                "WEBGL_debug_renderer_info");
#endif
        }

        void printContext()
        {
            std::cout << "[OpenGL] " << glGetString(GL_VERSION) << std::endl;
            std::cout << "[Vendor] " << glGetString(GL_VENDOR) << std::endl;
            std::cout << "[3D Renderer] " << glGetString(GL_RENDERER) << std::endl;
            std::cout << "[GLSL] " << glGetString(GL_SHADING_LANGUAGE_VERSION) << std::endl;
            //std::cout << "[Extensions] " << glGetString(GL_EXTENSIONS) << std::endl;

#ifdef __EMSCRIPTEN__
            std::cout << "[WEBGL Vendor] " << glGetString(0x9245) << std::endl;
            std::cout << "[WEBGL Renderer] " << glGetString(0x9246) << std::endl;
#endif
        }

        GLuint compileProgram(const char* vertexSource, const char* fragmentSource)
        {
            // Create and compile the vertex shader
            GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
            glShaderSource(vertexShader, 1, &vertexSource, NULL);
            glCompileShader(vertexShader);

            GLint Result = GL_FALSE;
            int InfoLogLength;

            // Check Vertex Shader
            glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &Result);
            glGetShaderiv(vertexShader, GL_INFO_LOG_LENGTH, &InfoLogLength);
            if (InfoLogLength > 1) {
                std::vector<char> VertexShaderErrorMessage(InfoLogLength + 1);
                glGetShaderInfoLog(vertexShader, InfoLogLength, NULL, &VertexShaderErrorMessage[0]);
                printf("vertex: %s\n", &VertexShaderErrorMessage[0]);
            }

            // Create and compile the fragment shader
            GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
            glShaderSource(fragmentShader, 1, &fragmentSource, NULL);
            glCompileShader(fragmentShader);

            // Check Fragment Shader
            glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &Result);
            glGetShaderiv(fragmentShader, GL_INFO_LOG_LENGTH, &InfoLogLength);
            if (InfoLogLength > 1) {
                std::vector<char> FragmentShaderErrorMessage(InfoLogLength + 1);
                glGetShaderInfoLog(fragmentShader, InfoLogLength, NULL, &FragmentShaderErrorMessage[0]);
                printf("fragment: %s\n", &FragmentShaderErrorMessage[0]);
            }

            // Link the vertex and fragment shader into a shader program
            GLuint program = glCreateProgram();
            glAttachShader(program, vertexShader);
            glAttachShader(program, fragmentShader);
            //glBindFragDataLocation(shaderProgram, 0, "outColor");
            glLinkProgram(program);

            // Check the program
            glGetProgramiv(program, GL_LINK_STATUS, &Result);
            glGetProgramiv(program, GL_INFO_LOG_LENGTH, &InfoLogLength);
            if (InfoLogLength > 1) {
                std::vector<char> ProgramErrorMessage(InfoLogLength + 1);
                glGetProgramInfoLog(program, InfoLogLength, NULL, &ProgramErrorMessage[0]);
                printf("program: %s\n", &ProgramErrorMessage[0]);
            }

            return program;
        }
    }
}