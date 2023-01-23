#include "nm_Renderer.hpp"

#include <vector>
#include <string.h>
#include "Core/nm_Renderer.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtx/vector_angle.hpp"
#include "glad/glad.h"
#include <GLFW/glfw3.h>
#include "Core/nm_Matrix.hpp"

#include "ft2build.h"
#include FT_FREETYPE_H


namespace nmGfx
{
    bool Renderer::Init(int windowWidth, int windowHeight, int videoWidth, int videoHeight, const char* title, WindowFlags flags)
    {
        _window = nmGfx::Window(windowWidth, windowHeight, videoWidth, videoHeight, title, flags);

        _data2d._frameBuffer.Create2DDefault(&_window, videoWidth, videoHeight);
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

            _fullscreen._model.Create();
            _fullscreen._model.ResetAttributes();
            std::vector<float> data(sizeof(fullscreen_vertices) / sizeof(float));
            std::vector<uint32_t> indices_vec(sizeof(fullscreen_indices) / sizeof(uint32_t));
            memcpy(data.data(), fullscreen_vertices, sizeof(fullscreen_vertices));
            memcpy(indices_vec.data(), fullscreen_indices, sizeof(fullscreen_indices));
            _fullscreen._model.SetModelData(data);
            _fullscreen._model.SetIndexData(indices_vec);
            _fullscreen._model.SetAttribute(0, AttributeType::VEC2);
            _fullscreen._model.SetAttribute(1, AttributeType::VEC2);
            _fullscreen._model.UploadAttributes();
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

        { // skybox cube
            static const float skybox_vertices[] = {
                /*       aPos       */  
                -1.0f,  -1.0f,   1.0f,
                 1.0f,  -1.0f,   1.0f,
                 1.0f,  -1.0f,  -1.0f,
                -1.0f,  -1.0f,  -1.0f,
                -1.0f,   1.0f,   1.0f,
                 1.0f,   1.0f,   1.0f,
                 1.0f,   1.0f,  -1.0f,
                -1.0f,   1.0f,  -1.0f,
            };  
            static const unsigned int skybox_indices[] = {
                1, 2, 6,
                6, 5, 1,

                0, 4, 7,
                7, 3, 0,

                4, 5, 6,
                6, 7, 4,

                0, 3, 2,
                2, 1, 0,

                0, 1, 5,
                5, 4, 0,

                3, 7, 6,
                6, 2, 3,
            };

            _data3d._skyboxModel.Create();
            _data3d._skyboxModel.ResetAttributes();
            std::vector<float> data(sizeof(skybox_vertices) / sizeof(float));
            std::vector<uint32_t> indices_vec(sizeof(skybox_indices) / sizeof(uint32_t));
            memcpy(data.data(), skybox_vertices, sizeof(skybox_vertices));
            memcpy(indices_vec.data(), skybox_indices, sizeof(skybox_indices));
            _data3d._skyboxModel.SetModelData(data);
            _data3d._skyboxModel.SetIndexData(indices_vec);
            _data3d._skyboxModel.SetAttribute(0, AttributeType::VEC3);
            _data3d._skyboxModel.UploadAttributes();
        }

        _Freetype = std::make_unique<FT_Library>();
        if (FT_Init_FreeType(_Freetype.get()))  
        {
            return false;
        }

    
        _GlyphVAO.Create();
        _GlyphVBO.Create(BufferType::VERTEX_BUFFER);

        _GlyphVAO.Use();
        _GlyphVBO.Use();

        _GlyphVBO.BufferData(nullptr, sizeof(float) * 6 * 4, BufferUsage::DYNAMIC_DRAW);

        _GlyphVAO.ResetAttributes();
        _GlyphVAO.SetAttribute(0, AttributeType::VEC4);
        _GlyphVAO.UploadAttributes();

        _GlyphVBO.Unbind();
        VertexArray::Unbind();

        return true;
    }

    Renderer::~Renderer()
    {
        FT_Done_FreeType(*_Freetype.get());
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
        _data3d._projectionMatrix = projectionMatrix;
        _data3d._viewMatrix = glm::inverse(cameraTransform);

        _data3d._gBuffer.Use();
        glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
        glEnable(GL_DEPTH_TEST);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        if(_data3d._skyboxTexture != nullptr)
        {
            // Draw skybox
            glDepthFunc(GL_LEQUAL);
            _data3d._skyboxShader.Use();
            _data3d._skyboxShader.UniformTexture("uSkybox", *_data3d._skyboxTexture, 0);

            glm::mat4 view = glm::mat4(glm::mat3(_data3d._viewMatrix));
            _data3d._skyboxShader.UniformMat4("uViewProj", _data3d._projectionMatrix * view);
            _data3d._skyboxModel.Draw();
            glDepthFunc(GL_LESS);
        }


        _data3d._shader.Use();
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
        // _data3d._shader.UniformFloat("uMat_Specular", material.specular);
        // _data3d._shader.UniformTexture("uMat_SpecularTex", material.specular_tex ? *(material.specular_tex) : _whiteTexture, 1);

        _data3d._shader.UniformInt("uDrawID", drawID);
        model.Draw();
    }

