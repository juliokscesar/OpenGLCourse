#include "UIHelper.hpp"
#include "glm/gtc/type_ptr.hpp"



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

    void EntityPropertiesManager(const std::unordered_map<std::string, DrawableEntity&>& entities)
    {
	ImGui::Begin("Entity Properties");

	unsigned int entityId = 0;
	for (auto& [name, entity] : entities)
	{
	    ImGui::Text("%s properties", name.c_str());

	    const std::string id = std::to_string(entityId);
	    std::string labelId = std::string("Position##") + id;
	    ImGui::InputFloat3(labelId.c_str(), glm::value_ptr(entity.Transform.GetPositionRef()));

	    labelId = std::string("Rotation##") + id;
	    ImGui::InputFloat3(labelId.c_str(), glm::value_ptr(entity.Transform.GetRotationRef()));

	    labelId = std::string("Scale##") + id; 
	    ImGui::InputFloat3(labelId.c_str(), glm::value_ptr(entity.Transform.GetScaleRef()));

	    entityId++;
	}

	ImGui::End();
    }
}
