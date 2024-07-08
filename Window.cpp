#include "Window.hpp"

#include <iostream>
#include <stdexcept>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "Input.hpp"
#include "StaticMesh.hpp"
#include "Shader.hpp"
#include "Texture2D.hpp"
#include "Entity.hpp"
#include "Camera.hpp"



static bool g_bResized = false;
void framebufferResizeCallback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
    g_bResized = true;
}

void Window::Init()
{
    // glfw initialization and setup
    glfwInit();

    // set opengl version to 3.3 and core profile
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    
    // set to focus when window opens
    glfwWindowHint(GLFW_FOCUS_ON_SHOW, GLFW_TRUE);

    m_glfwWindow = glfwCreateWindow(m_width, m_height, m_title.c_str(), nullptr, nullptr);
    if (!m_glfwWindow)
    {
        glfwTerminate();
        throw std::runtime_error("Failed to create GLFW window");
    }

    // Set current context to this window
    glfwMakeContextCurrent(m_glfwWindow);

    // initialzie GLAD
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        glfwTerminate();
        throw std::runtime_error("Failed to initialize GLAD");
    }

    // set viewport size
    glViewport(0, 0, m_width, m_height);
    // set callback for viewport resize
    glfwSetFramebufferSizeCallback(m_glfwWindow, framebufferResizeCallback);

    // hide cursor and capture it (to capture mouse movement)
    glfwSetInputMode(m_glfwWindow, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    
    // Init mouse and keyboard callbacks
    Input::Init(m_glfwWindow);

    // enable depth testing
    glEnable(GL_DEPTH_TEST);
}

