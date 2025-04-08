#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <cmath>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <stack>
#include <SOIL2/SOIL2.h>
#include "module.h"
#include "KeyboardHandler.h"
#include "Sphere.h"
#include "Torus.h"
#include "ImportedModel.h"


using namespace std;

#define numVAOs 1
#define numVBOs 4


stack<glm::mat4> mvStack;
float cameraX, cameraY, cameraZ;
float cubeLocX, cubeLocY, cubeLocZ;
float pyrLocX, pyrLocY, pyrLocZ;
GLuint renderingProgram;
GLuint vao[numVAOs];
GLuint vbo[numVBOs];


//Sphere mySphere(1000);
Torus myTorus(0.5f, 0.2f, 48);
//ImportedModel myModel("untitled.obj");


//allocate variables used in display() function, so that they won't need to be allocated during rendering

GLuint projLoc, mvLoc;
GLuint mLoc, vLoc, pLoc, nLoc;
GLuint globalAmbLoc, ambLoc, diffLoc, specLoc, posLoc, mAmbLoc, mDiffLoc, mSpecLoc, mShiLoc;
int width, height;
float aspect;
float tf;
glm::mat4 pMat, vMat, mMat, mvMat, invTrMat;
glm::vec3 currentLightPos, lightPosV;
float lightPos[3];

//initial light location
glm::vec3 initialLightLoc = glm::vec3(5.0f, 2.0f, 2.0f);


//white light properties
float globalAmbient[4] = { 0.7f, 0.7f, 0.7f, 1.0f };
float lightAmbient[4] = { 0.0f, 0.0f, 0.0f, 1.0f };
float lightDiffuse[4] = { 1.0f, 1.0f, 1.0f, 1.0f }; 
float lightSpecular[4] = { 1.0f, 1.0f, 1.0f, 1.0f };

float* matAmb = bronzeAmbient();
float* matDif = bronzeDiffuse();
float* matSpe = bronzeSpecular();
float matShi = bronzeShininess();

float ROTATION_SPEED = 0.1;

GLuint brickTexture;

