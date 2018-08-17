#include <iostream>
#include <exception>
#include <functional>
#include <vector>

#ifdef __EMSCRIPTEN__
    #include <emscripten.h>
#endif

#include <SDL.h>
#include <glm/gtc/matrix_transform.hpp>

#include "opengl.hpp"

#define SCREEN_WIDTH 640
#define SCREEN_HEIGHT 480

#ifdef __EMSCRIPTEN__
    namespace emscripten
    {
        using LoopPtr = void(*)(void*);

        template<typename TLambda>
        LoopPtr getLoopPtr(const TLambda&)
        {
            static LoopPtr executor = [](void* lambda)
            {
                (*static_cast<TLambda*>(lambda))();
            };

            return executor;
        }
    }
#endif

template<typename TGameLoop, typename TOnExit>
int runGameLoop(const TGameLoop& loop, const TOnExit& onExit)
{
#ifdef __EMSCRIPTEN__
    auto emscriptenLoop = [&]()
    {
        if (!loop())
        {
            emscripten_cancel_main_loop();
            onExit();
        }
    };

    emscripten_set_main_loop_arg(
        emscripten::getLoopPtr(emscriptenLoop),
        &emscriptenLoop,
        0, true);
#else
    while (loop()); onExit();
#endif

    return 0;
}

renderable prepareQuad()
{
    renderable quad;

    // Create Vertex Array Object
    glGenVertexArrays(1, &quad.vao);
    glBindVertexArray(quad.vao);

    // Create a Vertex Buffer Object and copy the vertex data to it
    glGenBuffers(1, &quad.vbo);

    GLfloat vertices[] = {
        -1.f, -1.f, 1.f, -1.f, -1.f, 1.f,
        -1.f, 1.f, 1.f, -1.f, 1.f, 1.f
    };

    glBindBuffer(GL_ARRAY_BUFFER, quad.vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    quad.count = 2;

    const GLchar* vertexSource = "#version 300 es\n" R"SHADER_END(

        in vec2 position;
        out vec2 UV;

        void main()
        {
            UV = (position.xy + vec2(1, 1)) / 2.0;
            UV.y = -UV.y; // SDL software uses DirectX coordinate system

            gl_Position = vec4(position.xy, 0.0, 1.0);
        }

    )SHADER_END";

    const GLchar* fragmentSource = "#version 300 es\n" R"SHADER_END(

        precision mediump float;
        out vec4 color;

        in vec2 UV;
        uniform sampler2D diffuseTexture;

        void main()
        {
            color = texture(diffuseTexture, UV);
        }

    )SHADER_END";

    quad.program = opengl::compileProgram(vertexSource, fragmentSource);

    glUseProgram(quad.program);

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
    glGenVertexArrays(1, &triangle.vao);
    glBindVertexArray(triangle.vao);

    // Create a Vertex Buffer Object and copy the vertex data to it
    glGenBuffers(1, &triangle.vbo);

    GLfloat vertices[] = { 0.0f, 0.5f, -0.5f, -0.5f, 0.5f, -0.5f };

    glBindBuffer(GL_ARRAY_BUFFER, triangle.vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    triangle.count = 1;

    const GLchar* vertexSource = "#version 300 es\n" R"SHADER_END(

        in vec2 position;
        out vec4 color;

        void main()
        {
            gl_Position = vec4(position.xy, 0.0, 1.0);
        }

    )SHADER_END";

    const GLchar* fragmentSource = "#version 300 es\n" R"SHADER_END(

        precision mediump float;
        out vec4 color;

        void main()
        {
            color = vec4(0.4, 0.8, 0.2, 1.0);
        }

    )SHADER_END";

    triangle.program = opengl::compileProgram(vertexSource, fragmentSource);

    glUseProgram(triangle.program);

    // Specify the layout of the vertex data
    GLint posAttrib = glGetAttribLocation(triangle.program, "position");
    glEnableVertexAttribArray(posAttrib);
    glVertexAttribPointer(posAttrib, 2, GL_FLOAT, GL_FALSE, 0, 0);

    return triangle;
}

