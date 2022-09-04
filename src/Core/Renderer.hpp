#ifndef __NM_GFX_RENDERER_HPP__
#define __NM_GFX_RENDERER_HPP__
#pragma once

#include <memory>

#include <glm/glm.hpp>

#include "Core/Window.hpp"
#include "Core/GL/Model.hpp"
#include "Core/Shader.hpp"
#include "Core/GL/Texture.hpp"
#include "Core/GL/Framebuffer.hpp"
#include "Core/GL/Material.hpp"

namespace nmGfx
{

    enum class Backend
    {
        OPENGL = 0,
    };
    
    // pure virtual base class that has renderer methods
    class Renderer
    {
    public:
        void Init(int windowWidth, int windowHeight, int videoWidth, int videoHeight, const char* title, WindowFlags flags);
        
        Window& GetWindow() { return _window; }
        
        Renderer() = default;
        ~Renderer();

        /**
         * @brief Begin 3D context, use shaders, calculate matrices
         * 
         * @param projectionMatrix
         * @param cameraTransform Camera transform (NOT view matrix)
         */
        void Begin3D(const glm::mat4 projectionMatrix, const glm::mat4 cameraTransform);
        void End3D();

        /**
         * @brief Returns id of given x, y coordinate in 3d space !Must be called within 3d context(Between Begin3D-End3D block)
         * 
         * @param x 
         * @param y 
         * @return int id
         */
        int Get3DPickID(int x, int y);

        /**
         * @brief Returns id of given x, y coordinate in 3d space. can be called outside of a context
         * 
         * @param x 
         * @param y 
         * @return int id
         */
        int Get3DPickIDSafe(int x, int y);


        /**
         * @brief Draws 3d layer on full screen
         * 
         */
        void Draw3DLayer();


        void DrawModel(const Model& model, const glm::mat4& transform, const Material& material, int drawID = 0);

    private:
        Window _window{};

        // Model _model2d;
        // Shader _shader2d;
        Texture _whiteTexture;

        // Model _lightModel;
        // Shader _lightShader;

        Model _fullscreenModel;
        Shader _fullscreenShader;


        struct {
            Framebuffer _gBuffer;

            glm::mat4 _projectionMatrix;
            glm::mat4 _viewMatrix;

            Shader _shader;
        } _data3d;
    };
    
} // namespace nmGfx


#endif // __NM_GFX_RENDERER_HPP__