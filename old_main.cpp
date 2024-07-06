// GLAD must always be included before any other header files that use OpenGL
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>

#include <iostream>
#include <cmath>

#include "Shader.hpp"


void framebuffer_resize_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);

glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 3.0f);
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);
float cameraSpeed = 2.5f;
float lastMouseX = 400.0f, lastMouseY = 300.0f;
float pitch = 0.0f, yaw = -90.0f;

float deltaTime = 0.0f, lastFrame = 0.0f;

int main()
{
    glfwInit();
    
    // Set OpenGL version 3.3 to be used
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    // Set Core-profile to be used
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);


    int WINDOW_WIDTH = 800;
    int WINDOW_HEIGHT = 600;
    // Create Window object
    GLFWwindow* window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Hello World!", nullptr, nullptr);
    if (!window)
    {
        std::cout << "Failed to create GLFW window\n";
        glfwTerminate();
        return -1;
    }
    // Set current context to this window
    glfwMakeContextCurrent(window);

    // Initialize GLAD
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD\n";
        return -1;
    }

    // Set viewport size (rendering window)
    glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);
    // set callback for when user resizes window
    glfwSetFramebufferSizeCallback(window, framebuffer_resize_callback);

    // set keyboard input callback
    //glfwSetKeyCallback(window, processInput);

    // hide cursor and capture it (to capture mouse input)
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    // set mouse callback
    glfwSetCursorPosCallback(window, mouse_callback);

    // enable depth testing
    glEnable(GL_DEPTH_TEST);

    // Triangle triVertices
    float triVertices[] = {
        // positions         //colors
        -0.5f, -0.5f, 0.0f,  1.0f, 0.0f, 0.0f, // bottom right
         0.5f, -0.5f, 0.0f,  0.0f, 1.0f, 0.0f, // bottom left
         0.0f,  0.5f, 0.0f,  0.0f, 0.0f, 1.0f  // top
    };

    // Generate Vertex Array Object
    unsigned int triVAO = 0;
    glGenVertexArrays(1, &triVAO);
    // bind the VAO
    glBindVertexArray(triVAO);

    // Generate Vertex Buffer Object
    unsigned int triVBO = 0;
    glGenBuffers(1, &triVBO);

    // Bind VBO spcecifying its type of Buffer
    glBindBuffer(GL_ARRAY_BUFFER, triVBO);
    // Copy our triVertices array to the currently bound buffer
    /* ARGS:
    *  1st: Type of the buffer (VBO is of type GL_ARRAY_BUFFER)
    *  2nd: Size of the data in bytes
    *  3rd: Data to be sent
    *  4th: How the GPU should manage this data (GL_STREAM_DRAW, GL_STATIC_DRAW, GL_DYNAMIC_DRAW)
    */
    glBufferData(GL_ARRAY_BUFFER, sizeof(triVertices), triVertices, GL_STATIC_DRAW);


    // specify vertex attributes for the data passed
    // vertex attrib 0 is for position
    /* ARGS:
    *  1st: Location of which vertex attribute to configure (as in `layout (location=0)` in the vertex shader)
    *  2nd: Size of the vertex attribute. Since it's a vec3, we use 3.
    *  3rd: Type of the data.
    *  4th: If the data should be normalized (only for integer,byte values)
    *  5th: Stride. Specifies the space between consecutive vertex attributes.
    *  6th: void* offset of where the vertex attrib begins.
    */
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    // enable the vertex attrib (giving its location)
    glEnableVertexAttribArray(0);

    // vertex attrib 1 is for color
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3*sizeof(float)));
    glEnableVertexAttribArray(1);

    // unbind vbo and vao
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);


    // ***TEXTURE FOR THE RECTANGLE***
    // activate texture unit 0
    glActiveTexture(GL_TEXTURE0);
    // generating OpenGL texture
    unsigned int texture{};
    glGenTextures(1, &texture);
    // bind texture (from here on any subsequent texture commands will configure the currently bound texture)
    glBindTexture(GL_TEXTURE_2D, texture);

    // set texture wrapping/filtering options (on the currently bound texture)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // most textures use 0.0 in the upper y, but opengl flips it
    stbi_set_flip_vertically_on_load(true);

    // load image data
    int width, height, nrChannels;
    unsigned char* data = stbi_load("textures/container.jpg", &width, &height, &nrChannels, 0);

    // load data into texture
    /* ARGS:
    *  1st: Texture target (2D texture in this case)
    *  2nd: Mipmap level, if settings mipmap level manually
    *  3rd: Format to store the image. (RGB values in this case)
    *  4th: width of resulting texture
    *  5th: height of resulting texture
    *  6th: always 0 (legacy stuff)
    *  7th: format of source image
    *  8th: datatype of the source image
    *  9th: loaded image data
    */
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);

    // generate mipmaps (currently bound texture)
    glGenerateMipmap(GL_TEXTURE_2D);

    // free image memory
    stbi_image_free(data);

    // second texture
    // activate texture unit 1
    glActiveTexture(GL_TEXTURE1);

    unsigned int texture1{};
    glGenTextures(1, &texture1);
    glBindTexture(GL_TEXTURE_2D, texture1);

    // set texture wrapping/filtering options (on the currently bound texture)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // load second image data
    data = stbi_load("textures/roma.png", &width, &height, &nrChannels, 0);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);

    stbi_image_free(data);

    // ***Rectangle Using EBOs***
    // Rectangle vertices (overlaping vertices)
    // float rectVertices[] = {
    //     // first triangle
    //      0.5f,  0.5f, 0.0f, // top right
    //      0.5f, -0.5f, 0.0f, // bottom right 
    //     -0.5f,  0.5f, 0.0f, // top left
    //     // second triangle
    //      0.5f, -0.5f, 0.0f, // bottom right
    //     -0.5f, -0.5f, 0.0f, // bottom left
    //     -0.5f,  0.5f, 0.0f
    // };
    // Rectangle vertices (using indexed drawing)
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

    // Generate Vertex Array object for the rectangle and bind it
    unsigned int rectVAO = 0;
    glGenVertexArrays(1, &rectVAO);
    glBindVertexArray(rectVAO);

    // Generate VBO for the rectangle, copy data and bind it
    unsigned int rectVBO = 0;
    glGenBuffers(1, &rectVBO);
    glBindBuffer(GL_ARRAY_BUFFER, rectVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(rectVertices), rectVertices, GL_STATIC_DRAW);

    // Generate Element Buffer Object
    unsigned int rectEBO = 0;
    glGenBuffers(1, &rectEBO);
    // bind ebo and copy the indices data to it
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, rectEBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(rectIndices), rectIndices, GL_STATIC_DRAW);

    // specify vertex attributes for the rectangle data
    // position vertex attribute (location 0)
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8*sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);


    // color vertex attribute (location 1)
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8*sizeof(float), (void*)(3*sizeof(float)));
    glEnableVertexAttribArray(1);

    // texture coord vertex attribute (location 2)
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8*sizeof(float), (void*)(6*sizeof(float)));
    glEnableVertexAttribArray(2);

    // unbind vao and vbo
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);


    float cubeVertices[] = {
        -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
        0.5f, -0.5f, -0.5f,  1.0f, 0.0f,
        0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
        0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
        -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,

        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
        0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
        0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
        0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
        -0.5f,  0.5f,  0.5f,  0.0f, 1.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,

        -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
        -0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
        -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

        0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
        0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
        0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
        0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
        0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
        0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
        0.5f, -0.5f, -0.5f,  1.0f, 1.0f,
        0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
        0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,

        -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
        0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
        0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
        0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
        -0.5f,  0.5f,  0.5f,  0.0f, 0.0f,
        -0.5f,  0.5f, -0.5f,  0.0f, 1.0f
    };


    

    unsigned int cubeVAO = 0, cubeVBO = 0;
    glGenVertexArrays(1, &cubeVAO);
    glGenBuffers(1, &cubeVBO);

    glBindVertexArray(cubeVAO);

    glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVertices), cubeVertices, GL_STATIC_DRAW);
    
    // cube position vertex attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5*sizeof(float), (void*)(0));
    glEnableVertexAttribArray(0);

    // cube texture coord vertex attrib
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 5*sizeof(float), (void*)(3*sizeof(float)));
    glEnableVertexAttribArray(2);

    // Create our shader
    Shader shader("shaders/old_vertex.glsl", "shaders/old_frag.glsl");
    shader.Use();

    // set textures samples to use 0 and 1 texture units
    shader.SetInt("texture0", 0);
    shader.SetInt("texture1", 1);



    // ******GOING 3D******
    // Model matrix
    glm::mat4 model = glm::mat4(1.0f);
    //model = glm::rotate(model, glm::radians(-55.0f), glm::vec3(1.0f, 0.0f, 0.0f));

    // Projection matrix (in this case perspective)
    glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)WINDOW_WIDTH / (float)WINDOW_HEIGHT, 0.1f, 100.0f);


    // whole lot of fucking cubes
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

    // Initialize program loop
    while (!glfwWindowShouldClose(window))
    {
        float currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        processInput(window);
    
        // Clear the screen with this color
        // glClearColor is a state-setting function
        glClearColor(0.3f, 0.3f, 0.3f, 1.0f);
        // glClear is a state-using function that uses the current state to retrieve the clearing color
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // activate the shader program
        shader.Use();

        shader.SetMat4("model", model);

        glm::mat4 view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
        shader.SetMat4("view", view);
        shader.SetMat4("projection", projection);

        /* UNIFORM USAGE */
        // float time = glfwGetTime();
        // float green = (std::sin(time) / 2.0f) + 0.5f;
        // int uniformVertexColorLoc = glGetUniformLocation(shaderProgram, "ourColor");
        // glUniform4f(uniformVertexColorLoc, 0.0f, green, 0.0f, 1.0f);

        // uncomment to draw triangle
        // Bind the triangle's VAO
        //glBindVertexArray(triVAO);  
        // Draw a GL_TRIANGLE with starting index 0 and 3 triVertices
        //glDrawArrays(GL_TRIANGLES, 0, 3);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture);

        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, texture1);

        // draw a fucking lot of cubes
        glBindVertexArray(cubeVAO);
        for (int i = 0; i < 10; i++)
        {
            model = glm::mat4(1.0f);
            model = glm::translate(model, cubePositions[i]);
            
            float angle = glm::sin((float)glfwGetTime()) * (i+1) * glm::radians(20.0f);
            model = glm::rotate(model, angle, glm::vec3(1.0f, 0.3f, 0.5f));

            shader.SetMat4("model", model);
            glDrawArrays(GL_TRIANGLES, 0, 36);
        }

        // uncomment to draw rectangle
        //lBindVertexArray(rectVAO);
        //glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}