void setupVertices(void) {

    //float pyramidPositions[54] = {
    //    -1.0f, -1.0f,  1.0f,  1.0f, -1.0f,  1.0f,  0.0f,  1.0f,  0.0f,
    //     1.0f, -1.0f,  1.0f,  1.0f, -1.0f, -1.0f,  0.0f,  1.0f,  0.0f,
    //     1.0f, -1.0f, -1.0f, -1.0f, -1.0f, -1.0f,  0.0f,  1.0f,  0.0f,
    //    -1.0f, -1.0f, -1.0f, -1.0f, -1.0f,  1.0f,  0.0f,  1.0f,  0.0f,
    //    -1.0f, -1.0f, -1.0f,  1.0f, -1.0f,  1.0f, -1.0f, -1.0f,  1.0f,
    //     1.0f, -1.0f,  1.0f, -1.0f, -1.0f, -1.0f,  1.0f, -1.0f, -1.0f
    //};
    //float pyrTexCoords[36] = {
    //    
    //    0.0f, 0.0f, 1.0f, 0.0f, 0.5f, 1.0f,   0.0f, 0.0f, 1.0f, 0.0f, 0.5f, 1.0f,
    //    0.0f, 0.0f, 1.0f, 0.0f, 0.5f, 1.0f,   0.0f, 0.0f, 1.0f, 0.0f, 0.5f, 1.0f,
    //    0.0f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f,   1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f
    //    
    //};
    //std::vector<int> ind = mySphere.getIndices();
    //std::vector<glm::vec3> vert = mySphere.getVertices();
    //std::vector<glm::vec2> tex = mySphere.getTexCoords();
    //std::vector<glm::vec3> norm = mySphere.getNormals();
    //std::vector<float> pvalues;
    //std::vector<float> tvalues;
    //std::vector<float> nvalues;
    //int numIndices = mySphere.getNumIndices();
    //for (int i = 0; i < numIndices; i++) {
    //    pvalues.push_back((vert[ind[i]]).x);
    //    pvalues.push_back((vert[ind[i]]).y);
    //    pvalues.push_back((vert[ind[i]]).z);
    //    tvalues.push_back((tex[ind[i]]).s);
    //    tvalues.push_back((tex[ind[i]]).t);
    //    nvalues.push_back((norm[ind[i]]).x);
    //    nvalues.push_back((norm[ind[i]]).y);
    //    nvalues.push_back((norm[ind[i]]).z);
    //}

    std::vector<int> ind = myTorus.getIndices();
    std::vector<glm::vec3> vert = myTorus.getVertices();
    //std::vector<glm::vec2> tex = myTorus.getTexCoords();
    std::vector<glm::vec3> norm = myTorus.getNormals();
    std::vector<float> pvalues;
    //std::vector<float> tvalues;
    std::vector<float> nvalues;
    int numVertices = myTorus.getNumVertices();
    for (int i = 0; i < numVertices; i++) {
        pvalues.push_back(vert[i].x);
        pvalues.push_back(vert[i].y);
        pvalues.push_back(vert[i].z);
        //tvalues.push_back(tex[i].s);
        //tvalues.push_back(tex[i].t);
        nvalues.push_back(norm[i].x);
        nvalues.push_back(norm[i].y);
        nvalues.push_back(norm[i].z);
    }


    glGenVertexArrays(1, vao);
    glBindVertexArray(vao[0]);
    glGenBuffers(numVBOs, vbo);
    //vertices into vbo[0]
    glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
    glBufferData(GL_ARRAY_BUFFER, pvalues.size() * 4, &pvalues[0], GL_STATIC_DRAW);
    //textures into vbo[1]  
    //glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
    //glBufferData(GL_ARRAY_BUFFER, tvalues.size() * 4, &tvalues[0], GL_STATIC_DRAW);
    //normals into vbo[2]
    glBindBuffer(GL_ARRAY_BUFFER, vbo[2]);
    glBufferData(GL_ARRAY_BUFFER, nvalues.size() * 4, &nvalues[0], GL_STATIC_DRAW);
    //opengl indexing
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo[3]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, ind.size() * 4, &ind[0], GL_STATIC_DRAW);

    //for the pyramid
    //glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
    //glBufferData(GL_ARRAY_BUFFER, sizeof(pyramidPositions), pyramidPositions, GL_STATIC_DRAW);
    //pyr tex coords
    //glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
    //glBufferData(GL_ARRAY_BUFFER, sizeof(pyrTexCoords), pyrTexCoords, GL_STATIC_DRAW);
    //std::vector<glm::vec3> vert = myModel.getVertices();
    //std::vector<glm::vec2> tex = myModel.getTexCoords();
    //std::vector<glm::vec3> norm = myModel.getNormals();
    //int numObjVertices = myModel.getNumVertices();
    //std::vector<float> pvalues;
    //std::vector<float> tvalues;
    //std::vector<float> nvalues;
    //for (int i = 0; i < numObjVertices; i++) {
    //    pvalues.push_back((vert[i]).x);
    //    pvalues.push_back((vert[i]).y);
    //    pvalues.push_back((vert[i]).z);
    //    tvalues.push_back((tex[i]).s);
    //    tvalues.push_back((tex[i]).t);
    //    nvalues.push_back((norm[i]).x);
    //    nvalues.push_back((norm[i]).y);
    //    nvalues.push_back((norm[i]).z);
    //}
    //glGenVertexArrays(1, vao);
    //glBindVertexArray(vao[0]);
    //glGenBuffers(numVBOs, vbo);
    ////vertices into vbo[0]
    //glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
    //glBufferData(GL_ARRAY_BUFFER, pvalues.size() * 4, &pvalues[0], GL_STATIC_DRAW);
    ////textures into vbo[1]  
    //glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
    //glBufferData(GL_ARRAY_BUFFER, tvalues.size() * 4, &tvalues[0], GL_STATIC_DRAW);
    ////normals into vbo[2]
    //glBindBuffer(GL_ARRAY_BUFFER, vbo[2]);
    //glBufferData(GL_ARRAY_BUFFER, nvalues.size() * 4, &nvalues[0], GL_STATIC_DRAW);

}


