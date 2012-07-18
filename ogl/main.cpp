
#include "utils.hpp"
#include "shader.hpp"
#include "mesh.hpp"

GLFWwindow window;

static const float PI = 3.1415926535;

int wWidth = 300, wHeight = 300;
bool running = true;
double oldTime = 0.f;
float deltaTime = 0.f;
float FoV = 45.0f;
float horzAngle = PI;
float vertAngle = 0.0f;
float speed = 5.f;
float mouseSpeed = 0.2f;

GLuint BufferIds[4];

glm::vec3 move(0, 0, 2);

glm::mat4   ViewMat = glm::lookAt(move, glm::vec3(0,0,1), glm::vec3(0,1,0)),
            ModlMat,
            ProjMat;
GLuint ModlMatUniLoc;

Shader* shader = NULL;
Shader* uvshader = NULL;
Mesh* mesh = NULL;
Mesh* mesh2 = NULL;

void WinInit(void);
void Init(void);
void Draw(void);
void Term(void);

void ResizeFunc(GLFWwindow, int, int);
void KeyboardFunc(GLFWwindow, int, int);

int main(int argc, char* argv[]) {
    Init();
    Draw();
    Term();
}

void Init() {
    WinInit();
    glfwSetCursorPos(window, wWidth/2, wHeight/2);

    shader = new Shader("vert.glsl", "frag.glsl");
    uvshader = new Shader("vert.uv.glsl", "frag.uv.glsl");
    
    mesh = new Mesh(&(uvshader->mProg), &ProjMat, &ViewMat);
    mesh->vpush(.5f,-.5f,-.5f)->vpush(-.5f,-.5f,-.5f)->vpush(.5f, .5f,-.5f)->vpush(-.5f, .5f,-.5f);
    mesh->vpush(.6f,-.6f, .5f)->vpush(-.6f,-.6f, .5f)->vpush(.6f, .6f, .5f)->vpush(-.6f, .6f, .5f);

    mesh->cpush(1, 0, 0)->cpush(0, 1, 0)->cpush(0, 0, 1)->cpush(0, 0, 0);
    mesh->cpush(1, 1, 0)->cpush(1, 0, 1)->cpush(0, 1, 1)->cpush(1, 1, 1);

    mesh->ipush(4, 6, 5)->ipush(6, 7, 5)->ipush(0, 1, 2)->ipush(2, 1, 3); // front; back
    mesh->ipush(0, 2, 4)->ipush(4, 2, 6)->ipush(1, 5, 3)->ipush(5, 7, 3); // right; left
    mesh->ipush(2, 3, 6)->ipush(3, 7, 6)->ipush(0, 4, 1)->ipush(1, 4, 5); // top; bottom
/*
    mesh->uvpush(.33f, .67f)->uvpush(.33f, 1.f)->uvpush(0.f, .67f)->uvpush(.33f, 1.f)->uvpush(0.f, 1.f)->uvpush(0.f, .67f); // front
    mesh->uvpush(.67f, 1.f)->uvpush(.33f, 1.f)->uvpush(.67f, .67f)->uvpush(.67f, .67f)->uvpush(.33f, 1.f)->uvpush(.33f, .67f); // back
    mesh->uvpush(.67f, .67f)->uvpush(1.f, .67f)->uvpush(.67f, 1.f)->uvpush(.67f, 1.f)->uvpush(1.f, .67f)->uvpush(1.f, 1.f); // right
    mesh->uvpush(.33f, .33f)->uvpush(.33f, .67f)->uvpush(0.f, .33f)->uvpush(.33f, .67f)->uvpush(0.f, .67f)->uvpush(0.f, .33f); // left 
    mesh->uvpush(.67f, .67f)->uvpush(.33f, .67f)->uvpush(.67f, .33f)->uvpush(.33f, .67f)->uvpush(.33f, .33f)->uvpush(.67f, .33f); // top
    mesh->uvpush(1.f, .33f)->uvpush(1.f, .67f)->uvpush(.67f, .33f)->uvpush(.67f, .33f)->uvpush(1.f, .67f)->uvpush(.67f, .67f); // bottom
*/
    for(int i = 0; i < 12; i++) {
        mesh->uvpush(0.f, 1.f)->uvpush(0.f, 0.f)->uvpush(1.f, 1.f);
    }
    mesh->setTexture("texture.raw");


    mesh->mModlMat = glm::translate(mesh->mModlMat, glm::vec3(0, 0, -2));
    mesh->GenIDs();

    mesh2 = new Mesh(&(shader->mProg), &ProjMat, &ViewMat);
    mesh2->vpush(0.f, 0.f, -.5f)->vpush(-.5f, 0.f, .5f)->vpush(.5f, 0.f, .5f)->vpush(0.f, 1.f, 0.f);
    mesh2->cpush(1, 0, 0)->cpush(0, 1, 0)->cpush(0, 0, 1)->cpush(.5, .5, .5);
    mesh2->ipush(0, 2, 1)->ipush(2, 3, 1)->ipush(0, 3, 2)->ipush(1, 3, 0);
//    for(int i = 0; i < 4; i++) { mesh2->uvpush(0.f, 1.f)->uvpush(0.f, 0.f)->uvpush(1.f, 1.f); }
//    mesh2->setTexture("texture.png");

    mesh2->mModlMat = glm::translate(mesh2->mModlMat, glm::vec3(0, 0, 4));
    mesh2->GenIDs();

}

