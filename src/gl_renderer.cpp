#include "gl_renderer.h"

// --------------------------------------------------------------------------------------------------
//                                      OpenGl Structs
// --------------------------------------------------------------------------------------------------
struct GLContext
{
    GLuint programId;
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