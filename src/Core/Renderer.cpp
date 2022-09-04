#include "Renderer.hpp"

#include <vector>
#include <string.h>
#include "Core/Renderer.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtx/vector_angle.hpp"
#include "glad/glad.h"
#include <GLFW/glfw3.h>
#include "Core/Matrix.hpp"

namespace nmGfx
{
    void Renderer::Init(int windowWidth, int windowHeight, int videoWidth, int videoHeight, const char* title, WindowFlags flags)
    {
        _window = nmGfx::Window(windowWidth, windowHeight, videoWidth, videoHeight, title, flags);


        _data2d._shader.LoadFile("res/default2d.glsl");
        _data2d._frameBuffer.Create2DDefault(&_window, videoWidth, videoHeight);

        _data3d._shader.LoadFile("res/default.glsl");
        _fullscreenShader.LoadFile("res/fullscreen.glsl");

        _data3d._gBuffer.CreateGBuffer(&_window, videoWidth, videoHeight);
        
        unsigned char pixel[3] = { 255, 255, 255 };
        _whiteTexture.LoadFromData(pixel, 1, 1, 3);

        { // fullscreen quad
            static const float fullscreen_vertices[] = {
                /* aPos */     /* UV */
                -1.0f,  1.0f,    0.f, 1.f,
                -1.0f, -1.0f,    0.f, 0.f,
                1.0f, -1.0f,    1.f, 0.f,
                1.0f,  1.0f,    1.f, 1.f,
            };  
            static const unsigned int fullscreen_indices[] = {
                0, 1, 2,
                0, 2, 3
            };

            _fullscreenModel.Create();
            _fullscreenModel.ResetAttributes();
            std::vector<float> data(sizeof(fullscreen_vertices) / sizeof(float));
            std::vector<uint32_t> indices_vec(sizeof(fullscreen_indices) / sizeof(uint32_t));
            memcpy(data.data(), fullscreen_vertices, sizeof(fullscreen_vertices));
            memcpy(indices_vec.data(), fullscreen_indices, sizeof(fullscreen_indices));
            _fullscreenModel.SetModelData(data);
            _fullscreenModel.SetIndexData(indices_vec);
            _fullscreenModel.SetAttribute(0, AttributeType::VEC2);
            _fullscreenModel.SetAttribute(1, AttributeType::VEC2);
            _fullscreenModel.UploadAttributes();
        }

        { // 2d model quad
            static const float quad_vertices[] = {
                  /* aPos */       /* UV */
                -0.5f,   0.5f,    0.f, 1.f,
                -0.5f,  -0.5f,    0.f, 0.f,
                 0.5f,  -0.5f,    1.f, 0.f,
                 0.5f,   0.5f,    1.f, 1.f,
            };  
            static const unsigned int quad_indices[] = {
                0, 1, 2,
                0, 2, 3
            };

            _data2d._model2d.Create();
            _data2d._model2d.ResetAttributes();
            std::vector<float> data(sizeof(quad_vertices) / sizeof(float));
            std::vector<uint32_t> indices_vec(sizeof(quad_indices) / sizeof(uint32_t));
            memcpy(data.data(), quad_vertices, sizeof(quad_vertices));
            memcpy(indices_vec.data(), quad_indices, sizeof(quad_indices));
            _data2d._model2d.SetModelData(data);
            _data2d._model2d.SetIndexData(indices_vec);
            _data2d._model2d.SetAttribute(0, AttributeType::VEC2);
            _data2d._model2d.SetAttribute(1, AttributeType::VEC2);
            _data2d._model2d.UploadAttributes();
        }
    }

    Renderer::~Renderer()
    {
        // glfwTerminate();
    }


    void Renderer::ClearLayers()
    {
        _window.UnbindFramebuffer();
        glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
        glDisable(GL_DEPTH_TEST);
        glClear(GL_COLOR_BUFFER_BIT);
    }

    void Renderer::Begin3D(const glm::mat4 projectionMatrix, const glm::mat4 cameraTransform)
    {
        _data3d._gBuffer.Use();
        glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
        glEnable(GL_DEPTH_TEST);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        _data3d._shader.Use();

        _data3d._projectionMatrix = projectionMatrix;
        _data3d._viewMatrix = glm::inverse(cameraTransform);

        glm::mat4 view_proj = _data3d._projectionMatrix * _data3d._viewMatrix;
        _data3d._shader.UniformMat4("uViewProjection", view_proj);
    }