void Window::MainLoop()
{
    // ===== TRIANGLE ======
    float triVertices[] = {
        // positions         // tex coords
        -0.5f, -0.5f, 0.0f,  0.0f, 0.0f, // left bottom
         0.5f, -0.5f, 0.0f,  1.0f, 0.0f, // right bottom
         0.0f,  0.5f, 0.0f,  0.5f, 0.5f // top
    };

    StaticMesh triangleMesh(triVertices, sizeof(triVertices), 3, "shaders/triangle_vert.glsl", "shaders/triangle_frag.glsl");
    // position vertex attrib
    triangleMesh.SetVertexAttribute(0, 3, GL_FLOAT, GL_FALSE, 5*sizeof(float), (void*)0);
    // texcoords vertex attrib
    triangleMesh.SetVertexAttribute(1, 2, GL_FLOAT, GL_FALSE, 5*sizeof(float), (void*)(3*sizeof(float)));

    Texture2D atlasTex("textures/medieval-signs-atlas.png", true, GL_RGB, GL_RGBA, GL_TEXTURE0);
    // activate unit in shader (not needed)
    triangleMesh.GetShader().SetInt("texUnit", 0);

    Entity triangle(triangleMesh);

    // ===== END TRIANGLE ======

    // ===== RECTANGLE ======
    float rectVertices[] = {
        // positions         // colors          // texture coords
         0.5f,  0.5f, 0.0f,  1.0f, 0.0f, 0.0f,  1.0f, 1.0f, // top right
         0.5f, -0.5f, 0.0f,  0.0f, 1.0f, 0.0f,  1.0f, 0.0f, // bottom right
        -0.5f, -0.5f, 0.0f,  0.0f, 0.0f, 1.0f,  0.0f, 0.0f, // bottom left
        -0.5f,  0.5f, 0.0f,  0.0f, 1.0f, 1.0f,  0.0f, 1.0f  // top left
    };
    GLuint rectIndices[] = {
        0, 1, 3, // first triangle
        1, 2, 3  // second triangle
    };

    StaticMesh rectMesh(
        rectVertices, 
        sizeof(rectVertices), 
        6, 
        rectIndices, 
        sizeof(rectIndices), 
        "shaders/rectangle_vert.glsl", 
        "shaders/rectangle_frag.glsl"
    );
    rectMesh.SetVertexAttribute(0, 3, GL_FLOAT, GL_FALSE, 8*sizeof(float), (void*)0);
    rectMesh.SetVertexAttribute(2, 2, GL_FLOAT, GL_FALSE, 8*sizeof(float), (void*)(6*sizeof(float)));

    Texture2D containerTex("textures/container.jpg", false, GL_RGB, GL_RGB, GL_TEXTURE0);
    Texture2D romaTex("textures/roma.png", true, GL_RGB, GL_RGBA, GL_TEXTURE1);

    rectMesh.GetShader().SetInt("texture0", 0); // set texture0 to use unit GL_TEXTURE0
    rectMesh.GetShader().SetInt("texture1", 1); // set texture0 to use unit GL_TEXTURE1
    rectMesh.GetShader().SetFloat("mixProportion", 0.5);

    Entity rectangle(rectMesh);

    // ===== END RECTANGLE ======


    // ===== CUBE ======
    float cubeVertices[]={
        // positions        // texcoords
        // BACK
        -0.5f, -0.5f, -0.5f,  1.0f, 0.0f,
         0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
         0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
        -0.5f,  0.5f, -0.5f,  1.0f, 1.0f,

        // RIGHT
         0.5f, -0.5f, -0.5f,  1.0f, 0.0f,
         0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
         0.5f,  0.5f,  0.5f,  0.0f, 1.0f,
         0.5f, -0.5f,  0.5f,  0.0f, 0.0f,

        // FRONT
        -0.5f,  0.5f,  0.5f,  0.0f, 1.0f,
         0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
         0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,

        // TOP
         0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
        -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
        -0.5f,  0.5f,  0.5f,  0.0f, 0.0f,
         0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

        // LEFT
        -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
        -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
        -0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
        -0.5f, -0.5f,  0.5f,  1.0f, 0.0f,

        // BOTTOM
        -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
         0.5f, -0.5f, -0.5f,  1.0f, 0.0f,
         0.5f, -0.5f,  0.5f,  1.0f, 1.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, 1.0f
    };

    unsigned int cubeIndices[] = {
        0, 1, 3,
        3, 1, 2,
        5, 4, 7,
        7, 5, 6,
        9, 10, 11,
        11, 9, 8,
        14, 15, 12,
        12, 14, 13,
        17, 18, 16,
        16, 18, 19,
        23, 20, 21,
        21, 23, 22
    };

    StaticMesh cubeMesh(
        cubeVertices, 
        sizeof(cubeVertices), 
        static_cast<unsigned int>(sizeof(cubeIndices) / sizeof(float)),
        cubeIndices,
        sizeof(cubeIndices),
        "shaders/cube_vert.glsl", 
        "shaders/cube_frag.glsl"
    );
    // position vertex attribute
    cubeMesh.SetVertexAttribute(0, 3, GL_FLOAT, GL_FALSE, 5*sizeof(float), (void*)0);
    // texcoord vertex attribute
    cubeMesh.SetVertexAttribute(1, 2, GL_FLOAT, GL_FALSE, 5*sizeof(float), (void*)(3*sizeof(float)));

    Entity cube(cubeMesh);
    cube.Mesh.GetShader().SetInt("texture0", 0);
    cube.Mesh.GetShader().SetInt("texture1", 1);
    cube.Mesh.GetShader().SetFloat("mixProportion", 0.5f);

    // ===== END CUBE ======

    // TODO: Figure out a way to take this 3D setup to another place
    // (maybe a Scene/World class? Including a camera, world origin (for model transform) etc)
    // ===== CAMERA =====

    Camera camera;
    camera.Transform.SetPosition(glm::vec3(0.0f, 0.0f, 3.0f));

    // ===== END CAMERA =====
    
    // ===== 3D SETUP ======

    // MODEL MATRIX WILL BE OUR TRANSFORM MATRIX (because world origin is 0,0,0)
    // for now

    glm::mat4 view = glm::mat4(1.0f);
    view = glm::translate(view, glm::vec3(0.0f, 0.0f, -3.0f));

    float near = 0.1f;
    float far = 100.0f;
    glm::mat4 projection = glm::perspective(
        glm::radians(camera.FOV),
        m_aspectRatio,
        near,
        far
    );

    // ===== END 3D SETUP ======


    // ===== LIGHTING =====
    
    float cubeWithNormals[] = {
        -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
        0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f, 
        0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f, 
        0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f, 
        -0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f, 
        -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f, 

        -0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
        0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
        0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
        0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
        -0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
        -0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,

        -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
        -0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
        -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
        -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
        -0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
        -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,

        0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
        0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
        0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
        0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
        0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
        0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,

        -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
        0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
        0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
        0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,

        -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
        0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
        0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
        0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
        -0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
        -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f
    };

    StaticMesh cubeNormals(
        cubeWithNormals,
        sizeof(cubeWithNormals),
        36,
        "shaders/lighting_vert.glsl",
        "shaders/lighting_frag.glsl"
    );
    cubeNormals.SetVertexAttribute(0, 3, GL_FLOAT, GL_FALSE, 6*sizeof(float), (void*)0);
    cubeNormals.SetVertexAttribute(1, 3, GL_FLOAT, GL_FALSE, 6*sizeof(float), (void*)(3*sizeof(float)));

    // cube to be affected by the light
    Entity noTexCube(cubeNormals);
    glm::vec3 objColor(1.0f, 0.5f, 0.31f);

    // create new mesh only for light (with differnt shader as well)
    StaticMesh lightMesh(
        cubeVertices,
        sizeof(cubeVertices),
        sizeof(cubeIndices) / sizeof(float),
        cubeIndices,
        sizeof(cubeIndices),
        "shaders/lightsource_vert.glsl",
        "shaders/lightsource_frag.glsl"
    );
    lightMesh.SetVertexAttribute(0, 3, GL_FLOAT, GL_FALSE, 5*sizeof(float), (void*)0);

    Entity lightSource(lightMesh);
    lightSource.Transform.Scale(0.5f);
    lightSource.Transform.SetPosition(glm::vec3(1.2f, 1.0f, 2.0f));
    // white light
    glm::vec3 lightColor(1.0f);

    // ambient lighting factor
    float ambientFactor = 0.1f;

    // specular strength and (constant) shininess
    float specularStrength = 0.5f;
    float specularShininess = 32.0f;

    // ===== END LIGHTING =====



    float deltaTime = 0.0f;
    float lastFrame = 0.0f;
    while (!glfwWindowShouldClose(m_glfwWindow))
    {
        float currentFrame = (float)glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        if (g_bResized)
            this->updateWindowProperties();

        if (Input::GetKeyState(GLFW_KEY_ESCAPE))
        {
            glfwSetWindowShouldClose(m_glfwWindow, true);
        }
        
        // Currently activate texture before drawing the object, so the object uses the activated textures
        // Ideally it's better to activate the textures in the Draw function, but it's best to way for a Material class
        //containerTex.Activate();
        //romaTex.Activate();
        //rectangle.Draw();

        //atlasTex.Activate();
        //triangle.Draw();

        camera.Update(deltaTime);

        projection = glm::perspective(
            glm::radians(camera.FOV),
            m_aspectRatio,
            near,
            far
        );


        noTexCube.Mesh.GetShader().SetMat4("model", noTexCube.Transform.GetTransformMatrix());
        noTexCube.Mesh.GetShader().SetMat4("view", camera.GetLookAtMatrix());
        noTexCube.Mesh.GetShader().SetMat4("projection", projection);
        // lighting 
        noTexCube.Mesh.GetShader().SetVec3("objectColor", objColor);
        noTexCube.Mesh.GetShader().SetVec3("lightColor", lightColor);
        noTexCube.Mesh.GetShader().SetVec3("lightPos", lightSource.Transform.GetPosition());
        noTexCube.Mesh.GetShader().SetVec3("viewPos", camera.Transform.GetPosition());
        noTexCube.Mesh.GetShader().SetFloat("ambientFactor", ambientFactor);
        noTexCube.Mesh.GetShader().SetFloat("specularStrength", specularStrength);
        noTexCube.Mesh.GetShader().SetFloat("specularShininess", specularShininess);
        noTexCube.Draw();

        lightSource.Mesh.GetShader().SetMat4("model", lightSource.Transform.GetTransformMatrix());
        lightSource.Mesh.GetShader().SetMat4("view", camera.GetLookAtMatrix());
        lightSource.Mesh.GetShader().SetMat4("projection", projection);
        lightSource.Mesh.GetShader().SetVec3("lightColor", lightColor);
        lightSource.Draw();


        // light orbiting around cube
        float radius = 2.0f;
        float time = static_cast<float>(glfwGetTime());
        float light_x = radius * std::sin(time);
        float light_z = radius * std::cos(time);
        float light_y = 1.0f;
        lightSource.Transform.SetPosition(glm::vec3(light_x, light_y, light_z));


        glfwSwapBuffers(m_glfwWindow);
        glfwPollEvents();
    }
}

void Window::updateWindowProperties()
{
    // i can't update the member's width and height of the window in the callback
    // so this is an ugly turnaround
    int width = 0, height = 0;
    glfwGetWindowSize(m_glfwWindow, &width, &height);
    m_width = width, m_height = height;

    m_aspectRatio = (float)m_width / (float)m_height;

    g_bResized = false;
}
