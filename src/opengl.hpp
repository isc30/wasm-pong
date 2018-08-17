#pragma once

#ifdef __EMSCRIPTEN__
    #include <emscripten.h>
    #include <GLES3/gl3.h>
    #include <emscripten/html5.h>
#else
    #include <glad/glad.h> // OpenGL ES 3.0
#endif

#include <string>
#include <SDL.h>

namespace opengl
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
            , message("OpenGL error " + std::to_string(errorCode) + " `"
                + errorDescription + "` @ " + file + ":" + std::to_string(line)
                + (call.size() > 0 ? " - `" + call + "`" : ""))
        {
        }

        const char* what() const noexcept override
        {
            return message.c_str();
        }
    };

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
}

#define GL(glCall) glCall; do {} while (opengl::checkError(__FILE__, __LINE__, #glCall))
#define GL_CHECK() do {} while (opengl::checkError(__FILE__, __LINE__, "GL_CHECK()"))

struct renderable
{
    GLuint program = 0;
    GLuint vao = 0, vbo = 0, index = 0;
    GLsizei count = 0;

    void render(bool wireframe = false) const
    {
        GLenum mode = wireframe ? GL_LINE_LOOP : GL_TRIANGLES;
        GLsizei verticesCount = 3 * count;

        GL(glUseProgram(program));
        GL(glBindVertexArray(vao));

        if (index == 0)
        {
            GL(glBindBuffer(GL_ARRAY_BUFFER, vbo));
            GL(glDrawArrays(mode, 0, verticesCount));
            GL(glBindBuffer(GL_ARRAY_BUFFER, 0));
        }
        else
        {
            GL(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, index));
            GL(glDrawElements(mode, verticesCount, GL_UNSIGNED_INT, nullptr));
            GL(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0));
        }

        GL(glBindVertexArray(0));
        GL(glUseProgram(0));
    }
};

namespace opengl
{
    void link()
    {
#ifndef __EMSCRIPTEN__
        if (!gladLoadGLES2Loader(SDL_GL_GetProcAddress))
        {
            std::cout << "Error initializing OpenGL ES 3.0" << std::endl;
            std::abort();
        }
#else
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

    template<typename CRender>
    void usingSurfaceTexture(const SDL_Surface* surface, const CRender& render)
    {
        GLuint TextureID = 0;
        GL(glGenTextures(1, &TextureID));
        GL(glBindTexture(GL_TEXTURE_2D, TextureID));

        int Mode = surface->format->BytesPerPixel == 4
            ? GL_RGBA
            : GL_RGB;

        GL(glTexImage2D(GL_TEXTURE_2D, 0, Mode, surface->w, surface->h, 0, Mode, GL_UNSIGNED_BYTE, surface->pixels));

        GL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
        GL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));

        GL(glActiveTexture(GL_TEXTURE0 + 0));
        GL(glBindTexture(GL_TEXTURE_2D, TextureID));

        render();

        GL(glBindTexture(GL_TEXTURE_2D, 0));

        GL(glDeleteTextures(1, &TextureID));
    }

    inline void new2dLayer()
    {
        GL(glClear(GL_DEPTH_BUFFER_BIT));
    }
}
