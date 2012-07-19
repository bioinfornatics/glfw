//========================================================================
// GLFW - An OpenGL library
// Platform:    X11/EGL
// API version: 3.0
// WWW:         http://www.glfw.org/
//------------------------------------------------------------------------
// Copyright (c) 2002-2006 Marcus Geelnard
// Copyright (c) 2006-2010 Camilla Berglund <elmindreda@elmindreda.org>
//
// This software is provided 'as-is', without any express or implied
// warranty. In no event will the authors be held liable for any damages
// arising from the use of this software.
//
// Permission is granted to anyone to use this software for any purpose,
// including commercial applications, and to alter it and redistribute it
// freely, subject to the following restrictions:
//
// 1. The origin of this software must not be misrepresented; you must not
//    claim that you wrote the original software. If you use this software
//    in a product, an acknowledgment in the product documentation would
//    be appreciated but is not required.
//
// 2. Altered source versions must be plainly marked as such, and must not
//    be misrepresented as being the original software.
//
// 3. This notice may not be removed or altered from any source
//    distribution.
//
//========================================================================

#ifndef _platform_h_
#define _platform_h_

#include <unistd.h>
#include <signal.h>
#include <stdint.h>
#include <X11/Xlib.h>
#include <X11/keysym.h>
#include <X11/Xatom.h>
#include <X11/Xutil.h>

// Include EGL
#include <EGL/egl.h>

// With XFree86, we can use the XF86VidMode extension
#if defined(_GLFW_HAS_XF86VIDMODE)
 #include <X11/extensions/xf86vmode.h>
#endif

#if defined(_GLFW_HAS_XRANDR)
 #include <X11/extensions/Xrandr.h>
#endif

// Do we have support for dlopen/dlsym?
#if defined(_GLFW_HAS_DLOPEN)
 #include <dlfcn.h>
#endif

// The Xkb extension provides improved keyboard support
#if defined(_GLFW_HAS_XKB)
 #include <X11/XKBlib.h>
#endif

// We support two different ways for getting addresses for EGL
// extension functions: eglGetProcAddress and dlsym
#if defined(_GLFW_HAS_EGLGETPROCADDRESS)
 #define _glfw_eglGetProcAddress(x) eglGetProcAddress(x)
#elif defined(_GLFW_HAS_DLOPEN)
 #define _glfw_eglGetProcAddress(x) dlsym(_glfwLibrary.EGL.libEGL, x)
 #define _GLFW_DLOPEN_LIBEGL
#else
 #error "No OpenGL entry point retrieval mechanism was enabled"
#endif

#define _GLFW_PLATFORM_WINDOW_STATE  _GLFWwindowX11 X11
#define _GLFW_PLATFORM_CONTEXT_STATE _GLFWcontextEGL EGL
#define _GLFW_PLATFORM_LIBRARY_WINDOW_STATE _GLFWlibraryX11 X11
#define _GLFW_PLATFORM_LIBRARY_OPENGL_STATE _GLFWlibraryEGL EGL

// Clipboard format atom indices
#define _GLFW_CLIPBOARD_FORMAT_UTF8     0
#define _GLFW_CLIPBOARD_FORMAT_COMPOUND 1
#define _GLFW_CLIPBOARD_FORMAT_STRING   2
#define _GLFW_CLIPBOARD_FORMAT_COUNT    3

// Clipboard conversion status tokens
#define _GLFW_CONVERSION_INACTIVE       0
#define _GLFW_CONVERSION_SUCCEEDED      1
#define _GLFW_CONVERSION_FAILED         2


//========================================================================
// GLFW platform specific types
//========================================================================

//------------------------------------------------------------------------
// Pointer length integer
//------------------------------------------------------------------------
typedef intptr_t GLFWintptr;


//------------------------------------------------------------------------
// Platform-specific OpenGL context structure
//------------------------------------------------------------------------
typedef struct _GLFWcontextEGL
{
   EGLConfig      config;
   EGLContext     context;
   EGLSurface     surface;
   XVisualInfo*   visual;
} _GLFWcontextEGL;


//------------------------------------------------------------------------
// Platform-specific window structure
//------------------------------------------------------------------------
typedef struct _GLFWwindowX11
{
    // Platform specific window resources
    Colormap      colormap;          // Window colormap
    Window        handle;            // Window handle

    // Various platform specific internal variables
    GLboolean     overrideRedirect; // True if window is OverrideRedirect
    GLboolean     keyboardGrabbed;  // True if keyboard is currently grabbed
    GLboolean     cursorGrabbed;    // True if cursor is currently grabbed
    GLboolean     cursorHidden;     // True if cursor is currently hidden
    GLboolean     cursorCentered;   // True if cursor was moved since last poll
    int           cursorPosX, cursorPosY;

} _GLFWwindowX11;