void installLights(glm::mat4 vMatrix) {
        
    lightPosV = glm::vec3(vMatrix * glm::vec4(currentLightPos, 1.0));
    lightPos[0] = lightPosV.x;
    lightPos[1] = lightPosV.y;
    lightPos[2] = lightPosV.z;

    //get the locations of the light and material fields in the shader
    globalAmbLoc = glGetUniformLocation(renderingProgram, "globalAmbient");
    ambLoc = glGetUniformLocation(renderingProgram, "light.ambient");
    diffLoc = glGetUniformLocation(renderingProgram, "light.diffuse");
    specLoc = glGetUniformLocation(renderingProgram, "light.specular");
    posLoc = glGetUniformLocation(renderingProgram, "light.position");
    mAmbLoc = glGetUniformLocation(renderingProgram, "material.ambient");
    mDiffLoc = glGetUniformLocation(renderingProgram, "material.diffuse");
    mSpecLoc = glGetUniformLocation(renderingProgram, "material.specular");
    mShiLoc = glGetUniformLocation(renderingProgram, "material.shininess");

    //set the uniform light and material values in the shader
    glProgramUniform4fv(renderingProgram, globalAmbLoc, 1, globalAmbient);
    glProgramUniform4fv(renderingProgram, ambLoc, 1, lightAmbient);
    glProgramUniform4fv(renderingProgram, diffLoc, 1, lightDiffuse);
    glProgramUniform4fv(renderingProgram, specLoc, 1, lightSpecular);
    glProgramUniform4fv(renderingProgram, posLoc, 1, lightPos);
    glProgramUniform4fv(renderingProgram, mAmbLoc, 1, matAmb);
    glProgramUniform4fv(renderingProgram, mDiffLoc, 1, matDif);
    glProgramUniform4fv(renderingProgram, mSpecLoc, 1, matSpe);
    glProgramUniform1f(renderingProgram, mShiLoc, matShi);
}

void init(GLFWwindow* window) {
    renderingProgram = createShaderProgram();
    cameraX = 0.0f; cameraY = 0.0f; cameraZ = -10.0f;
    cubeLocX = 0.0f; cubeLocY = -2.0f; cubeLocZ = 0.0f;
    pyrLocX = 4.0f; pyrLocY = 1.0f; pyrLocZ = 0.0f;
    setupVertices();    
}


