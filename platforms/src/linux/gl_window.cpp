#include "platforms/linux/gl_window.h"

#include <GL/glx.h>
#include <exception>
#include <X11/XKBlib.h>
#include <X11/cursorfont.h>
#include <X11/Xcursor/Xcursor.h>

#include "platforms/linux/x11_key_map.h"


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

WindowGLLinux::WindowGLLinux(const std::shared_ptr<WindowEventsHandler>& handler)
    : RenderWindow(handler) {

}

WindowGLLinux::~WindowGLLinux() {
    Destroy();
}

void WindowGLLinux::Create(int16_t posX, int16_t posY, uint16_t width, uint16_t height, const std::string& name) {
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
        throw std::runtime_error("failed to retrieve a framebuffer config");
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

    m_window = XCreateWindow(m_display, parent, posX, posY, width, height, 0, vi->depth, InputOutput, vi->visual, valueMask, &swa);
    if (!m_window) {
        throw std::runtime_error("failed to create window");
    }

    XStoreName(m_display, m_window, name.c_str());

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
    CreateCursors();
    SetCursor(CursorType::Arrow);

    glXCreateContextAttribsARBProc glXCreateContextAttribsARB = nullptr;
    {
        // Create an oldstyle context first, to get the correct function pointer for glXCreateContextAttribsARB
        GLXContext ctx_old         = glXCreateContext(m_display, vi, 0, GL_TRUE);
        glXCreateContextAttribsARB = (glXCreateContextAttribsARBProc)glXGetProcAddress((const GLubyte*)"glXCreateContextAttribsARB");
        glXMakeCurrent(m_display, None, NULL);
        glXDestroyContext(m_display, ctx_old);
    }

    if (glXCreateContextAttribsARB == nullptr) {
        throw std::runtime_error("glXCreateContextAttribsARB entry point not found");
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
        throw std::runtime_error("failed to create GL context");
    }
    XFree(fbc);

    glXMakeCurrent(m_display, m_window, ctx);
    m_eventHandler->OnWindowSizeEvent(width, height);
}

void WindowGLLinux::Destroy() {
    if (m_display != nullptr) {
        DestroyCursors();
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

void WindowGLLinux::SetTitle(const std::string& title) {
    XStoreName(m_display, m_window, title.c_str());
}

void WindowGLLinux::SetCursor(CursorType value) {
    if (m_currentCursorType == value) {
        return;
    }
    m_currentCursorType = value;
    if (value <= CursorType::LastStandartCursor) {
        XDefineCursor(m_display, m_window, m_cursors[static_cast<uint>(value)]);
    } else {
        XDefineCursor(m_display, m_window, m_hiddenCursor);
    }
}

void WindowGLLinux::ProcessEvents() {
    m_eventHandler->OnNewFrame();
    XPending(m_display);
    while (XQLength(m_display)) {
        XEvent event;
        XNextEvent(m_display, &event);
        switch (event.type) {
            case ClientMessage: {
                if (event.xclient.data.l[0] == m_atomWMDeleteWindow) {
                    m_eventHandler->OnWindowDestroy();
                }
            }
            break;
            case DestroyNotify:
                m_eventHandler->OnWindowDestroy();
                break;
            case ConfigureNotify:
                m_eventHandler->OnWindowSizeEvent(event.xconfigure.width, event.xconfigure.height);
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
            case MotionNotify:
                m_eventHandler->OnCursorPosition(static_cast<double>(event.xmotion.x), static_cast<double>(event.xmotion.y));
                break;
            default:
                break;
        }
    }
}

void WindowGLLinux::CreateCursors() {
    for (uint i=0; i!=static_cast<uint>(CursorType::LastStandartCursor) + 1; ++i) {
        auto type = static_cast<CursorType>(i);
        uint nativeType = 0;
        switch (type) {
            case CursorType::Arrow:
                nativeType = XC_left_ptr;
                break;
            case CursorType::IBeam:
                nativeType = XC_xterm;
                break;
            case CursorType::Crosshair:
                nativeType = XC_crosshair;
                break;
            case CursorType::Hand:
                nativeType = XC_hand2;
                break;
            case CursorType::ResizeH:
                nativeType = XC_sb_h_double_arrow;
                break;
            case CursorType::ResizeV:
                nativeType = XC_sb_v_double_arrow;
                break;
            default:
                throw std::runtime_error("unknown cursor type id");
        }

        m_cursors[i] = XCreateFontCursor(m_display, nativeType);
        if (!m_cursors[i]) {
            throw std::runtime_error("failed to create standard cursor");
        }
    }

    // create hidden cursor
    const uint width = 16;
    const uint height = 16;

    XcursorImage* image = XcursorImageCreate(width, height);
    if (image == NULL) {
        throw std::runtime_error("failed to create hidden cursor");
    }

    image->xhot = 0;
    image->yhot = 0;

    auto* target = image->pixels;
    for (uint i=0; i!=width*height;  ++i, ++target) {
        *target = 0;
    }

    m_hiddenCursor = XcursorImageLoadCursor(m_display, image);
    XcursorImageDestroy(image);
}

void WindowGLLinux::DestroyCursors() {
    for (uint i=0; i!=static_cast<uint>(CursorType::LastStandartCursor) + 1; ++i) {
        if (m_cursors[i] != 0) {
            XFreeCursor(m_display, m_cursors[i]);
            m_cursors[i] = 0;
        }
    }
    if (m_hiddenCursor != 0) {
        XFreeCursor(m_display, m_hiddenCursor);
        m_hiddenCursor = 0;
    }
}

void WindowGLLinux::HandleKeyEvent(KeyAction action, uint code, uint state) {
    auto key = KeySymToKey(XkbKeycodeToKeysym(m_display, code, 0, 0));
    m_eventHandler->OnKeyEvent(action, key, StateToModifiers(state));
}

void WindowGLLinux::HandleMouseButtonEvent(KeyAction action, uint code, uint state) {
    switch (code) {
        case Button1:
            m_eventHandler->OnKeyEvent(action, Key::MouseLeft, StateToModifiers(state));
            break;
        case Button2:
            m_eventHandler->OnKeyEvent(action, Key::MouseMiddle, StateToModifiers(state));
            break;
        case Button3:
            m_eventHandler->OnKeyEvent(action, Key::MouseRight, StateToModifiers(state));
            break;
        case Button4:
            m_eventHandler->OnScroll(1);
            break;
        case Button5:
            m_eventHandler->OnScroll(-1);
            break;
    }
}
