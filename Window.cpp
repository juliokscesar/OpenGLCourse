#include "Window.hpp"

#include <stdexcept>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <imgui/imgui.h>
#include <imgui/backends/imgui_impl_glfw.h>
#include <imgui/backends/imgui_impl_opengl3.h>

#include <stb/stb_image.h>
#include <unordered_map>

#include "Shader.hpp"
#include "Input.hpp"
#include "Camera.hpp"
#include "StaticMesh.hpp"
#include "Entity.hpp"
#include "ModelFactory.hpp"
#include "UIHelper.hpp"

static bool g_bResized = false;
static struct {int newWidth; int newHeight; } g_updatedProperties;
void framebufferResizeCallback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
    g_bResized = true;
    
    g_updatedProperties.newWidth = width;
    g_updatedProperties.newHeight = height;
}


void updateAndDrawEntity(Entity& entity, const Shader& shader, float deltaTime, const Camera& camera, const glm::mat4& projection)
{
    entity.Update(deltaTime);

    shader.Use();

    shader.SetMat4("u_model", entity.Transform.GetTransformMatrix());
    shader.SetMat4("u_view", camera.GetLookAtMatrix());
    shader.SetMat4("u_projection", projection);

    for (auto& meshData : entity.GetMeshRef().GetSubMeshesRef())
    {
	shader.SetBool("u_useMaterial", meshData.UseMaterial);
	if (meshData.UseMaterial)
	    shader.SetMaterial("u_material", meshData.Mat);

	glBindVertexArray(meshData.VAO);
	glDrawElements(GL_TRIANGLES, meshData.NumIndices, GL_UNSIGNED_INT, 0);
    }
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
    std::vector<float> cubeVertices={
        // positions        // texcoords (2D TEXTURE)
        // BACK
        -0.5f, -0.5f, -0.5f,  1.0f, 0.0f, // A 0 
         0.5f, -0.5f, -0.5f,  0.0f, 0.0f, // B 1 
         0.5f,  0.5f, -0.5f,  0.0f, 1.0f, // C 2 
        -0.5f,  0.5f, -0.5f,  1.0f, 1.0f, // D 3 

        // RIGHT
         0.5f, -0.5f, -0.5f,  1.0f, 0.0f, // B 4 
         0.5f,  0.5f, -0.5f,  1.0f, 1.0f, // C 5 
         0.5f,  0.5f,  0.5f,  0.0f, 1.0f, // F 6 
         0.5f, -0.5f,  0.5f,  0.0f, 0.0f, // G 7 

        // FRONT
        -0.5f,  0.5f,  0.5f,  0.0f, 1.0f, // E 8 
         0.5f,  0.5f,  0.5f,  1.0f, 1.0f, // F 9 
         0.5f, -0.5f,  0.5f,  1.0f, 0.0f, // G 10 
        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f, // H 11 

        // TOP
         0.5f,  0.5f, -0.5f,  1.0f, 1.0f, // C 12 
        -0.5f,  0.5f, -0.5f,  0.0f, 1.0f, // D 13 
        -0.5f,  0.5f,  0.5f,  0.0f, 0.0f, // E 14 
         0.5f,  0.5f,  0.5f,  1.0f, 0.0f, // F 15 

        // LEFT
        -0.5f, -0.5f, -0.5f,  0.0f, 0.0f, // A 16
        -0.5f,  0.5f, -0.5f,  0.0f, 1.0f, // D 17
        -0.5f,  0.5f,  0.5f,  1.0f, 1.0f, // E 18
        -0.5f, -0.5f,  0.5f,  1.0f, 0.0f, // H 19

        // BOTTOM
        -0.5f, -0.5f, -0.5f,  0.0f, 0.0f, // A 20 
         0.5f, -0.5f, -0.5f,  1.0f, 0.0f, // B 21 
         0.5f, -0.5f,  0.5f,  1.0f, 1.0f, // G 22 
        -0.5f, -0.5f,  0.5f,  0.0f, 1.0f  // H 23
    };

    std::vector<unsigned int> cubeIndices = {
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

    std::vector<VertexAttribProperties> cubeVertexAttributes = {
	{ 0, 3, 5*sizeof(float), 0  },
	{ 2, 2, 5*sizeof(float), 3*sizeof(float) }
    };

    Texture2D cubeTexture = ObjectLoader::LoadTextureFromFile("textures/container.jpg");

    Material cubeMaterial;
    cubeMaterial.DiffuseMaps.push_back(cubeTexture);

    StaticMesh cubeMesh(
	cubeVertices,
	cubeIndices,
	cubeVertexAttributes,
	cubeMaterial
    );

    Entity cubeEntity(cubeMesh);

    StaticMesh noMatCubeMesh(cubeVertices, cubeIndices, cubeVertexAttributes);
    Entity noMatCubeEntity(noMatCubeMesh);

    ObjectLoader::Model backpackModel = ObjectLoader::LoadModel("models/backpack/backpack.obj");
    StaticMesh backpackMesh(backpackModel.Mesh);
    Entity backpackEntity(backpackMesh);


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

    
    Shader basicShader("shaders/basic_shader.vert", "shaders/basic_shader.frag");
    

    
    std::unordered_map<std::string, Entity&> entitiesNamesMap = {
	{"Cube", cubeEntity},
	{"NoMatCube", noMatCubeEntity},
	{"Backpack", backpackEntity}
    };

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
	
	UIHelper::EntityPropertiesManager(entitiesNamesMap);

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

		
	updateAndDrawEntity(cubeEntity, basicShader, deltaTime, camera, projection);
	updateAndDrawEntity(noMatCubeEntity, basicShader, deltaTime, camera, projection);
	updateAndDrawEntity(backpackEntity, basicShader, deltaTime, camera, projection);

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
    // also, getting state variables is expensive, so we keep track of the new
    // widht and height through a unnamed struct g_updatedProperties
    m_width = g_updatedProperties.newWidth;
    m_height = g_updatedProperties.newHeight;

    m_aspectRatio = (float)m_width / (float)m_height;

    g_bResized = false;
}
