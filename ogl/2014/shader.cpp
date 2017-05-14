#include "shader.hpp"
#include "utils.hpp"

#include <vector>

Shader::Shader() {};

Shader::Shader(const char* vert, const char* frag) {
    string vert_s = OpenTextFile(vert);
    string frag_s = OpenTextFile(frag);

    mVert = glCreateShader(GL_VERTEX_SHADER);
    if(mVert == 0) { serrx("Vertex shader id is 0."); }
    mFrag = glCreateShader(GL_FRAGMENT_SHADER);
    if(mFrag == 0) { serrx("Fragment shader id is 0."); }
    errx("create shader");

    const char* vert_c = vert_s.c_str();
    const char* frag_c = frag_s.c_str();

    glShaderSource(mVert, 1, &vert_c, NULL);
    errx("set shader source");
    glCompileShader(mVert);
    PrintLog(mVert, GL_COMPILE_STATUS);
    errx("compile vertex shader");

    glShaderSource(mFrag, 1, &frag_c, NULL);
    glCompileShader(mFrag);
    PrintLog(mVert, GL_COMPILE_STATUS);
    errx("compile fragment shader");

    mProg = glCreateProgram();

    glAttachShader(mProg, mVert);
    glAttachShader(mProg, mFrag);
    glLinkProgram(mProg);
    PrintLog(mProg, GL_LINK_STATUS);
    errx("link the shader program");
}

Shader::~Shader() { 
    glUseProgram(0);
    glDetachShader(mProg, mVert);
    glDetachShader(mProg, mFrag);
    glDeleteShader(mVert);
    glDeleteShader(mFrag);
    glDeleteProgram(mProg);
}

void Shader::PrintLog(unsigned int ID, unsigned int check) {
    GLint Result = GL_FALSE;
    int InfoLogLength;

    if(ID == mProg) {
        glGetProgramiv(ID, check, &Result);
        glGetProgramiv(ID, GL_INFO_LOG_LENGTH, &InfoLogLength);
        if(InfoLogLength > 1) {
            std::vector<char> ProgramErrorMessage( std::max(InfoLogLength, int(1)) );
            glGetProgramInfoLog(ID, InfoLogLength, NULL, &ProgramErrorMessage[0]);
            std::cout << "Shader Program Log:\n" << &ProgramErrorMessage[0] << std::endl;
        }
    } else {
        glGetShaderiv(ID, check, &Result);
        glGetShaderiv(ID, GL_INFO_LOG_LENGTH, &InfoLogLength);
        if(InfoLogLength > 1) {
            std::vector<char> ErrorMessage(InfoLogLength);
            glGetShaderInfoLog(ID, InfoLogLength, NULL, &ErrorMessage[0]);
            std::cout << "Shader Log:\n" << &ErrorMessage[0] << std::endl;
        }
    }
}