renderable prepareCube()
{
    renderable cube;

    // VAO
    glGenVertexArrays(1, &cube.vao);
    glBindVertexArray(cube.vao);

    // VBO
    GLfloat vertices[] = {
        0.f, 1.f, 0.f,
        -1.f, 0.f, -1.f,
        1.f, 0.f, -1.f,
        0.f, 0.f, 1.f,
    };

    glGenBuffers(1, &cube.vbo);
    glBindBuffer(GL_ARRAY_BUFFER, cube.vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), &vertices[0], GL_STATIC_DRAW);

    // INDEX
    GLuint indices[] = {
        1, 0, 2,
        2, 0, 3,
        3, 0, 1,
        2, 3, 1,
    };

    glGenBuffers(1, &cube.index);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, cube.index);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), &indices[0], GL_STATIC_DRAW);

    cube.count = 4;

    const GLchar* vertexSource = "#version 300 es\n" R"SHADER_END(

        in vec3 position;
        uniform mat4 MVP;

        void main()
        {
            gl_Position = MVP * vec4(position, 1.0);
        }

    )SHADER_END";

    const GLchar* fragmentSource = "#version 300 es\n" R"SHADER_END(

        precision mediump float;
        out vec4 color;

        void main()
        {
            color = vec4(0.0, 0.0, 0.0, 1.0);
        }

    )SHADER_END";

    cube.program = opengl::compileProgram(vertexSource, fragmentSource);

    glUseProgram(cube.program);

    // Specify the layout of the vertex data
    GLint posAttrib = glGetAttribLocation(cube.program, "position");
    glEnableVertexAttribArray(posAttrib);
    glVertexAttribPointer(posAttrib, 3, GL_FLOAT, GL_FALSE, sizeof(float), 0);

    return cube;
}

std::tuple<SDL_Renderer*, SDL_Surface*> createSurfaceRenderer()
{
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

    SDL_Renderer* renderer = SDL_CreateSoftwareRenderer(surface);

    SDL_RendererInfo rendererInfo;
    SDL_GetRendererInfo(renderer, &rendererInfo);

    std::cout << "[2D Renderer] SDL2 " << rendererInfo.name << std::endl;

    return { renderer, surface };
}

int main(int argc, char** argv)
{
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

    opengl::link();
    opengl::printContext();

    glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);

    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glFrontFace(GL_CCW);

    SDL_GL_SetSwapInterval(0);

    SDL_Renderer* renderer;
    SDL_Surface* surface;

    std::tie(renderer, surface) = createSurfaceRenderer();

    renderable quad = prepareQuad();
    renderable triangle = prepareTriangle();
    renderable cube = prepareCube();

    const auto loop = [&]()
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

                case SDL_KEYDOWN:
                {
                    if (e.key.keysym.sym == SDLK_ESCAPE)
                    {
                        return false;
                    }
                }

                default: break;
            }
        }

        // Clear the screen
        /////////////////////////////////////////////////////////////////////////////////////////

        GL(glClearColor(0.f, 0x33 / 255.f, 0x66 / 255.f, 1.f));
        GL(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));

        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0); // transparent overlay
        SDL_RenderClear(renderer);

        // 3D rendering
        /////////////////////////////////////////////////////////////////////////////////////////

        triangle.render();

        glm::mat4 Projection = glm::perspective(
            glm::radians(45.0f),
            (float)SCREEN_WIDTH / (float)SCREEN_HEIGHT,
            0.01f, 1000.0f);

        int mouseX, mouseY;
        SDL_GetMouseState(&mouseX, &mouseY);

        glm::mat4 View = glm::lookAt(
            glm::vec3(
                20 * (mouseX / (float)SCREEN_WIDTH) - 10,
                20 * (mouseY / (float)SCREEN_HEIGHT) - 10,
                -5),
            glm::vec3(0, 0, 0), // and looks at the origin
            glm::vec3(0, 1, 0)  // Head is up (set to 0,-1,0 to look upside-down)
        );

        glm::mat4 Model = glm::mat4(1.0f);
        glm::mat4 mvp = Projection * View * Model;

        GL(glUseProgram(cube.program));

        GLint matrixID = GL(glGetUniformLocation(cube.program, "MVP"));
        GL(glUniformMatrix4fv(matrixID, 1, GL_FALSE, &mvp[0][0]));

        opengl::new2dLayer();
        cube.render(true);

        // 2D rendering
        /////////////////////////////////////////////////////////////////////////////////////////

        SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
        SDL_RenderDrawLine(renderer, 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);

        SDL_SetRenderDrawColor(renderer, 255, 215, 0, 255);
        SDL_RenderDrawLine(renderer, 0, SCREEN_HEIGHT, SCREEN_WIDTH, 0);

        opengl::usingSurfaceTexture(surface, [&]()
        {
            opengl::new2dLayer();
            quad.render();
        });

        // swap buffers
        GL_CHECK();
        SDL_GL_SwapWindow(window);

        return true;
    };

    const auto onExit = [&]()
    {
        SDL_FreeSurface(surface);
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);

        SDL_Quit();

        std::cout << "end" << std::endl;
    };

    return runGameLoop(loop, onExit);
}