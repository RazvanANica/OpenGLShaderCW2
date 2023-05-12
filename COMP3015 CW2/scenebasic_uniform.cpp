#include "scenebasic_uniform.h"

#include <cstdio>
#include <cstdlib>
#include <sstream>
#include <string>

using std::string;

#include <iostream>
using std::cerr;
using std::endl;

#include <glm/gtc/matrix_transform.hpp>
#include "helper/glutils.h"
#include "helper/texture.h"

using glm::vec3;
using glm::vec4;
using glm::mat4;



SceneBasic_Uniform::SceneBasic_Uniform() : plane(10.0f, 10.0f, 100, 100) {
    Vinayagar = ObjMesh::load("media/Vinayagar.obj", true); //load first mesh
    Plant = ObjMesh::load("media/Pot.obj", true); //load second mesh
}

void SceneBasic_Uniform::initScene()
{
    compile();
    glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
    glEnable(GL_DEPTH_TEST);
    projection = mat4(1.0f);
    angle = glm::pi<float>() / 4.0f;

    setupFBO();

    // Array for full-screen quad
    GLfloat verts[] = {
    -1.0f, -1.0f, 0.0f, 1.0f, -1.0f, 0.0f, 1.0f, 1.0f, 0.0f,
    -1.0f, -1.0f, 0.0f, 1.0f, 1.0f, 0.0f, -1.0f, 1.0f, 0.0f
    };
    GLfloat tc[] = {
    0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f,
    0.0f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f
    };

    // Set up the buffers
    unsigned int handle[2];
    glGenBuffers(2, handle);
    glBindBuffer(GL_ARRAY_BUFFER, handle[0]);
    glBufferData(GL_ARRAY_BUFFER, 6 * 3 * sizeof(float), verts, GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, handle[1]);
    glBufferData(GL_ARRAY_BUFFER, 6 * 2 * sizeof(float), tc, GL_STATIC_DRAW);

    // Set up the vertex array object
    glGenVertexArrays(1, &quad);
    glBindVertexArray(quad);
    glBindBuffer(GL_ARRAY_BUFFER, handle[0]);
    glVertexAttribPointer((GLuint)0, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(0); // Vertex position
    glBindBuffer(GL_ARRAY_BUFFER, handle[1]);
    glVertexAttribPointer((GLuint)2, 2, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(2); // Texture coordinates
    glBindVertexArray(0);
    
    vec3 intense = vec3(5.0f);
    prog.setUniform("Lights[0].Ld", intense);
    prog.setUniform("Lights[1].Ld", intense);
    prog.setUniform("Lights[2].Ld", intense);

    prog.setUniform("Lights[0].Ls", intense);
    prog.setUniform("Lights[1].Ls", intense);
    prog.setUniform("Lights[2].Ls", intense);

    intense = vec3(0.2f);
    prog.setUniform("Lights[0].La", intense);
    prog.setUniform("Lights[1].La", intense);
    prog.setUniform("Lights[2].La", intense);

    GLuint tex1 = Texture::loadTexture("media/texture/Brick.jpg");
    GLuint tex2 = Texture::loadTexture("media/texture/moss.png");

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, tex1);
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, tex2);

}

void SceneBasic_Uniform::compile()
{
	try {
		prog.compileShader("shader/basic_uniform.vert");
		prog.compileShader("shader/basic_uniform.frag");
		prog.link();
		prog.use();
	} catch (GLSLProgramException &e) {
		cerr << e.what() << endl;
		exit(EXIT_FAILURE);
	}
}

void SceneBasic_Uniform::update( float t )
{
    angle = t * 10.0f;
}

void SceneBasic_Uniform::render()
{   
    pass1();
    computeLogAveLuminance();
    pass2();

    
}



void SceneBasic_Uniform::resize(int w, int h)
{
    glViewport(0, 0, w, h);
    width = w;
    height = h;
    projection = glm::perspective(glm::radians(70.0f), (float)w / h, 0.3f, 100.0f);

}

void SceneBasic_Uniform::setMatrices() {
    mat4 mv = view * model;
    prog.setUniform("ModelViewMatrix", mv);
    prog.setUniform("NormalMatrix", glm::mat3(vec3(mv[0]),vec3(mv[1]), vec3(mv[2])));
    prog.setUniform("MVP", projection * mv );
    prog.setUniform("ProjectionMatrix", projection);
}