    void Renderer::Draw3DLayer()
    {
        glm::mat4 fullproj = glm::ortho(0.f, (float)_window.GetWindowWidth(), 0.f, (float)_window.GetWindowHeight(), 0.f, 10.f); // no view matrix
        _fullscreen._shader.Use();
        _fullscreen._shader.UniformTexture("gAlbedo", _data3d._gBuffer._gAlbedo, 0);
        // _fullscreen._shader.UniformTexture("gPosition", _data3d._gBuffer._gPosition, 1);
        // _fullscreen._shader.UniformTexture("gNormal", _data3d._gBuffer._gNormal, 2);
        
        _fullscreen._model.Draw();
    }

    void Renderer::BeginPass(Framebuffer& pass) {
        pass.Use();
    }
    void Renderer::EndPass() {
        _window.UnbindFramebuffer();
    }

    void Renderer::SetClearColor(float r, float g, float b, float a) {
        glClearColor(r, g, b, a);
    }
    void Renderer::ClearColor() {
        glClear(GL_COLOR_BUFFER_BIT);
    }
    void Renderer::ClearDepth() {
        glClear(GL_DEPTH_BUFFER_BIT);
    }
    void Renderer::SetDepthTesting(bool enabled) {
        if(enabled)
            glEnable(GL_DEPTH_TEST);
        else 
            glDisable(GL_DEPTH_TEST);
    }
    void Renderer::DrawQuad(Shader& shader) {
        shader.Use();
        _data2d._model2d.Draw();
    }
    void Renderer::DrawText(nmGfx::Shader &s, Font& font, const std::string& text, float scale)
    {
        // activate corresponding render state	
        s.Use();
        // s.UniformVec3("textColor", color);
        _GlyphVAO.Use();

        // iterate through all characters
        std::string::const_iterator c;
        float x = 0.f;
        float y = 0.f;
        for (c = text.begin(); c != text.end(); c++)
        {
            Font::Character ch = font._Characters[*c];


            float xpos = x + ch.Bearing.x * scale;
            float ypos = y - (ch.Size.y - ch.Bearing.y) * scale;

            float w = ch.Size.x * scale;
            float h = ch.Size.y * scale;
            float vertices[6][4] = {
                { xpos,     ypos + h,   0.0f, 0.0f },            
                { xpos,     ypos,       0.0f, 1.0f },
                { xpos + w, ypos,       1.0f, 1.0f },

                { xpos,     ypos + h,   0.0f, 0.0f },
                { xpos + w, ypos,       1.0f, 1.0f },
                { xpos + w, ypos + h,   1.0f, 0.0f }           
            };
            // render glyph texture over quad
            s.UniformTexture("text", ch.TextureID, 0);
            // update content of VBO memory
            _GlyphVBO.Use();
            _GlyphVBO.BufferSubData(vertices, sizeof(vertices), 0);
            // glBindBuffer(GL_ARRAY_BUFFER, 0);
            glDrawArrays(GL_TRIANGLES, 0, 6);
            // now advance cursors for next glyph (note that advance is number of 1/64 pixels)
            x += (ch.Advance >> 6) * scale; // bitshift by 6 to get value in pixels (2^6 = 64)
        }
        VertexArray::Unbind();
    }

    glm::vec2 Renderer::CalcTextSize(Font& font, const std::string& text, float scale) {
        glm::vec2 size{0.f, 0.f};

        std::string::const_iterator c;
        for (c = text.begin(); c != text.end(); c++)
        {
            Font::Character ch = font._Characters[*c];
            size.x += (ch.Advance >> 6) * scale;
            if(ch.Size.y > size.y)
                size.y = ch.Size.y;
        }
        
        return size;
    }

    void Renderer::DrawPassLayer(Framebuffer& pass) {
        glm::mat4 fullproj = glm::ortho(0.f, (float)_window.GetWindowWidth(), 0.f, (float)_window.GetWindowHeight(), 0.f, 10.f); // no view matrix
		_fullscreen._shader.Use();
		_fullscreen._shader.UniformTexture("gAlbedo", pass.GetAlbedoID(), 0);

		_fullscreen._model.Draw();
    }


