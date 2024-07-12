#include "UIHelper.hpp"



namespace UIHelper
{
    void Init(GLFWwindow* glfwWindow)
    {
        // Dear ImGui context setup
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGuiIO& io = ImGui::GetIO(); (void)io;
        //io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
        // setup imgui style
        ImGui::StyleColorsDark();
        // setup platform/rendered backends
        ImGui_ImplGlfw_InitForOpenGL(glfwWindow, true);
        ImGui_ImplOpenGL3_Init("#version 330");
    }
    
    void NewFrame()
    {
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
    }

    void Render()
    {
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    }
    
    void Terminate()
    {
        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplGlfw_Shutdown();
        ImGui::DestroyContext();
    }

    void FrameStatsWindow(float deltaTime)
    {
         // Frame stats window (ms/frame, FPS)
        ImGui::Begin("Frame stats");

        ImGui::Text("Time per frame: %f ms", deltaTime*1000);
        ImGui::Text("FPS: %f", 1.0f/deltaTime);

        ImGui::End();
    }
}
