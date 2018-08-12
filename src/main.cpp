#include <iostream>
#include <exception>
#include <functional>
#include <vector>

#ifdef __EMSCRIPTEN__
    #include <emscripten.h>

    #define GL_GLEXT_PROTOTYPES 1
    #include <SDL_opengles2.h>
#endif

#ifndef __EMSCRIPTEN__
    #include <glad/glad.h> // OpenGL ES 3.0
#endif

#include <SDL.h>

std::function<bool()> loop;
void main_loop() { loop(); }

void loadOpenGL()
{
#ifndef __EMSCRIPTEN__
    gladLoadGLES2Loader(SDL_GL_GetProcAddress);
#endif
}

void printContext()
{
    std::cout << "[OpenGL] " << glGetString(GL_VERSION) << std::endl;
    std::cout << "[Vendor] " << glGetString(GL_VENDOR) << std::endl;
    std::cout << "[3D Renderer] " << glGetString(GL_RENDERER) << std::endl;
    std::cout << "[GLSL] " << glGetString(GL_SHADING_LANGUAGE_VERSION) << std::endl;
    //std::cout << " [Extensions] " << glGetString(GL_EXTENSIONS) << std::endl;
}

struct renderable
{
    GLuint program;
    GLuint vao, vbo;
    GLsizei count;

    void render() const
    {
        glUseProgram(program);
            glBindVertexArrayOES(vao);
                glBindBuffer(GL_ARRAY_BUFFER, vbo);
                    glDrawArrays(GL_TRIANGLES, 0, 3 * count);
                glBindBuffer(GL_ARRAY_BUFFER, 0);
            glBindVertexArrayOES(0);
        glUseProgram(0);
    }
};

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
    // glBindFragDataLocation(shaderProgram, 0, "outColor");
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

renderable prepareQuad()
{
    renderable quad;

    // Create Vertex Array Object
    glGenVertexArraysOES(1, &quad.vao);
    glBindVertexArrayOES(quad.vao);

    // Create a Vertex Buffer Object and copy the vertex data to it
    glGenBuffers(1, &quad.vbo);

    GLfloat vertices[] = {
        -1.f, -1.f, 1.f, -1.f, -1.f, 1.f,
        -1.f, 1.f, 1.f, -1.f, 1.f, 1.f
    };

    glBindBuffer(GL_ARRAY_BUFFER, quad.vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    quad.count = 2;

    const GLchar* vertexSource =
        "#version 300 es\n"
        "in vec2 position;    \n"
        "out vec2 UV;\n"
        "void main()                  \n"
        "{                            \n"
        "   gl_Position = vec4(position.xy, 0.0, 1.0);  \n"
        "   UV = (position.xy + vec2(1, 1)) / 2.0;\n"
        "   UV.y = -UV.y;  \n" // SDL software used DirectX coordinate system
        "}                            \n";

    const GLchar* fragmentSource =
        "#version 300 es\n"
        "precision mediump float;\n"
        "out vec4 color;\n"
        "in vec2 UV;\n"
        "uniform sampler2D myTextureSampler;\n"
        "void main()                                  \n"
        "{                                            \n"
        "  color = texture( myTextureSampler, UV ).rgba;\n"
        //"  color = vec4(1, 0, 0, 1);\n"
        "}                                            \n";

    quad.program = compileProgram(vertexSource, fragmentSource);

    // Specify the layout of the vertex data
    GLint posAttrib = glGetAttribLocation(quad.program, "position");
    glEnableVertexAttribArray(posAttrib);
    glVertexAttribPointer(posAttrib, 2, GL_FLOAT, GL_FALSE, 0, 0);

    return quad;
}

renderable prepareTriangle()
{
    renderable triangle;

    // Create Vertex Array Object
    glGenVertexArraysOES(1, &triangle.vao);
    glBindVertexArrayOES(triangle.vao);

    // Create a Vertex Buffer Object and copy the vertex data to it
    glGenBuffers(1, &triangle.vbo);

    GLfloat vertices[] = { 0.0f, 0.5f, -0.5f, -0.5f, 0.5f, -0.5f };

    glBindBuffer(GL_ARRAY_BUFFER, triangle.vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    triangle.count = 1;

    const GLchar* vertexSource =
        "#version 300 es\n"
        "in vec2 position;    \n"
        "void main()                  \n"
        "{                            \n"
        "   gl_Position = vec4(position.xy, 0, 1.0);  \n"
        "}                            \n";

    const GLchar* fragmentSource =
        "#version 300 es\n"
        "precision mediump float;\n"
        "out vec4 color;\n"
        "void main()                                  \n"
        "{                                            \n"
        "  color = vec4 (0.4, 0.8, 0.2, 1.0 );\n"
        "}                                            \n";

    triangle.program = compileProgram(vertexSource, fragmentSource);

    // Specify the layout of the vertex data
    GLint posAttrib = glGetAttribLocation(triangle.program, "position");
    glEnableVertexAttribArray(posAttrib);
    glVertexAttribPointer(posAttrib, 2, GL_FLOAT, GL_FALSE, 0, 0);

    return triangle;
}

void renderOpenglSurface(const renderable& renderable, SDL_Surface* surface)
{
    GLuint TextureID = 0;
    glGenTextures(1, &TextureID);
    glBindTexture(GL_TEXTURE_2D, TextureID);

    int Mode = surface->format->BytesPerPixel == 4
        ? GL_RGBA
        : GL_RGB;

    glTexImage2D(GL_TEXTURE_2D, 0, Mode, surface->w, surface->h, 0, Mode, GL_UNSIGNED_BYTE, surface->pixels);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, TextureID);

    renderable.render(); // quad

    glBindTexture(GL_TEXTURE_2D, 0);

    glDeleteTextures(1, &TextureID);
}

int main(int argc, char** argv)
{
    #define SCREEN_WIDTH 640
    #define SCREEN_HEIGHT 480

    SDL_Window* window =
        SDL_CreateWindow("test", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
            SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN);

#ifndef __EMSCRIPTEN__
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
#endif

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_ES);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 32);

    SDL_GL_CreateContext(window);

    loadOpenGL();
    printContext();

    glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);

    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glFrontFace(GL_CCW);
	
    SDL_GL_SetSwapInterval(0);

    Uint32 rmask, gmask, bmask, amask;
