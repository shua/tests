#ifndef SHADER_HPP
#define SHADER_HPP

class Shader {
public:
    Shader();
    Shader(const char* vert, const char* frag);
    ~Shader();
    void PrintLog(unsigned int ID, unsigned int check);

    unsigned int mVert;
    unsigned int mFrag;
    unsigned int mProg;
};

#endif
