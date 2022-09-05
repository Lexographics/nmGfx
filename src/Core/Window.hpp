#ifndef __NM_GFX_WINDOW_HPP__
#define __NM_GFX_WINDOW_HPP__
#pragma once

#include <stdint.h>

class GLFWwindow;
namespace nmGfx
{
    enum WindowFlags : uint32_t
    {
        NONE = 0,
        FULLSCREEN = 1 << 1,
    };

    // pure virtual base class that has window methods
    class Window
    {
    public:
        // virtual void CreateWindow(int width, int height, const char* title, WindowFlags flags) = 0;
        bool ShouldClose();
        
        void PollEvents();
        void SwapBuffers();

        // void SetClearColor(float r, float g, float b, float a = 1.0f);
        // void Clear(uint32_t bits = ClearBit::COLOR);

        ~Window() = default;
        Window(int windowWidth, int windowHeight, int videoWidth, int videoHeight, const char* title, WindowFlags flags);
        Window() = default;

        void SetFullscreen(bool fullscreen);
        inline bool GetFullscreen() { return _fullscreen; }

        // Returns x mouse pos relative to window
        int GetWindowMousePosX();
        // Returns y mouse pos relative to window
        int GetWindowMousePosY();

        // Returns x mouse pos relative to video space
        int GetVideoMousePosX();
        // Returns y mouse pos relative to window
        int GetVideoMousePosY();


        int GetWindowHeight();
        int GetWindowWidth();

        inline int GetVideoWidth() { return _videoWidth; }
        inline int GetVideoHeight() { return _videoHeight; }

        void UnbindFramebuffer();

    private:
        friend class Renderer;
        
        GLFWwindow* _pWindow = nullptr;

        int _videoWidth;
        int _videoHeight;

        // Stores window values after setting fullscreen to revert on returning back to windowed mode
        bool _fullscreen = false;
        int _fullscreen_oldX = 0;
        int _fullscreen_oldY = 0;
        int _fullscreen_oldW = 0;
        int _fullscreen_oldH = 0;
        int _fullscreen_oldRefresh = 0;
    };
    
} // namespace nmGfx


#endif // __NM_GFX_WINDOW_HPP__