	void Renderer::Begin2D(const glm::mat4 cameraTransform, const glm::vec2 &cameraCenter /*= {0.5f, 0.5f}*/, const glm::vec4& clearColor /*= {0.f, 0.f, 0.f, 0.f}*/) {
		_data2d._frameBuffer.Use();
        SetClearColor(clearColor.x, clearColor.y, clearColor.z, clearColor.w);
		SetDepthTesting(true);
        ClearColor();
        ClearDepth();

		_data2d._shader.Use();

		float width = (float)_window.GetVideoWidth();
		float height = (float)_window.GetVideoHeight();
		_data2d._projectionMatrix = CalculateProjectionMatrix(width, height, cameraCenter.x, cameraCenter.y, 0.f, 10.f);
		_data2d._viewMatrix = glm::inverse(cameraTransform);

		glm::mat4 view_proj = _data2d._projectionMatrix * _data2d._viewMatrix;
		_data2d._shader.UniformMat4("uViewProjection", view_proj);
	}

	void Renderer::End2D() {
		_window.UnbindFramebuffer();
	}

	int Renderer::Get2DPickID(int x, int y) {
		int id = 0;
		glReadBuffer(GL_COLOR_ATTACHMENT1);
		glReadPixels(x, y, 1, 1, GL_RED_INTEGER, GL_INT, &id);
		return id;
	}

	int Renderer::Get2DPickIDSafe(int x, int y) {
		_data2d._frameBuffer.Use();
		int id = Get2DPickID(x, y);
		_window.UnbindFramebuffer();

		return id;
	}

	void Renderer::DrawTexture(Texture *texture, const glm::mat4 &transform, const glm::vec3 &tint /*= glm::vec3(1.f)*/, int drawID /*= 0*/) {
		_data2d._shader.UniformMat4("uModel", transform);
		_data2d._shader.UniformVec3("uTint", tint);
		_data2d._shader.UniformTexture("uTexture", texture ? *(texture) : _whiteTexture, 1);

		_data2d._shader.UniformInt("uDrawID", drawID);
		_data2d._model2d.Draw();
	}

	void Renderer::Draw2DLayer() {
		glm::mat4 fullproj = glm::ortho(0.f, (float)_window.GetWindowWidth(), 0.f, (float)_window.GetWindowHeight(), 0.f, 10.f); // no view matrix
		_fullscreen._shader.Use();
		_fullscreen._shader.UniformTexture("gAlbedo", _data2d._frameBuffer._gAlbedo, 0);

		_fullscreen._model.Draw();
	}


    bool Renderer::LoadFontWithFace(Font* font, FT_Face& face) {
        if (FT_Load_Char(face, 'X', FT_LOAD_RENDER))
        {
            return false;
        }

        glPixelStorei(GL_UNPACK_ALIGNMENT, 1); // disable byte-alignment restriction
  
        for (int c = 0; c < 128; c++)
        {
            if (FT_Load_Char(face, c, FT_LOAD_RENDER))
                continue;

            unsigned int texture;
            glGenTextures(1, &texture);
            glBindTexture(GL_TEXTURE_2D, texture);
            glTexImage2D(
                GL_TEXTURE_2D,
                0,
                GL_RED,
                face->glyph->bitmap.width,
                face->glyph->bitmap.rows,
                0,
                GL_RED,
                GL_UNSIGNED_BYTE,
                face->glyph->bitmap.buffer
            );
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            Font::Character character = {
                texture, 
                glm::ivec2(face->glyph->bitmap.width, face->glyph->bitmap.rows),
                glm::ivec2(face->glyph->bitmap_left, face->glyph->bitmap_top),
                static_cast<unsigned int>(face->glyph->advance.x)
            };
            font->_Characters[c] = character;
        }

        FT_Done_Face(face);
        return true;
    }
    bool Renderer::LoadFont(Font* font, unsigned char* data, unsigned size) {
        FT_Face face;

        if (FT_New_Memory_Face(*_Freetype.get(), data, size, 0, &face))
        {  
            return false;
        }
        FT_Set_Pixel_Sizes(face, 0, 48);  

        return LoadFontWithFace(font, face);
    }

    bool Renderer::LoadFont(Font* font, const std::string& path) {
        FT_Face face;

        if (FT_New_Face(*_Freetype.get(), path.c_str(), 0, &face))
        {  
            return false;
        }
        FT_Set_Pixel_Sizes(face, 0, 48);  

        return LoadFontWithFace(font, face);
    }
} // namespace nmGfx