void SceneBasic_Uniform::setupFBO() {

    GLuint depthBuf;
    // Create and bind the FBO
    glGenFramebuffers(1, &hdrFBO);
    glBindFramebuffer(GL_FRAMEBUFFER, hdrFBO);
    // The depth buffer
    glGenRenderbuffers(1, &depthBuf);
    glBindRenderbuffer(GL_RENDERBUFFER, depthBuf);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, width, height);
    // The HDR color buffer
    glActiveTexture(GL_TEXTURE0);
    glGenTextures(1, &hdrTex);
    glBindTexture(GL_TEXTURE_2D, hdrTex);
    glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGB32F, width, height);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    // Attach the images to the framebuffer
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,
        GL_RENDERBUFFER, depthBuf);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D,
        hdrTex, 0);
    GLenum drawBuffers[] = { GL_NONE, GL_COLOR_ATTACHMENT0 };
    glDrawBuffers(2, drawBuffers);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void SceneBasic_Uniform::pass1()
{
    prog.setUniform("Pass", 1);
    glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
    glViewport(0, 0, width, height);
    glBindFramebuffer(GL_FRAMEBUFFER, hdrFBO);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);
    

    view = glm::lookAt(vec3(0.0f, 4.0f, 5.0f), vec3(0.0f, 0.0f, 0.0f), vec3(0.0f, 2.0f, 1.0f));
    projection = glm::perspective(glm::radians(60.0f), (float)width / height,
        0.3f, 100.0f);
    

    drawScene();
}

void SceneBasic_Uniform::pass2()
{
    prog.setUniform("Pass", 2);
    // Revert to default framebuffer
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glDisable(GL_DEPTH_TEST);
    view = mat4(1.0);
    model = mat4(1.0);
    projection = mat4(1.0);
    setMatrices();
    // Render the quad
    glBindVertexArray(quad);
    glDrawArrays(GL_TRIANGLES, 0, 6);
}

void SceneBasic_Uniform::computeLogAveLuminance()
{

    int size = width * height;
    std::vector<GLfloat> texData(size * 3);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, hdrTex);
    glGetTexImage(GL_TEXTURE_2D, 0, GL_RGB, GL_FLOAT, texData.data());
    float sum = 0.0f;
    for (int i = 0; i < size; i++) {
        float lum = glm::dot(vec3(texData[i * 3 + 0], texData[i * 3 + 1],
            texData[i * 3 + 2]),
            vec3(0.2126f, 0.7152f, 0.0722f));
        sum += logf(lum + 0.00001f);
    }
    prog.setUniform("AveLum", expf(sum / size));
}


void SceneBasic_Uniform::drawScene()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    vec3 intense = vec3(1.0f);
    prog.setUniform("Lights[0].L", intense);
    prog.setUniform("Lights[1].L", intense);
    prog.setUniform("Lights[2].L", intense);
    vec4 lightPos = vec4(0.0f, 1.5f, 0.0f, 1.0f);
    lightPos.x = -5.0f;
    
    prog.setUniform("Lights[0].Position",  view * lightPos);
    lightPos.x = 0.0f;
    prog.setUniform("Lights[1].Position",  view * lightPos);
    lightPos.x = 5.0f;
    
    prog.setUniform("Lights[2].Position",  view *lightPos);




    prog.setUniform("Material.Kd", 0.5f, 0.7f, 0.2f);
    prog.setUniform("Material.Ks", 0.5f, 0.7f, 0.2f);
    prog.setUniform("Material.Ka", 0.5f, 0.7f, 0.2f);
    prog.setUniform("Material.Shininess", 100.0f);

    model = mat4(1.0f);
    model = glm::translate(model, vec3(0.0f, 1.0f, 0.0f));
    model = glm::rotate(model, glm::radians(-45.0f * angle), vec3(0.0f, 1.0f, 0.0f));
   
    setMatrices();
    Plant->render();


    prog.setUniform("Material.Kd", 0.9f, 0.4f, 0.4f);
    prog.setUniform("Material.Ks", 0.9f, 0.4f, 0.4f);
    prog.setUniform("Material.Ka", 0.9f, 0.4f, 0.4f);
    prog.setUniform("Material.Shininess", 20.0f);

    model = mat4(1.0f);
    model = glm::translate(model, vec3(-1.5f, 1.5f, 0.0f));
    model = glm::rotate(model, glm::radians(90.0f), vec3(0.0f, 1.0f, 0.0f));
    setMatrices();
    Vinayagar->render();

    prog.setUniform("Material.Kd", 0.2f, 0.5f, 0.3f);
    prog.setUniform("Material.Ks", 0.2f, 0.5f, 0.3f);
    prog.setUniform("Material.Ka", 0.1f, 0.1f, 0.1f);
    prog.setUniform("Material.Shininess", 20.0f);

    model = mat4(1.0f);
    model = glm::translate(model, vec3(1.5f, 1.5f, 0.0f));
    model = glm::rotate(model, glm::radians(-90.0f), vec3(0.0f, 1.0f, 0.0f));
    setMatrices();
    Vinayagar->render();


    prog.setUniform("Material.Kd", 0.2f, 0.2f, 0.2f);
    prog.setUniform("Material.Ks", 0.2f, 0.2f, 0.2f);
    prog.setUniform("Material.Ka", 0.2f, 0.2f, 0.2f);
    prog.setUniform("Material.Shininess", 20.0f);

    model = mat4(1.0f);
    model = glm::translate(model, vec3(0.0f, -0.45f, 0.0f));
    setMatrices();
    plane.render();
}
