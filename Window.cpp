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
    float cubeVertices[] = {
        // positions         // texcoords
        -0.5f, -0.5f, -0.5f,  0.0f, 0.0f, // front left bottom
         0.5f, -0.5f, -0.5f,  1.0f, 0.0f, // front right bottom
         0.5f,  0.5f, -0.5f,  1.0f, 1.0f, // front right top
        -0.5f,  0.5f, -0.5f,  0.0f, 1.0f, // front left top
        -0.5f,  0.5f,  0.5f,  0.0f, 1.0f, // back left top
         0.5f,  0.5f,  0.5f,  1.0f, 1.0f, // back right top
         0.5f, -0.5f,  0.5f,  1.0f, 0.0f, // back right bottom
        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f  // back left bottom
    };
    unsigned int cubeIndices[] = {
        0,1,2,
        2,3,0,
        0,7,4,
        4,0,3,
        3,4,5,
        5,3,2,
        2,5,6,
        6,2,1,
        1,6,0,
        0,6,7,
        7,4,5,
        5,7,6
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
    // ===== 3D SETUP ======

    // MODEL MATRIX WILL BE OUR TRANSFORM MATRIX (because world origin is 0,0,0)
    // for now

    glm::mat4 view = glm::mat4(1.0f);
    view = glm::translate(view, glm::vec3(0.0f, 0.0f, -3.0f));

    float fov = 45.0f;
    float aspectRatio = (float)m_width / (float)m_height;
    float near = 0.1f;
    float far = 100.0f;
    glm::mat4 projection = glm::perspective(
        glm::radians(fov),
        aspectRatio,
        near,
        far
    );

    // ===== END 3D SETUP ======

    // ===== CAMERA =====

    Camera camera;
    camera.Transform.SetPosition(glm::vec3(0.0f, 0.0f, 3.0f));

    // ===== END CAMERA =====

    glm::vec3 cubePositions[] = {
        glm::vec3( 0.0f,  0.0f,  0.0f), 
        glm::vec3( 2.0f,  5.0f, -15.0f), 
        glm::vec3(-1.5f, -2.2f, -2.5f),  
        glm::vec3(-3.8f, -2.0f, -12.3f),  
        glm::vec3( 2.4f, -0.4f, -3.5f),  
        glm::vec3(-1.7f,  3.0f, -7.5f),  
        glm::vec3( 1.3f, -2.0f, -2.5f),  
        glm::vec3( 1.5f,  2.0f, -2.5f), 
        glm::vec3( 1.5f,  0.2f, -1.5f), 
        glm::vec3(-1.3f,  1.0f, -1.5f)
    };

    Entity cubes[10];
    for (int i = 0; i < 10; i++)
    {
        cubes[i] = cube;
        cubes[i].Transform.SetPosition(cubePositions[i]);
    }

    float deltaTime = 0.0f;
    float lastFrame = 0.0f;
    while (!glfwWindowShouldClose(m_glfwWindow))
    {
        float currentFrame = (float)glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        glClearColor(0.3f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        if (g_bResized)
            this->updateWindowProperties();

        if (Input::GetKeyState(GLFW_KEY_ESCAPE))
        {
            glfwSetWindowShouldClose(m_glfwWindow, true);
        }
        
        // Currently active texture before drawing the object, so the object uses the activated textures
        // Ideally it's better to activate the textures in the Draw function, but it's best to way for a Material class
        //containerTex.Activate();
        //romaTex.Activate();
        //rectangle.Draw();

        //atlasTex.Activate();
        //triangle.Draw();

        camera.Update(deltaTime);

        containerTex.Activate();
        romaTex.Activate();
        for (auto& e_cube : cubes)
        {
            e_cube.Mesh.GetShader().SetMat4("model", e_cube.Transform.GetTransformMatrix());
            e_cube.Mesh.GetShader().SetMat4("view", camera.GetLookAtMatrix());
            e_cube.Mesh.GetShader().SetMat4("projection", projection);

            e_cube.Mesh.Draw();
        }

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

    g_bResized = false;
}
