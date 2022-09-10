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

        
        inline GLFWwindow* GetGLFWwindow() { return _pWindow; }


        /* Input */
        enum class MouseButton
        {
            LEFT = 0,
            MIDDLE,
            RIGHT,
        };
        enum class MousePressState
        {
            CLICKED = 0,
            RELEASED,
        };
        enum class CursorMode
        {
            NORMAL = 0,
            HIDDEN,
            DISABLED,
        };
        enum ModifierKeys : unsigned int
        {
            NONE  = 0,
            MOD_CONTROL   = 1 << 1,
            MOD_SHIFT     = 2 << 1,
            MOD_ALT       = 3 << 1,
            MOD_SUPER     = 4 << 1,
            MOD_CAPS_LOCK = 5 << 1,
            MOD_NUM_LOCK  = 6 << 1,
        };
        // maps one-to-one to glfw keys
        enum Key : int
        {
            // glfw3.h
            UNKNOWN = -1,

            SPACE             =  32,
            APOSTROPHE        =  39,  /* ' */
            COMMA             =  44,  /* , */
            MINUS             =  45,  /* - */
            PERIOD            =  46,  /* . */
            SLASH             =  47,  /* / */
            _0                =  48,
            _1                =  49,
            _2                =  50,
            _3                =  51,
            _4                =  52,
            _5                =  53,
            _6                =  54,
            _7                =  55,
            _8                =  56,
            _9                =  57,
            SEMICOLON         =  59,  /* ; */
            EQUAL             =  61,  /* = */
            A                 =  65,
            B                 =  66,
            C                 =  67,
            D                 =  68,
            E                 =  69,
            F                 =  70,
            G                 =  71,
            H                 =  72,
            I                 =  73,
            J                 =  74,
            K                 =  75,
            L                 =  76,
            M                 =  77,
            N                 =  78,
            O                 =  79,
            P                 =  80,
            Q                 =  81,
            R                 =  82,
            S                 =  83,
            T                 =  84,
            U                 =  85,
            V                 =  86,
            W                 =  87,
            X                 =  88,
            Y                 =  89,
            Z                 =  90,
            LEFT_BRACKET      =  91,  /* [ */
            BACKSLASH         =  92,  /* \ */
            RIGHT_BRACKET     =  93,  /* ] */
            GRAVE_ACCENT      =  96,  /* ` */
            WORLD_1           =  161, /* non-US #1 */
            WORLD_2           =  162, /* non-US #2 */

            /* Function keys */
            ESCAPE             = 256,
            ENTER              = 257,
            TAB                = 258,
            BACKSPACE          = 259,
            INSERT             = 260,
            DELETE             = 261,
            RIGHT              = 262,
            LEFT               = 263,
            DOWN               = 264,
            UP                 = 265,
            PAGE_UP            = 266,
            PAGE_DOWN          = 267,
            HOME               = 268,
            END                = 269,
            CAPS_LOCK          = 280,
            SCROLL_LOCK        = 281,
            NUM_LOCK           = 282,
            PRINT_SCREEN       = 283,
            PAUSE              = 284,
            F1                 = 290,
            F2                 = 291,
            F3                 = 292,
            F4                 = 293,
            F5                 = 294,
            F6                 = 295,
            F7                 = 296,
            F8                 = 297,
            F9                 = 298,
            F10                = 299,
            F11                = 300,
            F12                = 301,
            F13                = 302,
            F14                = 303,
            F15                = 304,
            F16                = 305,
            F17                = 306,
            F18                = 307,
            F19                = 308,
            F20                = 309,
            F21                = 310,
            F22                = 311,
            F23                = 312,
            F24                = 313,
            F25                = 314,
            KP_0               = 320,
            KP_1               = 321,
            KP_2               = 322,
            KP_3               = 323,
            KP_4               = 324,
            KP_5               = 325,
            KP_6               = 326,
            KP_7               = 327,
            KP_8               = 328,
            KP_9               = 329,
            KP_DECIMAL         = 330,
            KP_DIVIDE          = 331,
            KP_MULTIPLY        = 332,
            KP_SUBTRACT        = 333,
            KP_ADD             = 334,
            KP_ENTER           = 335,
            KP_EQUAL           = 336,
            LEFT_SHIFT         = 340,
            LEFT_CONTROL       = 341,
            LEFT_ALT           = 342,
            LEFT_SUPER         = 343,
            RIGHT_SHIFT        = 344,
            RIGHT_CONTROL      = 345,
            RIGHT_ALT          = 346,
            RIGHT_SUPER        = 347,
            MENU               = 348,
        };

        bool IsMouseButtonPressed(MouseButton button);
        void SetCursorMode(CursorMode mode);
        bool IsWindowHovered();
        bool IsKeyPressed(Key key);

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