//------------------------------------------------------------------------
// Platform-specific library global data for X11
//------------------------------------------------------------------------
typedef struct _GLFWlibraryX11
{
    Display*        display;
    int             screen;
    Window          root;
    Cursor          cursor;   // Invisible cursor for hidden cursor

    Atom            wmDeleteWindow;    // WM_DELETE_WINDOW atom
    Atom            wmName;            // _NET_WM_NAME atom
    Atom            wmIconName;        // _NET_WM_ICON_NAME atom
    Atom            wmPing;            // _NET_WM_PING atom
    Atom            wmState;           // _NET_WM_STATE atom
    Atom            wmStateFullscreen; // _NET_WM_STATE_FULLSCREEN atom
    Atom            wmActiveWindow;    // _NET_ACTIVE_WINDOW atom

    // True if window manager supports EWMH
    GLboolean       hasEWMH;

    struct {
        GLboolean   available;
        int         eventBase;
        int         errorBase;
    } VidMode;

    struct {
        GLboolean   available;
        int         eventBase;
        int         errorBase;
        int         majorVersion;
        int         minorVersion;
        GLboolean   gammaBroken;
    } RandR;

    struct {
        GLboolean   available;
        int         majorOpcode;
        int         eventBase;
        int         errorBase;
        int         majorVersion;
        int         minorVersion;
    } Xkb;

    // Key code LUT (mapping X11 key codes to GLFW key codes)
    int             keyCodeLUT[256];

    // Screensaver data
    struct {
        GLboolean   changed;
        int         timeout;
        int         interval;
        int         blanking;
        int         exposure;
    } saver;

    // Fullscreen data
    struct {
        GLboolean   modeChanged;
#if defined(_GLFW_HAS_XRANDR)
        SizeID      oldSizeID;
        int         oldWidth;
        int         oldHeight;
        Rotation    oldRotation;
#endif /*_GLFW_HAS_XRANDR*/
#if defined(_GLFW_HAS_XF86VIDMODE)
        XF86VidModeModeInfo oldMode;
#endif /*_GLFW_HAS_XF86VIDMODE*/
    } FS;

    // Timer data
    struct {
        GLboolean   monotonic;
        double      resolution;
        uint64_t    base;
    } timer;

    // Selection data
    struct {
        Atom atom;
        Atom formats[_GLFW_CLIPBOARD_FORMAT_COUNT];
        char* string;
        Atom target;
        Atom targets;
        Atom property;
        int status;
    } selection;

} _GLFWlibraryX11;

//------------------------------------------------------------------------
// Platform-specific library global data for EGL
//------------------------------------------------------------------------
typedef struct _GLFWlibraryEGL
{
    EGLDisplay      display;
    EGLint          majorVersion, minorVersion;

#if defined(_GLFW_DLOPEN_LIBEGL)
    void*           libEGL;  // dlopen handle for libEGL.so
#endif
} _GLFWlibraryEGL;

//------------------------------------------------------------------------
// Joystick information & state
//------------------------------------------------------------------------
GLFWGLOBAL struct {
    int           Present;
    int           fd;
    int           NumAxes;
    int           NumButtons;
    float*        Axis;
    unsigned char* Button;
} _glfwJoy[GLFW_JOYSTICK_LAST + 1];


//========================================================================
// Prototypes for platform specific internal functions
//========================================================================

// Time
void _glfwInitTimer(void);

// OpenGL(|ES) support
int _glfwInitOpenGL(void);
void _glfwTerminateOpenGL(void);
int _glfwCreateContext(_GLFWwindow* window,
                       const _GLFWwndconfig* wndconfig,
                       const _GLFWfbconfig* fbconfig);
void _glfwDestroyContext(_GLFWwindow* window);
XVisualInfo* _glfwGetContextVisual(_GLFWwindow* window);

// Fullscreen support
int  _glfwGetClosestVideoMode(int* width, int* height, int* rate);
void _glfwSetVideoModeMODE(int mode, int rate);
void _glfwSetVideoMode(int* width, int* height, int* rate);
void _glfwRestoreVideoMode(void);

// Joystick input
void _glfwInitJoysticks(void);
void _glfwTerminateJoysticks(void);

// Unicode support
long _glfwKeySym2Unicode(KeySym keysym);

// Clipboard handling
GLboolean _glfwReadSelection(XSelectionEvent* request);
Atom _glfwWriteSelection(XSelectionRequestEvent* request);

// Event processing
void _glfwProcessPendingEvents(void);

#endif // _platform_h_