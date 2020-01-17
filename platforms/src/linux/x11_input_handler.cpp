#include "linux/x11_input_handler.h"

#include <locale>
#include <cstring>
#include <codecvt>
#include <X11/Xutil.h>
#include "platforms/window_events.h"


#if defined(X_HAVE_UTF8_STRING)
#define USE_UTF8 1
#endif

static bool CheckIM(XIM im) {
    XIMStyles* styles = NULL;
    if (XGetIMValues(im, XNQueryInputStyle, &styles, NULL) != NULL) {
        return false;
    }

    bool found = false;
    for(size_t i=0; i!=styles->count_styles; ++i) {
        if (styles->supported_styles[i] == (XIMPreeditNothing | XIMStatusNothing)) {
            found = true;
            break;
        }
    }

    XFree(styles);
    return found;
}

X11InputHandler::X11InputHandler(const std::shared_ptr<WindowEventsHandler>& handler)
    : m_handler(handler) {

}

X11InputHandler::~X11InputHandler() {
    if (m_ic != nullptr) {
        XDestroyIC(m_ic);
        m_ic = NULL;
    }
    if (m_im != nullptr) {
        XCloseIM(m_im);
        m_im = nullptr;
    }
}

void X11InputHandler::Create(Display* display, Window window) {
#if !defined(USE_UTF8)
    // HACK (from glfw):
    // If the current locale is "C" and the Xlib UTF-8 functions are
    //       unavailable, apply the environment's locale in the hope that it's
    //       both available and not "C"
    //       This is done because the "C" locale breaks wide character input,
    //       which is what we fall back on when UTF-8 support is missing
    if (strcmp(std::setlocale(LC_CTYPE, nullptr), "C") == 0) {
        std::setlocale(LC_CTYPE, "");
    }
#endif
    if (XSupportsLocale()) {
        XSetLocaleModifiers("");

        m_im = XOpenIM(display, 0, nullptr, nullptr);
        if (m_im) {
            if (!CheckIM(m_im)) {
                XCloseIM(m_im);
                m_im = nullptr;
            }

            m_ic = XCreateIC(m_im, XNInputStyle, XIMPreeditNothing | XIMStatusNothing, XNClientWindow,
                window, XNFocusWindow, window, nullptr);
        }
    }
}

void X11InputHandler::Handle(XKeyEvent& xKeyEvent) {
    int count;
    if (m_ic == nullptr) {
        char strBuff[256];
        count = XLookupString(&xKeyEvent, strBuff, sizeof(strBuff) / sizeof(strBuff[0]), nullptr, nullptr);
        static std::wstring_convert<std::codecvt_utf8<wchar_t>> xconv;
        m_handler->OnInputEvent(xconv.from_bytes(strBuff, strBuff + count));
        return;
    }

    Status status;
#if defined(USE_UTF8)
    using TChar = char;
    static size_t bufferSize = 256;
    static TChar* buffer = reinterpret_cast<TChar*>(calloc(bufferSize, sizeof(TChar)));

    count = Xutf8LookupString(m_ic, &xKeyEvent, buffer, static_cast<int>(bufferSize), nullptr, &status);
    if (status == XBufferOverflow) {
        free(buffer);
        bufferSize = static_cast<size_t>(count);
        buffer = reinterpret_cast<TChar*>(calloc(bufferSize, sizeof(TChar)));
        count = Xutf8LookupString(m_ic, &xKeyEvent, buffer, static_cast<int>(bufferSize), nullptr, &status);
    }

    if (status == XLookupChars || status == XLookupBoth) {
        static std::wstring_convert<std::codecvt_utf8<wchar_t>> xutf8conv;
        m_handler->OnInputEvent(xutf8conv.from_bytes(buffer, buffer + count));
    }
#else /*USE_UTF8*/
    using TChar = wchar_t;
    static size_t bufferSize = 256;
    static TChar* buffer = reinterpret_cast<TChar*>(calloc(bufferSize, sizeof(TChar)));

    count = XwcLookupString(m_ic, &xKeyEvent, buffer, static_cast<int>(bufferSize), nullptr, &status);
    if (status == XBufferOverflow) {
        free(buffer);
        bufferSize = static_cast<size_t>(count);
        buffer = reinterpret_cast<TChar*>(calloc(bufferSize, sizeof(TChar)));
        count = XwcLookupString(m_ic, &xKeyEvent, buffer, static_cast<int>(bufferSize), nullptr, &status);
    }

    if (status == XLookupChars || status == XLookupBoth) {
        m_handler->OnInputEvent(std::wstring(buffer, static_cast<size_t>(count)));
    }
#endif /*USE_UTF8*/
}
