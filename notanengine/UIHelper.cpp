#include "UIHelper.hpp"

#include <glm/gtc/type_ptr.hpp>


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

    void EntityPropertiesManager(const std::unordered_map<std::string, std::tuple<Entity&, const Shader&>>& entities)
    {
	ImGui::Begin("Entity Properties");

	unsigned int entityId = 0;
	for (auto& [name, tupleEntityShader] : entities)
	{
	    Entity& entity = std::get<0>(tupleEntityShader);

	    ImGui::Text("%s properties", name.c_str());

	    const std::string id = std::to_string(entityId);
	    std::string labelId = std::string("Position##") + id;
	    ImGui::InputFloat3(labelId.c_str(), glm::value_ptr(entity.Transform.GetPositionRef()));

	    labelId = std::string("Rotation##") + id;
	    ImGui::InputFloat3(labelId.c_str(), glm::value_ptr(entity.Transform.GetRotationRef()));

	    labelId = std::string("Scale##") + id; 
	    ImGui::InputFloat3(labelId.c_str(), glm::value_ptr(entity.Transform.GetScaleRef()));

	    bool visibility = entity.IsVisible();
	    const std::string visibleLabel = std::string("Is visible##") + id;
	    ImGui::Checkbox(visibleLabel.c_str(), &visibility);
	    entity.SetVisible(visibility);

	    entityId++;
	}

	ImGui::End();
    }

    void DirectionalLightPropertiesManager(DirectionalLight& dirLight)
    {
	ImGui::Begin("Direcional Light Properties");

	ImGui::SliderFloat3("Direction##dirLight", glm::value_ptr(dirLight.Direction), -360.0f, 360.0f);
	ImGui::SliderFloat3("Ambient##dirLight", glm::value_ptr(dirLight.Ambient), -360.0f, 360.0f);
	ImGui::SliderFloat3("Diffuse##dirLight", glm::value_ptr(dirLight.Diffuse), -360.0f, 360.0f);
	ImGui::SliderFloat3("Specular##dirLight", glm::value_ptr(dirLight.Specular), -360.0f, 360.0f);

	ImGui::End();
    }
}
