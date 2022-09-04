#ifndef __NM_GFX_TEXTURE_HPP__
#define __NM_GFX_TEXTURE_HPP__

#include <stdint.h>

namespace nmGfx
{
    enum class TextureType
    {
        TEXTURE2D = 0,
    };

    class Texture
    {
        public:
            Texture() = default;
            ~Texture();

            void LoadFromFile(const char* path, TextureType type = TextureType::TEXTURE2D);
            void Use(int slot = 0);

            
            // Setting values directly with pixel values
            void SetWidth(int width) { _width = width; }
            void SetHeight(int height) { _height = height; }
            void SetChannels(int height) { _height = height; }
            // Loads from raw pixel data
            void LoadFromData(unsigned char* data, int width, int height, int channels, TextureType type = TextureType::TEXTURE2D);
        private:
            int _width = 0;
            int _height = 0;
            int _channels = 0;

            unsigned int _id = 0;

            TextureType _type;

            friend class Renderer;
    };
} // namespace nmGfx


#endif // __NM_GFX_TEXTURE_HPP__