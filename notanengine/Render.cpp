#include "Render.hpp"
#include "StaticMesh.hpp"

#include <glm/gtc/matrix_transform.hpp>

namespace Render
{
    void DrawMeshData(MeshData& meshData)
    {
	glBindVertexArray(meshData.VAO);
	
	if (meshData.UseIndexedDrawing)
	    glDrawElements(GL_TRIANGLES, meshData.NumIndices, GL_UNSIGNED_INT, 0);

	else
	    glDrawArrays(GL_TRIANGLES, 0, meshData.NumIndices);
    }

    void DrawStaticMesh(StaticMesh& mesh)
    {
        for (auto& meshData : mesh.GetSubMeshesRef())
        {
	    DrawMeshData(meshData);
        }
    }

    void DrawOutlineStaticMesh(StaticMesh& mesh, const Shader& defaultShader, const Shader& outlineShader, const glm::vec3& outlineColor)
    {
	for (auto& meshData : mesh.GetSubMeshesRef())
	{
	    glClear(GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
	    // Draw mesh without stencing
	    // Disable stencil (no frags are discarded)
	    glStencilFunc(GL_ALWAYS, 1, 0xFF);
	    glStencilMask(0xFF);

	    defaultShader.Use();
	    DrawMeshData(meshData);

	    // Draw outline
	    // enable stencil. now drawn mesh frags will have stenil value 1
	    glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
	    glStencilMask(0x00);
	    glDepthMask(0x00);
	    glDepthFunc(GL_LESS);

	    outlineShader.Use();
	    outlineShader.SetVec3("u_outlineColor", outlineColor);
	    DrawMeshData(meshData);

	    // return to default stencil
	    glStencilFunc(GL_ALWAYS, 1, 0xFF);
	    glStencilMask(0xFF);
	    glDepthMask(0xFF);
	    glDepthFunc(GL_LESS);
	}
    }

    void DrawOutlineEntity(Entity& entity, const Shader& defaultShader, const Shader& outlineShader, const glm::vec3& outlineColor, const Camera& camera, const glm::mat4& projection, float outlineFactor)
    {
	if (!entity.IsVisible())
	    return;

	DrawEntity(entity, defaultShader, camera, projection);

	defaultShader.Use();
	defaultShader.SetMat4("u_model", entity.Transform.GetTransformMatrix());
	defaultShader.SetMat4("u_view", camera.GetLookAtMatrix());
	defaultShader.SetMat4("u_projection", projection); 
	
	outlineShader.Use();
	const glm::mat4 scaledModel = glm::scale(entity.Transform.GetTransformMatrix(), glm::vec3(outlineFactor));
	outlineShader.SetMat4("u_model", scaledModel);
	outlineShader.SetMat4("u_view", camera.GetLookAtMatrix());
	outlineShader.SetMat4("u_projection", projection);

	DrawOutlineStaticMesh(entity.GetMeshRef(), defaultShader, outlineShader, outlineColor);
    }

    void DrawEntity(Entity& entity, const Shader& shader, const Camera& camera, const glm::mat4& projection)
    {
	if (!entity.IsVisible())
	    return;

	shader.Use();

        shader.SetMat4("u_model", entity.Transform.GetTransformMatrix());
        shader.SetMat4("u_view", camera.GetLookAtMatrix());
        shader.SetMat4("u_projection", projection);

        for (auto& meshData : entity.GetMeshRef().GetSubMeshesRef())
        {
            shader.SetBool("u_useMaterial", meshData.UseMaterial);

            if (meshData.UseMaterial && meshData.Mat.DiffuseMaps.empty())
            {
                shader.SetBool("u_useMaterial", false);
                meshData.UseMaterial = false;
            }

            else if (meshData.UseMaterial)
                shader.SetMaterial("u_material", meshData.Mat);

	    DrawMeshData(meshData);
        }
    }

    void UpdateAndDrawEntity(Entity &entity, const Shader &shader, float deltaTime, const Camera &camera, const glm::mat4 &projection)
    {
        entity.Update(deltaTime);
	DrawEntity(entity, shader, camera, projection); 
    }

    void UpdateAndDrawEntityMap(const EntityRenderMap &entities, float deltaTime, const Camera &camera, const glm::mat4 &projection)
    {
        for (auto& [name, tupleEntityShader] : entities)
        {
            Entity& entity = std::get<0>(tupleEntityShader);
            const Shader& shader = std::get<1>(tupleEntityShader);

            UpdateAndDrawEntity(entity, shader, deltaTime, camera, projection);
        }
    }
}
