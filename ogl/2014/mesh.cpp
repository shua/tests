#include "mesh.hpp"

Mesh::Mesh(GLuint* shader, glm::mat4* ProjMat, glm::mat4* ViewMat) {
    setPtrs(shader, ProjMat, ViewMat);
}

Mesh::~Mesh() {
    glDisableVertexAttribArray(2);
    glDisableVertexAttribArray(1);
    glDisableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    if(VBO != 0) {
        glDeleteBuffers(1, &VBO);
    }
    if(CBO != 0) {
        glDeleteBuffers(1, &CBO);
    }
    if(TAO != 0) {
        glDeleteTextures(1, &TAO);
    }
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    if(IBO != 0) {
        glDeleteBuffers(1, &IBO);
    }
    glBindVertexArray(0);
    if(VAO != 0) {
        glDeleteVertexArrays(1, &VAO);
    }
}

void Mesh::GenIDs() {
    if(mVertices.size() != 0 && VAO == 0) {
        std::cout << "Generating VAO" << std::endl;
        glGenVertexArrays(1, &VAO);
        errx("generate the VAO");
        glBindVertexArray(VAO);
        errx("bind the VAO");

        glGenBuffers(1, &VBO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(float) * mVertices.size() * 4, &mVertices[0], GL_STATIC_DRAW);
        errx("load vertex buffers");

        glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(mVertices[0]), 0);
        glEnableVertexAttribArray(0);
        errx("applying vertex attributes");
    }
    if(mIndices.size() != 0 && IBO == 0) {
        std::cout << "Generating IBO" << std::endl;
        glGenBuffers(1, &IBO);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, mIndices.size() * sizeof(GLushort), &mIndices[0], GL_STATIC_DRAW);
        errx("load the index buffer");
    }
    if(mColors.size() !=  0 && CBO == 0) {
        std::cout << "Generating CBO" << std::endl;
        glGenBuffers(1, &CBO);
        glBindBuffer(GL_ARRAY_BUFFER, CBO);
        glBufferData(GL_ARRAY_BUFFER, mColors.size() * sizeof(float) * 4, &mColors[0], GL_STATIC_DRAW);
        errx("load color buffer");

        glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(mColors[0]), 0);
        glEnableVertexAttribArray(1);
        errx("applying color attributes");
    }
    if(mUvs.size() != 0 && TAO == 0) {
        std::cout << "Generating TAO" << std::endl;
        glGenTextures(1, &TAO);
        glBindTexture(GL_TEXTURE_2D, TAO);

        std::cout << "tex dim: " << mUvDim[0] << ", " << mUvDim[1] << std::endl;
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, mUvDim[0], mUvDim[1], 0, GL_RGB, GL_UNSIGNED_BYTE, texFile.c_str());
        errx("gen tex 1");

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        errx("generate texture");

        glGenBuffers(1, &UBO);
        glBindBuffer(GL_ARRAY_BUFFER, UBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(float) * mUvs.size() * 2, &mUvs[0], GL_STATIC_DRAW);

        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(mUvs[0]), 0);
        glEnableVertexAttribArray(2);
        errx("applying texture attribute");

        if(mShaderPtr != NULL) {
            mTexUniLoc = glGetUniformLocation(*mShaderPtr, "TextureSampler");
        }
    }
}

void Mesh::Draw() {
    glUseProgram(*mShaderPtr);
    glm::mat4 MVP = (*mProjMatPtr) * (*mViewMatPtr) * mModlMat;
    glUniformMatrix4fv(mModlMatUniLoc, 1, GL_FALSE, &MVP[0][0]);
    if(UBO != 0) {
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, TAO);
        glUniform1i(mTexUniLoc, 0);
    }
    glBindVertexArray(VAO);
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(mVertices[0]), 0);
    if(UBO != 0) {
        std::cout << "Drawing: UVs" << std::endl;
        glEnableVertexAttribArray(2);
        glBindBuffer(GL_ARRAY_BUFFER, UBO);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(mUvs[0]), 0);
    }else if(CBO != 0) {
        std::cout << "Drawing Colors" << std::endl;
        glEnableVertexAttribArray(1);
        glBindBuffer(GL_ARRAY_BUFFER, CBO);
        glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(mColors[0]), 0);
    }
    if(mIndices.size() != 0) {
        //std::cout << "indices" << std::endl;
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);
        glDrawElements(GL_TRIANGLES, mIndices.size(), GL_UNSIGNED_SHORT, 0);
    } else {
        glDrawArrays(GL_TRIANGLES, 0, mVertices.size());
    }
    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);
    glDisableVertexAttribArray(2);
}

Mesh* Mesh::vpush(float x, float y, float z, float w) {
    std::array<float, 4> temp = {x, y, z, w};
    mVertices.push_back(temp);
    return this;
}

Mesh* Mesh::ipush(float a, float b, float c) {
    mIndices.push_back(a);
    mIndices.push_back(b);
    mIndices.push_back(c);
    return this;
}

Mesh* Mesh::cpush(float r, float g, float b, float a) {
    std::array<float, 4> temp = {r, g, b, a};
    mColors.push_back(temp);
    return this;
}

Mesh* Mesh::uvpush(float u, float v) {
    std::array<float, 2> temp= {u, v};
    mUvs.push_back(temp);
    return this;
}

void Mesh::setTexture(string filename) {
    //texFile = OpenPNGFile(filename, mUvDim[0], mUvDim[1]);
    
    texFile = OpenTextFile(filename);
    mUvDim[0] = mUvDim[1] = 256;
}

void Mesh::setPtrs(GLuint* shader, glm::mat4* ProjMat, glm::mat4* ViewMat) {
    if(shader != NULL) {
        mShaderPtr = shader;
        mModlMatUniLoc = glGetUniformLocation(*mShaderPtr, "ModelMatrix");
        errx("get the model's uniform location");
        if(UBO != 0) {
            mTexUniLoc = glGetUniformLocation(*mShaderPtr, "TextureSampler");
        }
    }
    mProjMatPtr = (ProjMat == NULL) ? mProjMatPtr : ProjMat;
    mViewMatPtr = (ViewMat == NULL) ? mViewMatPtr : ViewMat;
}
