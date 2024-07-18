#include "Window.hpp"

#include <iostream>
#include <stdexcept>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <imgui/imgui.h>
#include <imgui/backends/imgui_impl_glfw.h>
#include <imgui/backends/imgui_impl_opengl3.h>

#include <stb/stb_image.h>

#include "Input.hpp"
#include "StaticMesh.hpp"
#include "Shader.hpp"
#include "Texture2D.hpp"
#include "Entity.hpp"
#include "Camera.hpp"
#include "Material.hpp"
#include "Light.hpp"
#include "UIHelper.hpp"
#include "ModelFactory.hpp"
#include "glm/ext/matrix_transform.hpp"


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

    // Disable vsync
    glfwSwapInterval(0);

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
    Input::RegisterCallbacks(m_glfwWindow);

    // enable depth testing
    glEnable(GL_DEPTH_TEST);

    // Initiate ImGui
    UIHelper::Init(m_glfwWindow);

    stbi_set_flip_vertically_on_load(true);
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

    Shader triangleShader("shaders/triangle_vert.glsl", "shaders/triangle_frag.glsl");

    StaticMesh triangleMesh(triVertices, sizeof(triVertices), 3);
    // position vertex attrib
    triangleMesh.SetVertexAttribute(0, 3, GL_FLOAT, GL_FALSE, 5*sizeof(float), (void*)0);
    // texcoords vertex attrib
    triangleMesh.SetVertexAttribute(1, 2, GL_FLOAT, GL_FALSE, 5*sizeof(float), (void*)(3*sizeof(float)));

    Texture2D atlasTex("textures/medieval-signs-atlas.png", true, GL_RGB, GL_RGBA, GL_TEXTURE0);
    // activate unit in shader (not needed)
    triangleShader.SetInt("texUnit", 0);

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

    Shader rectShader("shaders/rectangle_vert.glsl", "shaders/rectangle_frag.glsl");

    StaticMesh rectMesh(rectVertices, sizeof(rectVertices), 6, rectIndices, sizeof(rectIndices));
    rectMesh.SetVertexAttribute(0, 3, GL_FLOAT, GL_FALSE, 8*sizeof(float), (void*)0);
    rectMesh.SetVertexAttribute(2, 2, GL_FLOAT, GL_FALSE, 8*sizeof(float), (void*)(6*sizeof(float)));

    Texture2D containerTex("textures/container.jpg", false, GL_RGB, GL_RGB, GL_TEXTURE0);
    Texture2D romaTex("textures/roma.png", true, GL_RGB, GL_RGBA, GL_TEXTURE1);

    rectShader.SetInt("texture0", 0); // set texture0 to use unit GL_TEXTURE0
    rectShader.SetInt("texture1", 1); // set texture0 to use unit GL_TEXTURE1
    rectShader.SetFloat("mixProportion", 0.5);

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

    Shader cubeShader("shaders/cube_vert.glsl", "shaders/cube_frag.glsl");

    StaticMesh cubeMesh(
        cubeVertices, 
        sizeof(cubeVertices), 
        static_cast<unsigned int>(sizeof(cubeIndices) / sizeof(float)),
        cubeIndices,
        sizeof(cubeIndices)
    );
    // position vertex attribute
    cubeMesh.SetVertexAttribute(0, 3, GL_FLOAT, GL_FALSE, 5*sizeof(float), (void*)0);
    // texcoord vertex attribute
    cubeMesh.SetVertexAttribute(1, 2, GL_FLOAT, GL_FALSE, 5*sizeof(float), (void*)(3*sizeof(float)));

    Entity cube(cubeMesh);
    cubeShader.SetInt("texture0", 0);
    cubeShader.SetInt("texture1", 1);
    cubeShader.SetFloat("mixProportion", 0.5f);

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
        // positions          // normals           // texture coords
        -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 0.0f,
        0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 0.0f,
        0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 1.0f,
        0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 1.0f,
        -0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 0.0f,

        -0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   0.0f, 0.0f,
        0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   1.0f, 0.0f,
        0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   1.0f, 1.0f,
        0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   1.0f, 1.0f,
        -0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   0.0f, 1.0f,
        -0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   0.0f, 0.0f,

        -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 0.0f,
        -0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 1.0f,
        -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
        -0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 0.0f,
        -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 0.0f,

        0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f,
        0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f,
        0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
        0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
        0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 0.0f,
        0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f,

        -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 1.0f,
        0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 1.0f,
        0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 0.0f,
        0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 0.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 0.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 1.0f,

        -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 1.0f,
        0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 1.0f,
        0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 0.0f,
        0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 0.0f,
        -0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 0.0f,
        -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 1.0f
    };

    StaticMesh cubeNormals(
        cubeWithNormals,
        sizeof(cubeWithNormals),
        36
    );
    // position vertex attribute
    cubeNormals.SetVertexAttribute(0, 3, GL_FLOAT, GL_FALSE, 8*sizeof(float), (void*)0);
    // normals vertex attribute
    cubeNormals.SetVertexAttribute(1, 3, GL_FLOAT, GL_FALSE, 8*sizeof(float), (void*)(3*sizeof(float)));
    // texcoords vertex attribute
    cubeNormals.SetVertexAttribute(2, 2, GL_FLOAT, GL_FALSE, 8*sizeof(float), (void*)(6*sizeof(float)));

    // cube to be affected by the light
    Entity noTexCube(cubeNormals);

    // create new mesh only for light (with differnt shader as well)
    StaticMesh lightMesh(
        cubeVertices,
        sizeof(cubeVertices),
        sizeof(cubeIndices) / sizeof(float),
        cubeIndices,
        sizeof(cubeIndices)
    );
    lightMesh.SetVertexAttribute(0, 3, GL_FLOAT, GL_FALSE, 5*sizeof(float), (void*)0);

    Entity lightSource(lightMesh);
    lightSource.Transform.Scale(0.5f);
    lightSource.Transform.SetPosition(glm::vec3(1.2f, 1.0f, 2.0f));
    // white light
    glm::vec3 lightAmbient(0.2f);
    glm::vec3 lightDiffuse(1.0f);
    glm::vec3 lightSpecular(1.0f);
    bool bOrbitingCube = true;

    // using directional light
    glm::vec3 dirLightDirection(-0.2f, -1.0f, -0.3f);
    DirectionalLight dirLight(dirLightDirection, lightAmbient, lightDiffuse, lightSpecular);

    // using point light
    // lightPos will be the lightSource position
    float attConst = 1.0f;
    float attLinear = 0.09f;
    float attQuad = 0.032f;
    PointLight pointLight(
        lightSource.Transform.GetPosition(), 
        lightAmbient, lightDiffuse, 
        lightSpecular, 
        attConst, 
        attLinear, 
        attQuad
    );

    // using spotlight as flashlight
    // light position will be camera position
    // light direction will be camera pointing direction
    float innerCutoff = 12.5f;
    float outerCutoff = 17.5f;
    SpotLight spotLight(
        camera.Transform.GetPosition(), 
        camera.GetFrontVector(), 
        lightAmbient, 
        lightDiffuse, 
        lightSpecular, 
        innerCutoff, 
        outerCutoff, 
        attConst, 
        attLinear, 
        attQuad
    );

    bool bEnableLighting = true;
    bool bUseDirectionalLight = true;
    bool bUsePointLight = true;
    bool bUseSpotLight = true;

    // Material using diffuse and specular maps
    Texture2D woodCrateTex("textures/container2.png", false, GL_RGB, GL_RGBA, 0);
    Texture2D woodCrateSpecular("textures/container2_specular.png", false, GL_RGB, GL_RGBA, 1);
    MaterialMap woodCrateMat{};
    woodCrateMat.diffuseMapTex = woodCrateTex.GetTexUnit();
    woodCrateMat.specularMapTex = woodCrateSpecular.GetTexUnit();
    woodCrateMat.shininess = 32.0f;


    Shader lightingShader("shaders/lighting_vert.glsl", "shaders/lighting_frag.glsl");
    Shader lightSourceShader("shaders/lightsource_vert.glsl", "shaders/lightsource_frag.glsl");

    // ===== END LIGHTING =====


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
    for (size_t i = 0; i < 10; i++)
    {
        cubes[i] = Entity(cubeNormals);
        cubes[i].Transform.SetPosition(cubePositions[i]);
    }


    // TODO: change entity class to include a model
    
    // TODO delete StaticMesh and Texture2D classess
    
    // =============================================
    // USING MODELS
    // =============================================
    Model backpack("models/backpack/backpack.obj");

    Shader modelShader("shaders/model3d_vert.glsl", "shaders/model3d_frag.glsl");

    float deltaTime = 0.0f;
    float lastFrame = 0.0f;
    while (!glfwWindowShouldClose(m_glfwWindow))
    {
        float currentFrame = (float)glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // start dear imgui frame
        UIHelper::NewFrame();

        UIHelper::FrameStatsWindow(deltaTime);

        if (g_bResized)
            this->updateWindowProperties();

        if (Input::GetKeyState(GLFW_KEY_ESCAPE))
        {
            glfwSetWindowShouldClose(m_glfwWindow, true);
        }
        
        // TODO: make this 'check once' better
        // Toggle mouse cursor capture and thus camera mouse movement 
        static bool tabChanged = false;
        if (Input::GetKeyState(GLFW_KEY_TAB) && !tabChanged)
        {
            int cursor = glfwGetInputMode(m_glfwWindow, GLFW_CURSOR);
            int mode = (cursor == GLFW_CURSOR_DISABLED) ? GLFW_CURSOR_NORMAL : GLFW_CURSOR_DISABLED;
            glfwSetInputMode(m_glfwWindow, GLFW_CURSOR, mode);

            tabChanged = true;
        }
        else if (!Input::GetKeyState(GLFW_KEY_TAB) && tabChanged)
        {
            tabChanged = false;
        }

        static bool pChanged = false;
        if (Input::GetKeyState(GLFW_KEY_P) && !pChanged)
        {
            int polygonMode[2];
            glGetIntegerv(GL_POLYGON_MODE, polygonMode);
            if (polygonMode[1] == GL_FILL)
                glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
            else
                glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
            
            pChanged = true;
        }
        else if (!Input::GetKeyState(GLFW_KEY_P) && pChanged)
            pChanged = false;


        camera.Update(deltaTime);

        projection = glm::perspective(
            glm::radians(camera.FOV),
            m_aspectRatio,
            near,
            far
        );


        // lighting 
        /* lightingShader.Use(); */
        /* //lightingShader.SetMat4("model", noTexCube.Transform.GetTransformMatrix()); */
        /* lightingShader.SetMat4("view", camera.GetLookAtMatrix()); */
        /* lightingShader.SetMat4("projection", projection); */

        /* lightingShader.SetBool("u_bEnableLighting", bEnableLighting); */
        /* lightingShader.SetBool("u_bUseDirectionalLight", bUseDirectionalLight); */
        /* lightingShader.SetBool("u_bUsePointLight", bUsePointLight); */
        /* lightingShader.SetBool("u_bUseSpotLight", bUseSpotLight); */
    
        /* // Directional light: */
        /* if (bUseDirectionalLight) */
        /* { */
        /*     lightingShader.SetVec3("u_dirLight.direction", dirLight.Direction); */

        /*     lightingShader.SetVec3("u_dirLight.ambient", dirLight.Ambient); */
        /*     lightingShader.SetVec3("u_dirLight.diffuse", dirLight.Diffuse); */
        /*     lightingShader.SetVec3("u_dirLight.specular", dirLight.Specular); */
        /* } */

        /* // Point light: */
        /* if (bUsePointLight) */
        /* { */
        /*     // update position */
        /*     pointLight.Position = lightSource.Transform.GetPosition(); */

        /*     lightingShader.SetVec3("u_pointLight.position", pointLight.Position); */
        /*     lightingShader.SetFloat("u_pointLight.attConstant", pointLight.Attenuation.Constant); */
        /*     lightingShader.SetFloat("u_pointLight.attLinear", pointLight.Attenuation.Linear); */
        /*     lightingShader.SetFloat("u_pointLight.attQuadratic", pointLight.Attenuation.Quadratic); */

        /*     lightingShader.SetVec3("u_pointLight.ambient", pointLight.Ambient); */
        /*     lightingShader.SetVec3("u_pointLight.diffuse", pointLight.Diffuse); */
        /*     lightingShader.SetVec3("u_pointLight.specular", pointLight.Specular); */
        /* } */

        /* // Spotlight */
        /* if (bUseSpotLight) */
        /* { */
        /*     // update position and direction */
        /*     spotLight.Position = camera.Transform.GetPosition(); */
        /*     spotLight.Direction = camera.GetFrontVector(); */

        /*     lightingShader.SetVec3("u_spotLight.position", spotLight.Position); */
        /*     lightingShader.SetVec3("u_spotLight.direction", spotLight.Direction); */
        /*     lightingShader.SetFloat("u_spotLight.innerCutoff", glm::radians(spotLight.InnerCutoff)); */
        /*     lightingShader.SetFloat("u_spotLight.outerCutoff", glm::radians(spotLight.OuterCutoff)); */
        /*     lightingShader.SetFloat("u_spotLight.attConstant", spotLight.Attenuation.Constant); */
        /*     lightingShader.SetFloat("u_spotLight.attLinear", spotLight.Attenuation.Linear); */
        /*     lightingShader.SetFloat("u_spotLight.attQuadratic", spotLight.Attenuation.Quadratic); */

        /*     lightingShader.SetVec3("u_spotLight.ambient", spotLight.Ambient); */
        /*     lightingShader.SetVec3("u_spotLight.diffuse", spotLight.Diffuse); */
        /*     lightingShader.SetVec3("u_spotLight.specular", spotLight.Specular); */
        /* } */

        /* lightingShader.SetVec3("u_viewPos", camera.Transform.GetPosition()); */
        
        /* // USING TEXTURE MATERIAL: */
        /* woodCrateTex.Activate(); */
        /* woodCrateSpecular.Activate(); */
        /* lightingShader.SetInt("u_material.diffuseMap", woodCrateMat.diffuseMapTex); */
        /* lightingShader.SetInt("u_material.specularMap", woodCrateMat.specularMapTex); */
        /* lightingShader.SetFloat("u_material.shininess", woodCrateMat.shininess); */

        /* for (size_t i = 0; i < 10; i++) */
        /* { */
        /*     float angle = 20.0f * i; */
        /*     glm::vec3 rot(-0.2f*angle, -1.0f*angle, -0.3f*angle); */
        /*     cubes[i].Transform.SetRotation(rot); */

        /*     lightingShader.SetMat4("model", cubes[i].Transform.GetTransformMatrix()); */
        /*     cubes[i].Draw(); */
        /* } */


        /* // light source cube (different shader) */
        /* lightSourceShader.Use(); */ 
        /* lightSourceShader.SetMat4("model", lightSource.Transform.GetTransformMatrix()); */
        /* lightSourceShader.SetMat4("view", camera.GetLookAtMatrix()); */
        /* lightSourceShader.SetMat4("projection", projection); */
        /* lightSourceShader.SetVec3("lightColor", lightDiffuse); */
        /* lightSource.Draw(); */


        /* // light orbiting around cube */
        /* if (bOrbitingCube) */
        /* { */
        /*     float radius = 10.0f; */
        /*     float time = static_cast<float>(glfwGetTime()); */
        /*     float light_x = radius * std::sin(time); */
        /*     float light_z = radius * std::cos(time); */
        /*     float light_y = 1.0f; */
        /*     lightSource.Transform.SetPosition(glm::vec3(light_x, light_y, light_z)); */
        /* } */

	modelShader.Use();
	modelShader.SetMat4("u_projection", projection);
	modelShader.SetMat4("u_view", camera.GetLookAtMatrix());

	glm::mat4 backpackTrans(1.0f);
	backpackTrans = glm::translate(backpackTrans, glm::vec3(0.0f, 0.0f, 0.0f));
	modelShader.SetMat4("u_model", backpackTrans);

	backpack.Draw(modelShader);

        UIHelper::Render();

        glfwSwapBuffers(m_glfwWindow);
        glfwPollEvents();
    }

    Terminate();
}

void Window::Terminate()
{
    UIHelper::Terminate();

    glfwDestroyWindow(m_glfwWindow);
    glfwTerminate();
}

void Window::updateWindowProperties()
{
    // i can't update the members width and height of the window in the callback
    // so this is an ugly turnaround
    int width = 0, height = 0;
    glfwGetWindowSize(m_glfwWindow, &width, &height);
    m_width = width, m_height = height;

    m_aspectRatio = (float)m_width / (float)m_height;

    g_bResized = false;
}
