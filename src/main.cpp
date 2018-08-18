#include <iostream>
#include <exception>
#include <functional>
#include <vector>

#ifdef __EMSCRIPTEN__
    #include <emscripten.h>
#endif

#include <SDL.h>
#include <glm/gtc/matrix_transform.hpp>

#include "optional.hpp"

#include "GameLoop.hpp"
#include "Window.hpp"
#include "SDLEventQueue.hpp"

#include "opengl.hpp"

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

std::tuple<SDL_Renderer*, SDL_Surface*> createSurfaceRenderer(const isc::vec2<uint32_t>& size)
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

    SDL_Surface* surface = SDL_CreateRGBSurface(0, size.x, size.y, 32, rmask, gmask, bmask, amask);
    SDL_SetSurfaceBlendMode(surface, SDL_BLENDMODE_BLEND);

    SDL_Renderer* renderer = SDL_CreateSoftwareRenderer(surface);

    return { renderer, surface };
}

struct GameLoop
{
    isc::Window window;
    SDL_Renderer* renderer;
    SDL_Surface* surface;

    nonstd::optional<isc::vec2<float>> touchLocation;

    renderable quad;
    renderable triangle;
    renderable cube;

    GameLoop()
    {
        window.create("Pong", { 640, 480 });

        std::tie(renderer, surface) = createSurfaceRenderer(window.getSize());

        quad = prepareQuad();
        triangle = prepareTriangle();
        cube = prepareCube();
    }

    void init()
    {
        SDL_RendererInfo rendererInfo;
        SDL_GetRendererInfo(renderer, &rendererInfo);

        opengl::printContext();
        std::cout << "[2D Renderer] SDL2 " << rendererInfo.name << std::endl;
    }

    ~GameLoop()
    {
        SDL_FreeSurface(surface);
        SDL_DestroyRenderer(renderer);

        SDL_Quit();

        std::cout << "[GameLoop] End" << std::endl;
    }

    bool loop(DeltaTime deltaTime)
    {
        SDL_Event event;

        while (isc::sdl::EventQueue::poll(event))
        {
            window.handleEvent(event);

            switch (event.type)
            {
                case SDL_QUIT:
                {
                    return false;
                }

                case SDL_KEYDOWN:
                {
                    if (event.key.keysym.sym == SDLK_ESCAPE)
                    {
                        return false;
                    }

                    break;
                }

                case SDL_FINGERDOWN:
                case SDL_FINGERMOTION:
                case SDL_FINGERUP:
                {
                    touchLocation = isc::vec2<float>(event.tfinger.x, event.tfinger.y);
                    break;
                }

                case SDL_WINDOWEVENT:
                {
                    const auto& windowEvent = event.window;

                    if (windowEvent.event == SDL_WINDOWEVENT_SIZE_CHANGED)
                    {
                        const auto& windowSize = window.getSize();

                        SDL_FreeSurface(surface);
                        SDL_DestroyRenderer(renderer);
                        std::tie(renderer, surface) = createSurfaceRenderer(windowSize);

                        std::cout << "Resize: [" << windowSize.x << "," << windowSize.y << "]" << std::endl;
                    }

                    break;
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
            (float)window.getSize().x / (float)window.getSize().y,
            0.01f, 1000.0f);

        isc::vec2<int32_t> mouse;
        SDL_GetMouseState(&mouse.x, &mouse.y);

        isc::vec2<float> input = touchLocation.has_value()
            ? touchLocation.value()
            : (isc::vec2<float>(mouse) / isc::vec2<float>(window.getSize()));

        glm::mat4 View = glm::lookAt(
            glm::vec3(
                20 * input.x - 10,
                20 * input.y - 10,
                -5),
            glm::vec3(0, 0, 0), // and looks at the origin
            glm::vec3(0, 1, 0)  // Head is up (set to 0,-1,0 to look upside-down)
        );

        glm::mat4 Model = glm::mat4(1.0f);
        glm::mat4 mvp = Projection * View * Model;

        GL(glUseProgram(cube.program));

        GLint matrixID = GL(glGetUniformLocation(cube.program, "MVP"));
        GL(glUniformMatrix4fv(matrixID, 1, GL_FALSE, &mvp[0][0]));

        new2dLayer();
        cube.render(true);

        // 2D rendering
        /////////////////////////////////////////////////////////////////////////////////////////

        SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
        SDL_RenderDrawLine(renderer, 0, 0, window.getSize().x, window.getSize().y);

        SDL_SetRenderDrawColor(renderer, 255, 215, 0, 255);
        SDL_RenderDrawLine(renderer, 0, window.getSize().y, window.getSize().x, 0);

        usingSurfaceTexture(surface, [&]()
        {
            new2dLayer();
            quad.render();
        });

        // swap buffers
        GL_CHECK();
        window.swap();

        return true;
    }
};

int main(int argc, char** argv)
{
    return initGameLoop<GameLoop>();
}