    void Renderer::End3D()
    {
        _window.UnbindFramebuffer();
    }

    int Renderer::Get3DPickID(int x, int y)
    {
        int id = 0;
        glReadBuffer(GL_COLOR_ATTACHMENT3);
        glReadPixels(x, y, 1, 1, GL_RED_INTEGER, GL_INT, &id);
        return id;
    }

    int Renderer::Get3DPickIDSafe(int x, int y)
    {
        _data3d._gBuffer.Use();
        int id = Get3DPickID(x, y);
        _window.UnbindFramebuffer();

        return id;
    }

    void Renderer::DrawModel(const Model& model, const glm::mat4& transform, const Material& material, int drawID /*= 0*/)
    {
        _data3d._shader.UniformMat4("uModel", transform);
        // Material (conditions in textures are for one lining  'if texture is null, bind white texture')
        _data3d._shader.UniformVec4("uMat_Albedo", material.albedo);
        _data3d._shader.UniformTexture("uMat_AlbedoTex", material.albedo_tex ? *(material.albedo_tex) : _whiteTexture, 0);
        _data3d._shader.UniformFloat("uMat_Specular", material.specular);
        _data3d._shader.UniformTexture("uMat_SpecularTex", material.specular_tex ? *(material.specular_tex) : _whiteTexture, 1);

        _data3d._shader.UniformInt("uDrawID", drawID);
        model.Draw();
    }

    void Renderer::Draw3DLayer()
    {
        glm::mat4 fullproj = glm::ortho(0.f, (float)_window.GetWindowWidth(), 0.f, (float)_window.GetWindowHeight(), 0.f, 10.f); // no view matrix
        _fullscreenShader.Use();
        _fullscreenShader.UniformMat4("uModel", glm::mat4(1.f));
        _fullscreenShader.UniformMat4("uViewProjection", fullproj);
        _fullscreenShader.UniformTexture("gAlbedo", _data3d._gBuffer._gAlbedo, 0);
        _fullscreenShader.UniformTexture("gPosition", _data3d._gBuffer._gPosition, 1);
        _fullscreenShader.UniformTexture("gNormal", _data3d._gBuffer._gNormal, 2);
        
        _fullscreenModel.Draw();
    }


    void Renderer::Begin2D(const glm::mat4 cameraTransform)
    {
        _data2d._frameBuffer.Use();
        glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
        glEnable(GL_DEPTH_TEST);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        _data2d._shader.Use();

        float halfwidth = (float)_window.GetVideoWidth() / 2.f;
        float halfheight = (float)_window.GetVideoHeight() / 2.f;
        _data2d._projectionMatrix = CalculateOrtho(-halfwidth, halfwidth, halfheight, -halfheight, 0.f, 10.f);
        _data2d._viewMatrix = glm::inverse(cameraTransform);

        glm::mat4 view_proj = _data2d._projectionMatrix * _data2d._viewMatrix;
        _data2d._shader.UniformMat4("uViewProjection", view_proj);
    }

    void Renderer::End2D()
    {
        _window.UnbindFramebuffer();
    }

    void Renderer::DrawTexture(std::shared_ptr<Texture> texture, const glm::mat4& transform, int drawID /*= 0*/)
    {
        _data2d._shader.UniformMat4("uModel", transform);
        _data2d._shader.UniformTexture("uTexture", texture ? *(texture) : _whiteTexture, 1);

        _data2d._shader.UniformInt("uDrawID", drawID);
        _data2d._model2d.Draw();
    }

    void Renderer::Draw2DLayer()
    {
        glm::mat4 fullproj = glm::ortho(0.f, (float)_window.GetWindowWidth(), 0.f, (float)_window.GetWindowHeight(), 0.f, 10.f); // no view matrix
        _fullscreenShader.Use();
        _fullscreenShader.UniformMat4("uModel", glm::mat4(1.f));
        _fullscreenShader.UniformMat4("uViewProjection", fullproj);
        _fullscreenShader.UniformTexture("gAlbedo", _data2d._frameBuffer._gAlbedo, 0);
        
        _fullscreenModel.Draw();
    }
} // namespace nmGfx