void Draw() {
    while(running){
        double currentTime = glfwGetTime();
        deltaTime = float(currentTime - oldTime);
        oldTime = currentTime;
        
        int xpos, ypos;
        glfwGetCursorPos(window, &xpos, &ypos);
        glfwSetCursorPos(window, wWidth/2, wHeight/2);
        horzAngle += (wWidth/2 - xpos) * deltaTime * mouseSpeed;
        while(horzAngle > 2 * PI) { horzAngle -= (2 * PI); }
        while(horzAngle < 0) { horzAngle += (2 * PI); }
        vertAngle += (wHeight/2 - ypos) * deltaTime * mouseSpeed;
        if(vertAngle >= 1.57) {vertAngle = 1.56;}
        else if(vertAngle <= -1.57) {vertAngle = -1.56;}
        glm::vec3 direction(
            cos(vertAngle) * sin(horzAngle),
            sin(vertAngle),
            cos(vertAngle) * cos(horzAngle));

        glm::vec3 right(sin(horzAngle - PI/2), 0, cos(horzAngle - PI/2));
        if(glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) {
            move += direction * deltaTime * speed;
        }
        if(glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) {
            move -= direction * deltaTime * speed;
        }
        if(glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS) {
            move += right * deltaTime * speed;
        }
        if(glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS) {
            move -= right * deltaTime * speed;
        }
        ViewMat = glm::lookAt(move, direction + move, glm::vec3(0, 1, 0));
        
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        //mesh2->Draw();
        mesh->Draw();
        //glBindVertexArray(mesh->VAO);
        //glBindBuffer(GL_ARRAY_BUFFER, mesh->VBO);
        //glDrawArrays(GL_TRIANGLES, 0, 3);
        errx("draw elements");
        glBindVertexArray(0);
        glUseProgram(0);

        glfwSwapBuffers();
        glfwPollEvents();
        running = glfwIsWindow(window) ? running : false;
    }
}

void Term() {
    delete mesh;
    errx("destroy mesh");
    
    delete shader;
    errx("destroy shader");

    glfwTerminate();
    exit(EXIT_SUCCESS);
}

void WinInit() {
    if(!glfwInit()) {
        serrx("glfwInit failed");
    }

    glfwOpenWindowHint(GLFW_OPENGL_VERSION_MAJOR, 3);
    glfwOpenWindowHint(GLFW_OPENGL_VERSION_MINOR, 3);
    glfwOpenWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);    
    glfwOpenWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_COMPAT_PROFILE);

    window = glfwOpenWindow(wWidth, wHeight, GLFW_WINDOWED, "Ya", NULL);
    if(!window) {
        glfwTerminate();
        serrx("Couldn't init glfw");
    }
    
    glewExperimental = GL_TRUE;
    if(glewInit() != GLEW_OK) {
        glfwTerminate();
        serrx("Couldn't init GLEW");
    }

    glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);
    glfwSetWindowSizeCallback(ResizeFunc);
    glfwSetKeyCallback(KeyboardFunc);
    glfwSwapInterval(1);

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    errx("set OpenGL depth testing options");

    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glFrontFace(GL_CCW);
    errx("set OpenGL culling options");

    glClearColor(0.2f, 0.2f, 0.2f, 0.f);
    return;
}

void ResizeFunc(GLFWwindow win, int w, int h) {
    h = (h > 0) ? h : 1;
    wWidth = w;
    wHeight = h;
    glViewport(0, 0, wWidth, wHeight);

    ProjMat = glm::perspective(
        60.f,
        (float)wWidth/wHeight,
        0.1f,
        100.0f);
    errx("resize window");
}

void KeyboardFunc(GLFWwindow win, int key, int action) {
    if(key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
        running = false;
    }
}