#if SDL_BYTEORDER == SDL_BIG_ENDIAN
    rmask = 0xff000000;
    gmask = 0x00ff0000;
    bmask = 0x0000ff00;
    amask = 0x000000ff;
#else
    rmask = 0x000000ff;
    gmask = 0x0000ff00;
    bmask = 0x00ff0000;
    amask = 0xff000000;
#endif

    SDL_Surface* surface = SDL_CreateRGBSurface(0, SCREEN_WIDTH, SCREEN_HEIGHT, 32, rmask, gmask, bmask, amask);

    SDL_SetSurfaceBlendMode(surface, SDL_BLENDMODE_BLEND);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    SDL_Renderer* renderer = SDL_CreateSoftwareRenderer(surface);

    SDL_RendererInfo rendererInfo;
    SDL_GetRendererInfo(renderer, &rendererInfo);

    std::cout << "[2D Renderer] SDL2 " << rendererInfo.name << std::endl;

    renderable quad = prepareQuad();
    renderable triangle = prepareTriangle();

    loop = [&]
    {
        SDL_Event e;
        while (SDL_PollEvent(&e))
        {
            switch (e.type)
            {
                case SDL_QUIT:
                {
                    return false;
                }

                case SDL_MOUSEMOTION:
                {
                    int x, y; SDL_GetMouseState(&x, &y);
                    std::cout << x << ", " << y << std::endl;
                    break;
                }

                default: break;
            }
        }

        // Clear the screen
        glClearColor(0.f, 0x33 / 255.f, 0x66 / 255.f, 1.f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // 3D rendering
        /////////////////////////////////////////////////////////////////////////////////////////

        triangle.render();

        // 2D rendering
        /////////////////////////////////////////////////////////////////////////////////////////

        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0); // transparent overlay
        SDL_RenderClear(renderer);

        SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
        SDL_RenderDrawLine(renderer, 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);

        SDL_SetRenderDrawColor(renderer, 255, 215, 0, 255);
        SDL_RenderDrawLine(renderer, 0, SCREEN_HEIGHT, SCREEN_WIDTH, 0);

        renderOpenglSurface(quad, surface); // render 2D surface

        // swap buffers
        SDL_GL_SwapWindow(window);

        return true;
    };

#ifdef __EMSCRIPTEN__
    emscripten_set_main_loop(main_loop, 0, true);
#else
    while(true) main_loop();
#endif

    std::cout << "end";

    return 0;
}