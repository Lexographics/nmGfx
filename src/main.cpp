#ifdef NMGFX_BUILD_PLAYGROUND

#define NMGFX_PRINT_MESSAGES

#include <iostream>
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#define TINYOBJLOADER_IMPLEMENTATION
#include "tiny_obj_loader.h"


#include "glad/glad.h"

// glad C/C++ Debug 3.3 core profile

#include "glm/gtc/matrix_transform.hpp"

#include "Core/nm_Renderer.hpp"
#include "Core/GL/nm_Model.hpp"
#include "Core/GL/nm_Material.hpp"
#include "Core/nm_Matrix.hpp"
#include "Core/nm_Shader.hpp"
#include "Core/GL/nm_Font.hpp"

int main(int argc, char const *argv[])
{
    nmGfx::Renderer renderer;
    renderer.Init(
        1280, 720, 1920, 1080, "nmGFX",
        nmGfx::WindowFlags::NONE);

    nmGfx::Window& window = renderer.GetWindow();


    renderer.GetData2D()._shader.LoadFile("res/default2d.glsl");
    renderer.GetData3D()._shader.LoadFile("res/default.glsl");
    renderer.GetData3D()._skyboxShader.LoadFile("res/skybox.glsl");
    renderer.GetDataFullscreen()._shader.LoadFile("res/fullscreen.glsl");

    nmGfx::Shader textShader;
    textShader.LoadFile("res/text.glsl");


    glm::mat4 projection = nmGfx::CalculateProjectionMatrix(1920, 1080, .5f, .5f, 0.f, 10.f);
    textShader.UniformMat4("projection", projection);
    textShader.UniformVec3("textColor", {1.f, 1.f, 1.f});
    
    nmGfx::Model model;
    model.LoadFromFile("res/viking_room.obj");

    std::shared_ptr<nmGfx::Texture> tex = std::make_shared<nmGfx::Texture>();
    tex->Load2DFromFile("res/viking_room.png");

    std::shared_ptr<nmGfx::Texture> tex2d = std::make_shared<nmGfx::Texture>();
    tex2d->Load2DFromFile("res/image.jpeg");

    nmGfx::Font font;
    if(!renderer.LoadFont(&font, "res/Roboto-Medium.ttf")) {
        std::cout << "Can not load font" << std::endl;
    }

    nmGfx::Framebuffer mainPass;
    mainPass.Create2DDefault(&window, 1920, 1080);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    nmGfx::Material mat;
    mat.albedo = {0.2f, 0.7f, 0.1f, 1.f};
    mat.albedo_tex = tex;
    float t = 0.f;
    while(!window.ShouldClose())
    {
        window.PollEvents();

        glm::mat4 camera = glm::translate(glm::mat4(1.f), {0.f, 0.5f, 2.f});

        renderer.Begin3D(nmGfx::CalculatePerspective((float)window.GetVideoWidth() / (float)window.GetVideoHeight(), 60.f, 0.1f, 500.f), camera);
        renderer.DrawModel(model, nmGfx::CalculateModelMatrix({0.f, 0.f, 0.f}, {90.f, 180.f, 90.f}, {1.f, 1.f, 1.f}), mat, 12);
        // printf("3D ID: %i\n", renderer.Get3DPickID(window.GetVideoMousePosX(), window.GetVideoHeight() - window.GetVideoMousePosY()));
        renderer.End3D();

        
        renderer.Begin2D(nmGfx::CalculateModelMatrix({0.f, 0.f, 1.f}, {.0f, .0f, t*10}, {1.f, 1.f, 1.f}));
        renderer.DrawTexture(tex2d.get(), nmGfx::CalculateModelMatrix({-200.f, 0.f}, 0, {tex2d->GetWidth() * 2.3f, tex2d->GetHeight() * 2.3f}), {0.f, 1.f, 1.f}, 13);
        renderer.DrawTexture(tex2d.get(), nmGfx::CalculateModelMatrix({200.f, 0.f}, 0, {tex2d->GetWidth() * 2.3f, tex2d->GetHeight() * 2.3f}), {1.f, 0.f, 1.f}, 14);

        // printf("2D ID: %i\n", renderer.Get2DPickID(window.GetVideoMousePosX(), window.GetVideoHeight() - window.GetVideoMousePosY()));
        textShader.UniformMat4("model", nmGfx::CalculateModelMatrix({200.f, 200.f}, t*100, {1.f, 1.f}, {0.f, 0.f}));
        renderer.DrawText(textShader, font, "siuuuuuuuuuuuuuu");
        renderer.End2D();

        renderer.BeginPass(mainPass);
        renderer.SetDepthTesting(false);
        renderer.ClearColor();

        textShader.UniformMat4("model", nmGfx::CalculateModelMatrix({0.f, 0.f}, 0.f, {1.f, 1.f}, {0.f, 0.f}));
        renderer.DrawText(textShader, font, "muchas gracias aficion");
        auto size = renderer.CalcTextSize(font, "muchas gracias aficion");

        textShader.UniformMat4("model", nmGfx::CalculateModelMatrix({size.x, size.y}, 0.f, {1.f, 1.f}, {0.f, 0.f}));
        renderer.DrawText(textShader, font, "este para vosotros");

        renderer.EndPass();


        t += 0.01f;

        renderer.ClearLayers();
        // renderer.Draw3DLayer();
        renderer.Draw2DLayer();
        renderer.DrawPassLayer(mainPass);

        window.SwapBuffers();
    }
    

    return 0;
}

#endif