void framebuffer_resize_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}

void processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    else if (glfwGetKey(window, GLFW_KEY_APOSTROPHE) == GLFW_PRESS)
    {
        static bool polyFill = true;
        if (polyFill)
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        else
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        
        polyFill = !polyFill;
    }

    cameraSpeed = 2.5f * deltaTime;

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        cameraPos += cameraFront * cameraSpeed;
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        cameraPos -= cameraFront * cameraSpeed;
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
        cameraPos += cameraUp * cameraSpeed;
    if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
        cameraPos -= cameraUp * cameraSpeed;
}

void mouse_callback(GLFWwindow *window, double xpos, double ypos)
{
    static bool firstMouse = true;
    if (firstMouse)
    {
        lastMouseX = static_cast<float>(xpos);
        lastMouseY = static_cast<float>(ypos);
        firstMouse = false;
    }

    float xoffset = static_cast<float>(xpos) - lastMouseX;
    float yoffset = static_cast<float>(ypos) - lastMouseY;
    yoffset *= -1.0f;
    lastMouseX = xpos;
    lastMouseY = ypos;

    const float sensitivity = 0.1f;
    xoffset *= sensitivity;
    yoffset *= sensitivity;

    yaw += xoffset;
    pitch += yoffset;

    if (pitch > 89.0f)
        pitch = 89.0f;
    if (pitch < -89.0f)
        pitch = -89.0f;
    
    if (yaw >= 360.0f || yaw <= -360.0f)
        yaw = 0.0f;

    glm::vec3 direction{};
    direction.x = std::cos(glm::radians(yaw)) * std::cos(glm::radians(pitch));
    direction.y = std::sin(glm::radians(pitch));
    direction.z = std::sin(glm::radians(yaw)) * std::cos(glm::radians(pitch));
    cameraFront = glm::normalize(direction);
}
