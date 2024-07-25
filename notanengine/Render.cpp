#include "Render.hpp"

namespace Render
{
    void UpdateAndDrawEntity(Entity &entity, const Shader &shader, float deltaTime, const Camera &camera, const glm::mat4 &projection)
    {
        entity.Update(deltaTime);
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

            glBindVertexArray(meshData.VAO);

            if (meshData.UseIndexedDrawing)
                glDrawElements(GL_TRIANGLES, meshData.NumIndices, GL_UNSIGNED_INT, 0);
            else
                glDrawArrays(GL_TRIANGLES, 0, meshData.NumIndices);
        }
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
