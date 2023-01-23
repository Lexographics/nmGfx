#ifndef __NM_GFX_FONT_HPP__
#define __NM_GFX_FONT_HPP__

#include <stdint.h>
#include <string>
#include "glm/glm.hpp"
#include <map>

namespace nmGfx
{
    class Font
    {
        public:

        public:
            Font() = default;
            ~Font();

        private:
            struct Character {
                unsigned int TextureID;
                glm::ivec2   Size;
                glm::ivec2   Bearing;
                unsigned int Advance;
            };
            std::map<int, Font::Character> _Characters;

			friend class Renderer;
    };
} // namespace nmGfx


#endif // __NM_GFX_FONT_HPP__