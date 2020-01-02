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

    m_window = static_cast<uint32_t>(XCreateWindow(m_display, parent, posX, posY, width, height, 0, vi->depth, InputOutput, vi->visual, valueMask, &swa));
    if (!m_window) {
        throw std::runtime_error("failed to create window");
    }

    m_atomWMDeleteWindow = XInternAtom(m_display, "WM_DELETE_WINDOW", False);
    XSetWMProtocols(m_display, m_window, &m_atomWMDeleteWindow, 1);

    // Set WM_NAME property
    {
        XStoreName(m_display, m_window, name.c_str());
    }

    // Set WM_CLASS property
    {
        XClassHint* hint = XAllocClassHint();
        hint->res_name = const_cast<char*>(name.c_str());
        hint->res_class = const_cast<char*>(name.c_str());
        XSetClassHint(m_display, m_window, hint);
        XFree(hint);
    }

    // Set WM_NORMAL_HINTS property
    {
        XSizeHints* hint = XAllocSizeHints();
        hint->flags = PMinSize;
        hint->min_width  = 320;
        hint->min_height = 240;
        XSetWMNormalHints(m_display, m_window, hint);
        XFree(hint);
    }

    // Create cursors
    {
        CreateCursors();
        SetCursor(CursorType::Arrow);
        GetCursorPos(m_lastCursorPosX, m_lastCursorPosY);
    }

    XMapWindow(m_display, m_window);
    XFlush(m_display);

    glXCreateContextAttribsARBProc glXCreateContextAttribsARB = nullptr;
    {
        // Create an oldstyle context first, to get the correct function pointer for glXCreateContextAttribsARB
        GLXContext ctx_old         = glXCreateContext(m_display, vi, 0, GL_TRUE);
        auto* address = glXGetProcAddress(reinterpret_cast<const GLubyte*>("glXCreateContextAttribsARB"));
        glXCreateContextAttribsARB = reinterpret_cast<glXCreateContextAttribsARBProc>(address);
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
    HandleSizeEvent(width, height);
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

void WindowGLLinux::GetCursorPos(int& x, int& y) {
    Window root, child;
    int rootX, rootY;
    unsigned int mask;
    XQueryPointer(m_display, m_window, &root, &child, &rootX, &rootY, &x, &y, &mask);
}

void WindowGLLinux::SetCursorPos(int x, int y) {
    m_lastCursorPosX = x;
    m_lastCursorPosY = y;
    XWarpPointer(m_display, None, m_window, 0, 0, 0, 0, x, y);
    XFlush(m_display);
}

void WindowGLLinux::SetCursor(CursorType value) {
    if ((m_currentCursorType == value) || (value >= CursorType::Undefined)) {
        return;
    }
    if (m_currentCursorType == CursorType::Disabled) {
        EnableCursor();
        SetCursorPos(m_visibleCursorPosX, m_visibleCursorPosY);
    }

    m_currentCursorType = value;
    if (value <= CursorType::LastStandartCursor) {
        XDefineCursor(m_display, m_window, m_cursors[static_cast<uint>(value)]);
    } else if (value == CursorType::Hidden) {
        XDefineCursor(m_display, m_window, m_hiddenCursor);
    } else if (value == CursorType::Disabled) {
        XDefineCursor(m_display, m_window, m_hiddenCursor);
        GetCursorPos(m_visibleCursorPosX, m_visibleCursorPosY);
        DisableCursor();
        SetCursorPos(m_windowCenterX, m_windowCenterY);
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
                if (static_cast<uint64_t>(event.xclient.data.l[0]) == m_atomWMDeleteWindow) {
                    m_eventHandler->OnWindowDestroy();
                }
            }
            break;
            case DestroyNotify:
                m_eventHandler->OnWindowDestroy();
                break;
            case ConfigureNotify:
                HandleSizeEvent(static_cast<uint32_t>(event.xconfigure.width), static_cast<uint32_t>(event.xconfigure.height));
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
            case MotionNotify: {
                if (m_currentCursorType == CursorType::Disabled) {
                    if ((event.xmotion.x == m_lastCursorPosX) && (event.xmotion.y == m_lastCursorPosY)) {
                        break;
                    }
                    auto dtX = static_cast<double>(event.xmotion.x - m_lastCursorPosX);
                    auto dtY = static_cast<double>(event.xmotion.y - m_lastCursorPosY);
                    m_virtualCursorX += dtX;
                    m_virtualCursorY += dtY;
                    m_eventHandler->OnCursorPosition(m_virtualCursorX, m_virtualCursorY);
                } else {
                    m_eventHandler->OnCursorPosition(static_cast<double>(event.xmotion.x), static_cast<double>(event.xmotion.y));
                }
                m_lastCursorPosX = event.xmotion.x;
                m_lastCursorPosY = event.xmotion.y;
            }
            break;
            default:
                break;
        }
    }

    if ((m_currentCursorType == CursorType::Disabled) && ((m_lastCursorPosX != m_windowCenterX) || (m_lastCursorPosY != m_windowCenterY))) {
        SetCursorPos(m_windowCenterX, m_windowCenterY);
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

        m_cursors[i] = static_cast<uint32_t>(XCreateFontCursor(m_display, nativeType));
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

    m_hiddenCursor = static_cast<uint32_t>(XcursorImageLoadCursor(m_display, image));
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

void WindowGLLinux::DisableCursor() {
    XGrabPointer(m_display, m_window, True,
                 ButtonPressMask | ButtonReleaseMask | PointerMotionMask,
                 GrabModeAsync, GrabModeAsync,
                 m_window,
                 m_hiddenCursor,
                 CurrentTime);
    XFlush(m_display);
}

void WindowGLLinux::EnableCursor() {
    XUngrabPointer(m_display, CurrentTime);
    XFlush(m_display);
}

void WindowGLLinux::HandleSizeEvent(uint32_t width, uint32_t height) {
    m_windowCenterX = width / 2;
    m_windowCenterY = height / 2;
    m_eventHandler->OnWindowSizeEvent(width, height);
}

void WindowGLLinux::HandleKeyEvent(KeyAction action, uint code, uint state) {
    auto key = KeySymToKey(static_cast<uint32_t>(XkbKeycodeToKeysym(m_display, static_cast<KeyCode>(code), 0, 0)));
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
