#include "Window.hpp"

#include <stdexcept>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <imgui/imgui.h>
#include <imgui/backends/imgui_impl_glfw.h>
#include <imgui/backends/imgui_impl_opengl3.h>

#include <stb/stb_image.h>

#include "Shader.hpp"
#include "Input.hpp"
#include "Camera.hpp"
#include "UIHelper.hpp"
#include "ModelFactory.hpp"
#include "StaticMesh.hpp"
#include "Entity.hpp"

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
	    sizeof(cubeIndices)
    );
    cubeMesh.SetVertexAttribute(0, 3, GL_FLOAT, GL_FALSE, 5*sizeof(float), (void*)0);
    cubeMesh.SetVertexAttribute(1, 2, GL_FLOAT, GL_FALSE, 5*sizeof(float), (void*)(3*sizeof(float)));

    Shader cubeShader("shaders/old/cube_vert.glsl", "shaders/old/cube_frag.glsl");
    cubeShader.Use();
    cubeShader.SetVec3("customColor", glm::vec3(1.0f, 1.0f, 1.0f));
    
    StaticMeshEntity cubeEntity(cubeMesh);
    cubeEntity.Transform.SetPosition(glm::vec3(0.0f, 3.0f, 0.0f));

    Camera camera;
    camera.Transform.SetPosition(glm::vec3(0.0f, 0.0f, 3.0f));
    
    float pNear =  0.1f;
    float pFar  = 100.0f;
    glm::mat4 projection = glm::perspective(
	glm::radians(camera.FOV),
	m_aspectRatio,
	pNear,
	pFar
    );

    Model backpack;
    ObjectLoader::LoadModel("models/backpack/backpack.obj", backpack);

    Shader modelShader("shaders/model3d.vert", "shaders/model3d.frag");

    ModelEntity backpackEntity(backpack);
    backpackEntity.Transform.SetPosition(glm::vec3(0.0f));




    float deltaTime = 0.0f;
    float lastFrame = 0.0f;

    auto updateAndDrawEntities = [&deltaTime, &projection, &camera](DrawableEntity& entity, const Shader& shader){
	shader.Use();
	shader.SetMat4("u_projection", projection);
	shader.SetMat4("u_view", camera.GetLookAtMatrix());

	entity.Update(deltaTime);
	entity.Draw(shader);
    };

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
            pNear,
            pFar
        );

	/* glm::mat4 cubeTrans(1.0f); */
	/* cubeTrans = glm::translate(cubeTrans, glm::vec3(0.0f, 3.0f, 0.0f)); */


	/* cubeShader.Use(); */
	/* cubeShader.SetMat4("model", cubeTrans); */
	/* cubeShader.SetMat4("view", camera.GetLookAtMatrix()); */
	/* cubeShader.SetMat4("projection", projection); */
	/* cubeShader.SetVec3("customColor", glm::vec3(1.0f, 1.0f, 1.0f)); */

	/* cubeMesh.Draw(); */

	/* glm::mat4 backpackTrans(1.0f); */
	/* backpackTrans = glm::translate(backpackTrans, glm::vec3(0.0f)); */
	
	/* modelShader.Use(); */
	/* modelShader.SetMat4("u_model", backpackTrans); */
	/* modelShader.SetMat4("u_view", camera.GetLookAtMatrix()); */
	/* modelShader.SetMat4("u_projection", projection); */


	/* backpack.Draw(modelShader); */

	cubeShader.Use();
	cubeShader.SetVec3("u_customColor", glm::vec3(1.0f));
	
	updateAndDrawEntities(cubeEntity, cubeShader);

	updateAndDrawEntities(backpackEntity, modelShader);

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
