#include "linux/gl_window.h"

#include <GL/glx.h>

#include "core/common/exception.h"

#ifndef GLX_CONTEXT_MAJOR_VERSION_ARB
#    define GLX_CONTEXT_MAJOR_VERSION_ARB 0x2091
#endif

#ifndef GLX_CONTEXT_MINOR_VERSION_ARB
#    define GLX_CONTEXT_MINOR_VERSION_ARB 0x2092
#endif

#ifndef GLX_CONTEXT_FLAGS_ARB
#    define GLX_CONTEXT_FLAGS_ARB 0x2094
#endif

#ifndef GLX_CONTEXT_DEBUG_BIT_ARB
#    define GLX_CONTEXT_DEBUG_BIT_ARB 0x0001
#endif

typedef GLXContext (*glXCreateContextAttribsARBProc)(Display*, GLXFBConfig, GLXContext, int, const int*);


void WindowGLLinux::Create(int16_t posX, int16_t posY, uint16_t width, uint16_t height, const std::string& title) {
    m_width = width;
    m_height = height;

    m_display = XOpenDisplay(0);

    static int visualAttribs[] = {
        GLX_RENDER_TYPE,    GLX_RGBA_BIT,
        GLX_DRAWABLE_TYPE,  GLX_WINDOW_BIT,
        GLX_DOUBLEBUFFER,   true,

        // The largest available total RGBA color buffer size (sum of GLX_RED_SIZE,
        // GLX_GREEN_SIZE, GLX_BLUE_SIZE, and GLX_ALPHA_SIZE) of at least the minimum
        // size specified for each color component is preferred.
        GLX_RED_SIZE,       8,
        GLX_GREEN_SIZE,     8,
        GLX_BLUE_SIZE,      8,
        GLX_ALPHA_SIZE,     8,

        // The largest available depth buffer of at least GLX_DEPTH_SIZE size is preferred
        GLX_DEPTH_SIZE,     24,

        //GLX_SAMPLE_BUFFERS, 1,
        GLX_SAMPLES, 1,
        None
     };

    int fbcount = 0;
    GLXFBConfig* fbc = glXChooseFBConfig(m_display, DefaultScreen(m_display), visualAttribs, &fbcount);
    if (!fbc) {
        throw EngineError("failed to retrieve a framebuffer config");
    }

    XVisualInfo* vi = glXGetVisualFromFBConfig(m_display, fbc[0]);

    XSetWindowAttributes swa;
    swa.colormap     = XCreateColormap(m_display, RootWindow(m_display, vi->screen), vi->visual, AllocNone);
    swa.border_pixel = 0;
    swa.event_mask =
        StructureNotifyMask |
        ExposureMask |
        KeyPressMask |
        KeyReleaseMask |
        ButtonPressMask |
        ButtonReleaseMask |
        PointerMotionMask;
    uint valueMask = CWBorderPixel | CWColormap | CWEventMask;
    auto parent = RootWindow(m_display, vi->screen);

    m_window = XCreateWindow(m_display, parent, posX, posY, m_width, m_height, 0, vi->depth, InputOutput, vi->visual, valueMask, &swa);
    if (!m_window) {
        throw EngineError("failed to create window");
    }

    {
        auto sizeHints        = XAllocSizeHints();
        sizeHints->flags      = PMinSize;
        sizeHints->min_width  = 320;
        sizeHints->min_height = 240;
        XSetWMNormalHints(m_display, m_window, sizeHints);
        XFree(sizeHints);
    }

    XMapWindow(m_display, m_window);

    glXCreateContextAttribsARBProc glXCreateContextAttribsARB = nullptr;
    {
        // Create an oldstyle context first, to get the correct function pointer for glXCreateContextAttribsARB
        GLXContext ctx_old         = glXCreateContext(m_display, vi, 0, GL_TRUE);
        glXCreateContextAttribsARB = (glXCreateContextAttribsARBProc)glXGetProcAddress((const GLubyte*)"glXCreateContextAttribsARB");
        glXMakeCurrent(m_display, None, NULL);
        glXDestroyContext(m_display, ctx_old);
    }

    if (glXCreateContextAttribsARB == nullptr) {
        throw EngineError("glXCreateContextAttribsARB entry point not found");
    }

    int flags = GLX_CONTEXT_FORWARD_COMPATIBLE_BIT_ARB;
#ifdef _DEBUG
    flags |= GLX_CONTEXT_DEBUG_BIT_ARB;
#endif

    int major_version = 4;
    int minor_version = 3;

    static int contextAttribs[] = {
        GLX_CONTEXT_MAJOR_VERSION_ARB, major_version,
        GLX_CONTEXT_MINOR_VERSION_ARB, minor_version,
        GLX_CONTEXT_FLAGS_ARB, flags,
        None
    };

    constexpr int valTrue = 1;
    GLXContext ctx  = glXCreateContextAttribsARB(m_display, fbc[0], NULL, valTrue, contextAttribs);
    if (!ctx) {
        throw EngineError("failed to create GL context");
    }
    XFree(fbc);

    glXMakeCurrent(m_display, m_window, ctx);
}

void WindowGLLinux::Destroy() {
    auto ctx = glXGetCurrentContext();
    glXMakeCurrent(m_display, None, NULL);
    glXDestroyContext(m_display, ctx);
    XDestroyWindow(m_display, m_window);
    XCloseDisplay(m_display);
}