void display(GLFWwindow* window, double currentTime, KeyboardHandler& keyboardHandler) {
    //need to init these each frame.
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glClear(GL_DEPTH_BUFFER_BIT);
    glClear(GL_COLOR_BUFFER_BIT);
    glUseProgram(renderingProgram);
    //brickTexture = loadTexture("brick.jpg");
        
    //get the uniform variables for the MV and projection matrices
    mvLoc = glGetUniformLocation(renderingProgram, "mv_matrix");
    projLoc = glGetUniformLocation(renderingProgram, "proj_matrix");

    //mLoc = glGetUniformLocation(renderingProgram, "m_matrix");
    //vLoc = glGetUniformLocation(renderingProgram, "v_matrix");
    //pLoc = glGetUniformLocation(renderingProgram, "p_matrix");
    nLoc = glGetUniformLocation(renderingProgram, "norm_matrix");



    //build perspective matrix      
    glfwGetFramebufferSize(window, &width, &height);
    aspect = (float)width / (float)height;
    pMat = glm::perspective(1.0472f, aspect, 0.1f, 1000.0f); //1.0472 rad = 60 deg
    vMat = glm::translate(glm::mat4(1.0f), glm::vec3(cameraX, cameraY, cameraZ));
    glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(pMat));
   
    //vMat = glm::lookAt(glm::vec3(sin((float)currentTime*0.5f) * 6.0, 2.0f, cos((float)currentTime*0.5f) * 6.0), glm::vec3(0, 0, 0), glm::vec3(0.0f, 1.0f, 0.0f));
    //vMat = glm::lookAt(glm::vec3(cameraX, cameraY, cameraZ), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    //mvStack.push(vMat);
    //the pyramid as sun
    //mvStack.push(mvStack.top()); // This line effectively duplicates the top element of the stack.
    //mvStack.top() *= glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.0f)); //sun position
    //mvStack.push(mvStack.top());
    //mvStack.top() *= glm::rotate(glm::mat4(1.0f), (float)currentTime, glm::vec3(0.0f, 0.0f, 1.0f)); //sun rotation
    //mvStack.top() *= glm::rotate(glm::mat4(1.0f), (float)currentTime, glm::vec3(1.0f, 0.0f, 0.0f)); //sun rotation

    mMat = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.0f));
    mMat *= glm::rotate(mMat, (float)currentTime * 0.5f, glm::vec3(1.0f, 0.0f, 0.0f));

 

    currentLightPos = glm::vec3(initialLightLoc.x, initialLightLoc.y, initialLightLoc.z);
    installLights(vMat);

    mvMat = vMat * mMat;
   
    invTrMat = glm::transpose(glm::inverse(mMat));


    glUniformMatrix4fv(mvLoc, 1, GL_FALSE, glm::value_ptr(mvMat));
    glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(pMat));
    glUniformMatrix4fv(nLoc, 1, GL_FALSE, glm::value_ptr(invTrMat));

    //glUniformMatrix4fv(mvLoc, 1, GL_FALSE, glm::value_ptr(mvStack.top()));
    

    //associateVBO with the corresponding vertex attribute in the vertex shader
    glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(0);

    //texture data
    //glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
    //glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, 0);
    //glEnableVertexAttribArray(1);
    //glActiveTexture(GL_TEXTURE0);
    //glBindTexture(GL_TEXTURE_2D, brickTexture);


    //bind the normals buffer to vertex attribute #1 in the vertex shader
    glBindBuffer(GL_ARRAY_BUFFER, vbo[2]);
    glVertexAttribPointer(1, 3, GL_FLOAT, false, 0, 0);
    glEnableVertexAttribArray(1);

    //adjust OpenGL settings and draw model
    glEnable(GL_CULL_FACE);
    glFrontFace(GL_CCW);   
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);


    //glDrawArrays(GL_TRIANGLES, 0, mySphere.getNumIndices()); // draw the sun

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo[3]); //to tell openGL which buffer contains the indices
    //openGL is able to recognize the presence of a GL_ELEMENT_ARRAY_BUFFER and utilize it to access the vertex attributes
    glDrawElements(GL_TRIANGLES, myTorus.getNumIndices(), GL_UNSIGNED_INT, 0); 

    //mvStack.pop(); mvStack.pop();

    float cameraSpeed = 0.004f;

    keyboardHandler.update();
    if (keyboardHandler.isKeyPressed(GLFW_KEY_W)) {
        cameraZ += cameraSpeed;
    }
    if (keyboardHandler.isKeyPressed(GLFW_KEY_S)) {
        cameraZ -= cameraSpeed;
    }
    if (keyboardHandler.isKeyPressed(GLFW_KEY_A)) {
        cameraX += cameraSpeed;
    }
    if (keyboardHandler.isKeyPressed(GLFW_KEY_D)) {
        cameraX -= cameraSpeed;
    }
    if (keyboardHandler.isKeyPressed(GLFW_KEY_Q)) {
        cameraY += cameraSpeed;
    }
    if (keyboardHandler.isKeyPressed(GLFW_KEY_E)) {
        cameraY -= cameraSpeed;
    }

    std::cout << "Light Position (View Space): " << lightPos[0] << ", " << lightPos[1] << ", " << lightPos[2] << std::endl;
}

int main(void) {
    if (!glfwInit()) { exit(EXIT_FAILURE); }
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    GLFWwindow* window = glfwCreateWindow(1600, 900, "program2", NULL, NULL);
    glfwMakeContextCurrent(window);
    if (glewInit() != GLEW_OK) { exit(EXIT_FAILURE); }
    glfwSwapInterval(1);
    init(window);
    KeyboardHandler keyboardHandler(window);

    while (!glfwWindowShouldClose(window)) {
        display(window, glfwGetTime(), keyboardHandler);
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwDestroyWindow(window);
    glfwTerminate();
    exit(EXIT_SUCCESS);

}