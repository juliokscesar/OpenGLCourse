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
	static Entity* selectedEntity = nullptr;
	unsigned int selectedId = 0;
        for (auto& [name, tupleEntityShader] : entities)
        {
	    const std::string entityButton = name + "##" + std::to_string(entityId);
            Entity& entity = std::get<0>(tupleEntityShader);

	    if (ImGui::Button(entityButton.c_str()))
	    {
		selectedEntity = &entity;
		selectedId = entityId;
	    }
	    ImGui::SameLine();

	    entityId++;
        }

	if (selectedEntity)
	{
            ImGui::Text("Properties");

            const std::string id = std::to_string(selectedId);
            std::string labelId = std::string("Position##") + id;
            ImGui::InputFloat3(labelId.c_str(), glm::value_ptr(selectedEntity->Transform.GetPositionRef()));

            labelId = std::string("Rotation##") + id;
            ImGui::InputFloat3(labelId.c_str(), glm::value_ptr(selectedEntity->Transform.GetRotationRef()));

            labelId = std::string("Scale##") + id; 
            ImGui::InputFloat3(labelId.c_str(), glm::value_ptr(selectedEntity->Transform.GetScaleRef()));

            bool visibility = selectedEntity->IsVisible();
            const std::string visibleLabel = std::string("Is visible##") + id;
            ImGui::Checkbox(visibleLabel.c_str(), &visibility);
            selectedEntity->SetVisible(visibility);

	    unsigned int materialId = 0;
	    for (auto& mesh : selectedEntity->GetMeshRef().GetSubMeshesRef())
	    {
		if (!mesh.UseMaterial)
		    continue;

		ImGui::Text("Material %i properties", materialId);

		const std::string id = std::to_string(materialId);
		std::string label = "Shininess##" + id;
		ImGui::SliderFloat(label.c_str(), &mesh.Mat.Shininess, 0.1f, 512.0f);

		label = "Tiling Factor##" + id;
		ImGui::SliderFloat(label.c_str(), &mesh.Mat.TilingFactor, 0.5f, 10.0f);

		materialId++;
	    }
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

    void CameraAndProjectionPropertiesManager(Camera& camera, float& pNear, float& pFar)
    {
        ImGui::Begin("Camera and Projection Properties");

        ImGui::SliderFloat("FOV", &camera.FOV, 1.0f, 180.0f);
        ImGui::SliderFloat("Near Plane", &pNear, 0.0f, 100.0f);
        ImGui::SliderFloat("Far Plane", &pFar, 10.0f, 1000.0f);

        ImGui::End();
    }
}
