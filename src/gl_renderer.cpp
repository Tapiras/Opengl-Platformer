#include "gl_renderer.h"

//To load PNG files
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

// --------------------------------------------------------------------------------------------------
//                                      OpenGl Constants
// --------------------------------------------------------------------------------------------------
const char* TEXTURE_PATH = "assets\\Textures\\Texture_Atlas.png";


// --------------------------------------------------------------------------------------------------
//                                      OpenGl Structs
// --------------------------------------------------------------------------------------------------
struct GLContext
{
    GLuint programId;
    GLuint textureID;
};

// --------------------------------------------------------------------------------------------------
//                                      OpenGL Globals
// --------------------------------------------------------------------------------------------------
static GLContext glContext;

// --------------------------------------------------------------------------------------------------
//                                      Opengl Functions                                      
// --------------------------------------------------------------------------------------------------
static void APIENTRY gl_debug_callback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* userParam)
{
    if (severity == GL_DEBUG_SEVERITY_LOW       ||
        severity == GL_DEBUG_SEVERITY_MEDIUM    ||
        severity == GL_DEBUG_SEVERITY_HIGH          )
    {
        SM_ASSERT(false, "OpenGL Error: %s", message);
    }
    else
    {
        SM_TRACE((char*)message);
    }   
}        

bool gl_init(BumpAllocator* transientStorage)
{
    load_gl_functions();

    glDebugMessageCallback(&gl_debug_callback, nullptr);
    glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
    glEnable(GL_DEBUG_OUTPUT);

    GLuint vertShaderID = glCreateShader(GL_VERTEX_SHADER);
    GLuint fragShaderID = glCreateShader(GL_FRAGMENT_SHADER);

    int fileSize = 0;
    char* vertShader = read_file("assets/shaders/quad.vert", &fileSize, transientStorage);
    char* fragShader = read_file("assets/shaders/quad.frag", &fileSize, transientStorage);
    if (!vertShader || !fragShader)
    {
        SM_ASSERT(false, "Failed to load shaders");
        return false;
    }

    glShaderSource(vertShaderID, 1, &vertShader, 0);
    glShaderSource(fragShaderID, 1, &fragShader, 0);

    glCompileShader(vertShaderID);
    glCompileShader(fragShaderID);

    //  Test of shaders compiles successfully
    {
        int success = 0;
        char shaderLog[2048] = {};

        glGetShaderiv(vertShaderID, GL_COMPILE_STATUS, &success);
        if (!success)
        {
            glGetShaderInfoLog(vertShaderID, 2048, 0, shaderLog);
            SM_ASSERT(false, "Failed to compile vertex shader: %s", shaderLog);
        }
    }
    // Test if fragment shader compiles successfully
    {
        int success = 0;
        char shaderLog[2048] = {};

        glGetShaderiv(fragShaderID, GL_COMPILE_STATUS, &success);
        if (!success)
        {
            glGetShaderInfoLog(fragShaderID, 512, 0, shaderLog);
            SM_ASSERT(false, "Failed to compile fragment shader: %s", shaderLog);
        }
    }

    glContext.programId = glCreateProgram();
    glAttachShader(glContext.programId, vertShaderID);
    glAttachShader(glContext.programId, fragShaderID);
    glLinkProgram(glContext.programId);

    glDetachShader(glContext.programId, vertShaderID);
    glDetachShader(glContext.programId, fragShaderID);
    glDeleteShader(vertShaderID);
    glDeleteShader(fragShaderID);

    // this has to be done, otherwise OpenGL will not draw
    GLuint VAO;
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    // Texture Loading using STBI
    {
        int width, height, channels;
        char* data = (char*)stbi_load(TEXTURE_PATH, &width, &height, &channels, 4);
        if(!data)
        {
            SM_ASSERT(false, "Failed to load texture");
            return false;
        }

        glGenTextures(1, &glContext.textureID);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, glContext.textureID);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        glTexImage2D(GL_TEXTURE_2D, 0, GL_SRGB8_ALPHA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);

        stbi_image_free(data);
    }

    glEnable(GL_FRAMEBUFFER_SRGB);
    glDisable(0x809D);

    // Depth testing
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_GREATER);

    glUseProgram(glContext.programId);

    return true;
}

void gl_render()
{
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClearDepth(0.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glViewport(0, 0, input.screenSizeX, input.screenSizeY);

    glDrawArrays(GL_TRIANGLES, 0, 6);
}