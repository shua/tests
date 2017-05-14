#ifndef MESH_HPP
#define MESH_HPP

#include <array>
#include "utils.hpp"

class Mesh {
public:
    Mesh(GLuint* shader = NULL, glm::mat4* ProjMat = NULL, glm::mat4* ViewMat = NULL);
    ~Mesh();

    void GenIDs();
    void Draw();

    Mesh* vpush(float x, float y, float z, float w = 1);
    Mesh* ipush(float a, float b, float c);
    Mesh* cpush(float r, float g, float b, float a = 1);
    Mesh* uvpush(float u, float v);
    void setTexture(string filename);
    void setPtrs(GLuint* shader, glm::mat4* ProjMat, glm::mat4* ViewMat);

    GLuint VAO = 0;
    GLuint VBO = 0;
    GLuint IBO = 0;
    GLuint CBO = 0;
    GLuint TAO = 0;
    GLuint UBO = 0;

    GLuint mTexUniLoc;
    GLuint mModlMatUniLoc;
    glm::mat4 mModlMat;
private:
    std::vector<std::array<float, 4>> mVertices;
    std::vector<GLushort> mIndices;
    std::vector<std::array<float, 4>> mColors;
    std::vector<std::array<float, 2>> mUvs;
    unsigned int mUvDim[2];
    string texFile;

    GLuint* mShaderPtr;
    glm::mat4* mProjMatPtr;
    glm::mat4* mViewMatPtr;
};

#endif
