#include "linux/gl_window.h"

#include <GL/glx.h>
#include <X11/XKBlib.h>

#include "linux/x11_key_map.h"
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


WindowGLLinux::~WindowGLLinux() {
    Destroy();
}

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

    m_atomWMDeleteWindow = XInternAtom(m_display, "WM_DELETE_WINDOW", False);
    XSetWMProtocols(m_display, m_window, &m_atomWMDeleteWindow, 1);

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
    if (m_display != nullptr) {
        auto ctx = glXGetCurrentContext();
        glXMakeCurrent(m_display, None, NULL);
        glXDestroyContext(m_display, ctx);
        XDestroyWindow(m_display, m_window);
        XCloseDisplay(m_display);
        m_display = nullptr;
        m_window = 0;
        m_atomWMDeleteWindow = 0;
    }
}


void WindowGLLinux::ProcessEvents() {
    XPending(m_display);
    while (XQLength(m_display)) {
        XEvent event;
        XNextEvent(m_display, &event);
        switch (event.type) {
            case ClientMessage: {
                if (event.xclient.data.l[0] == m_atomWMDeleteWindow) {
                    m_windowShouldClose = true;
                }
            }
            break;
            case DestroyNotify:
                m_windowShouldClose = true;
                break;
            case ConfigureNotify: {
                auto width = event.xconfigure.width;
                auto height = event.xconfigure.height;
                if (((width != m_width) || (height != m_height)) && (width != 0) && (height != 0)) {
                    m_width  = width;
                    m_height = height;
                }
            }
            break;
            case KeyPress:
                HandleKeyEvent(KeyAction::Press, event.xkey.keycode, event.xkey.state);
                break;
            case KeyRelease:
                HandleKeyEvent(KeyAction::Release, event.xkey.keycode, event.xkey.state);
                break;
            case ButtonPress:
                HandleMouseButtonEvent(KeyAction::Press, event.xbutton.button, event.xbutton.state);
                break;
            case ButtonRelease:
                HandleMouseButtonEvent(KeyAction::Release, event.xbutton.button, event.xbutton.state);
                break;
            default:
                break;
        }
    }
}

void WindowGLLinux::HandleKeyEvent(KeyAction action, uint code, uint state) {
    if (m_eventHandler) {
        auto key = KeySymToKey(XkbKeycodeToKeysym(m_display, code, 0, 0));
        m_eventHandler->OnKeyEvent(action, key, StateToModifiers(state));
    }
}

void WindowGLLinux::HandleMouseButtonEvent(KeyAction action, uint code, uint state) {
    if (m_eventHandler) {
        m_eventHandler->OnKeyEvent(action, MouseBottonToKey(code), StateToModifiers(state));
    }
}
