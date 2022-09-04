#include "Texture.hpp"

#include "glad/glad.h"
#include "stb_image.h"

namespace nmGfx
{
    static GLenum GetTextureType(TextureType type)
    {
        return type == TextureType::TEXTURE2D ? GL_TEXTURE_2D
               : GL_NONE;
    }
    static GLenum GetTextureFormat(int nrChannels)
    {
        return nrChannels == 1 ? GL_RED
            : nrChannels == 2 ? GL_RG
            : nrChannels == 3 ? GL_RGB
            : nrChannels == 4 ? GL_RGBA
            : GL_NONE;
    }

    Texture::~Texture()
    {
        
    }

    void Texture::LoadFromData(unsigned char* data, int width, int height, int channels, TextureType type/* = TextureType::TEXTURE2D*/)
    {
        _width = width;
        _height = height;
        _channels = channels;
        _type = type;

        if(data)
        {
            // glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

            glGenTextures(1, &_id);
            glBindTexture(GetTextureType(_type), _id);

            glTexParameteri(GetTextureType(_type), GL_TEXTURE_WRAP_S, GL_REPEAT);	
            glTexParameteri(GetTextureType(_type), GL_TEXTURE_WRAP_T, GL_REPEAT);
            glTexParameteri(GetTextureType(_type), GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
            glTexParameteri(GetTextureType(_type), GL_TEXTURE_MAG_FILTER, GL_LINEAR);

            glTexImage2D(GetTextureType(_type), 0, GL_RGB, _width, _height, 0, GetTextureFormat(_channels), GL_UNSIGNED_BYTE, data);
            glGenerateMipmap(GetTextureType(_type));
        }
        else
        {
            printf("Error: Failed to load texture from pixel data.\n");
        }
    }

    void Texture::LoadFromFile(const char* path, TextureType type/* = TextureType::TEXTURE2D*/)
    {
        _type = type;

        // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        unsigned char* data = stbi_load(path, &_width, &_height, &_channels, 0);
        if(data)
        {
            // glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

            glGenTextures(1, &_id);
            glBindTexture(GetTextureType(_type), _id);

            glTexParameteri(GetTextureType(_type), GL_TEXTURE_WRAP_S, GL_REPEAT);	
            glTexParameteri(GetTextureType(_type), GL_TEXTURE_WRAP_T, GL_REPEAT);
            glTexParameteri(GetTextureType(_type), GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
            glTexParameteri(GetTextureType(_type), GL_TEXTURE_MAG_FILTER, GL_LINEAR);

            glTexImage2D(GetTextureType(_type), 0, GL_RGBA, _width, _height, 0, GetTextureFormat(_channels), GL_UNSIGNED_BYTE, data);
            glGenerateMipmap(GetTextureType(_type));
        }
        else
        {
            printf("Error: Failed to load texture: %s\n", path);
        }
        stbi_image_free(data);
    }

    void Texture::Use(int slot /*= 0*/)
    {
        glActiveTexture(GL_TEXTURE0 + slot);
        glBindTexture(GetTextureType(_type), _id);
    }
} // namespace nmGfx
