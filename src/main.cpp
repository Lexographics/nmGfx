#include <iostream>
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#define TINYOBJLOADER_IMPLEMENTATION
#include "tiny_obj_loader.h"

// glad C/C++ Debug 3.3 core profile

#include "glm/gtc/matrix_transform.hpp"

#include "Core/Renderer.hpp"
#include "Core/GL/Model.hpp"
#include "Core/GL/Material.hpp"

int main(int argc, char const *argv[])
{
    nmGfx::Renderer renderer;
    renderer.Init(
        1280, 720, 1920, 1080, "nmGFX",
        nmGfx::WindowFlags::NONE);

    nmGfx::Window& window = renderer.GetWindow();
    // window.SetClearColor(0.3f, 0.4f, 0.5f);
    
    nmGfx::Model model;
    model.LoadFromFile("res/viking_room.obj");

    std::shared_ptr<nmGfx::Texture> tex = std::make_shared<nmGfx::Texture>();
    tex->LoadFromFile("res/viking_room.png");

    nmGfx::Material mat;
    mat.albedo = {0.2f, 0.7f, 0.1f, 1.f};
    mat.albedo_tex = tex;

    while(!window.ShouldClose())
    {
        window.PollEvents();
        
        glm::mat4 projection = glm::perspective(glm::radians(60.f), 16.0f / 9.0f, 0.1f, 500.f);
        glm::mat4 camera = glm::translate(glm::mat4(1.f), {0.f, 0.5f, 2.f});

        glm::mat4 modelTransform = glm::mat4(1.f);
        modelTransform = glm::rotate(modelTransform, glm::radians(180.f), {0.f, 1.f, 0.f});
        modelTransform = glm::rotate(modelTransform, glm::radians(-90.f), {1.f, 0.f, 0.f});

        renderer.Begin3D(projection, camera);
        renderer.DrawModel(model, modelTransform, mat, 12);
        // printf("ID: %i\n", renderer.Get3DPickID(window.GetVideoMousePosX(), window.GetVideoHeight() - window.GetVideoMousePosY()));
        renderer.End3D();

        renderer.Draw3DLayer();

        window.SwapBuffers();
    }
    

    return 0;
}
