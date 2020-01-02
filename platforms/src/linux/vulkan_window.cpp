#include "platforms/linux/vulkan_window.h"

#include <xcb/xcb_icccm.h>
#include <xcb/xcb_cursor.h>
#include <xcb/xcb_keysyms.h>

#include "platforms/linux/x11_key_map.h"


static std::string ParseXCBConnectError(int err) {
    switch (err) {
    case XCB_CONN_ERROR:
        return "XCB_CONN_ERROR: because of socket errors, pipe errors or other stream errors";
    case XCB_CONN_CLOSED_EXT_NOTSUPPORTED:
        return "XCB_CONN_CLOSED_EXT_NOTSUPPORTED: when extension not supported";
    case XCB_CONN_CLOSED_MEM_INSUFFICIENT:
        return "XCB_CONN_CLOSED_MEM_INSUFFICIENT: when memory not available";
    case XCB_CONN_CLOSED_REQ_LEN_EXCEED:
        return "XCB_CONN_CLOSED_REQ_LEN_EXCEED: exceeding request length that server accepts";
    case XCB_CONN_CLOSED_PARSE_ERR:
        return "XCB_CONN_CLOSED_PARSE_ERR: error during parsing display string";
    case XCB_CONN_CLOSED_INVALID_SCREEN:
        return "XCB_CONN_CLOSED_INVALID_SCREEN: because the server does not have a screen matching the display";
    default:
        return "unknown error";
    }
}

WindowVulkanLinux::WindowVulkanLinux(const std::shared_ptr<WindowEventsHandler>& handler)
    : RenderWindow(handler) {

}

WindowVulkanLinux::~WindowVulkanLinux() {
    Destroy();
}

void WindowVulkanLinux::Create(int16_t posX, int16_t posY, uint16_t width, uint16_t height, int screenNumber, const std::string& name) {
    m_connection = xcb_connect(nullptr, &screenNumber);
    if (int err = xcb_connection_has_error(m_connection); err != 0) {
        throw std::runtime_error("unable to make an XCB connection: " + ParseXCBConnectError(err));
    }

    const xcb_setup_t* setup = xcb_get_setup(m_connection);
    xcb_screen_iterator_t screenIt = xcb_setup_roots_iterator(setup);
    while (screenNumber-- > 0) {
        xcb_screen_next(&screenIt);
    }

    m_screen = screenIt.data;
    uint32_t valueMask = XCB_CW_BACK_PIXEL | XCB_CW_EVENT_MASK;
    uint32_t valueList[32];
    valueList[0] = m_screen->black_pixel;
    valueList[1] =
        XCB_EVENT_MASK_KEY_RELEASE |
        XCB_EVENT_MASK_KEY_PRESS |
        XCB_EVENT_MASK_EXPOSURE |
        XCB_EVENT_MASK_STRUCTURE_NOTIFY |
        XCB_EVENT_MASK_POINTER_MOTION |
        XCB_EVENT_MASK_BUTTON_PRESS |
        XCB_EVENT_MASK_BUTTON_RELEASE;

    m_window = xcb_generate_id(m_connection);
    xcb_create_window(m_connection, XCB_COPY_FROM_PARENT, m_window, m_screen->root, posX, posY, width, height, 0,
                      XCB_WINDOW_CLASS_INPUT_OUTPUT, m_screen->root_visual, valueMask, valueList);

    // Magic code that will send notification when window is destroyed
    xcb_intern_atom_cookie_t cookie = xcb_intern_atom(m_connection, 1, 12, "WM_PROTOCOLS");
    xcb_intern_atom_reply_t* reply  = xcb_intern_atom_reply(m_connection, cookie, 0);

    xcb_intern_atom_cookie_t cookie2 = xcb_intern_atom(m_connection, 0, 16, "WM_DELETE_WINDOW");
    m_atomWMDeleteWindow = xcb_intern_atom_reply(m_connection, cookie2, 0);

    xcb_change_property(m_connection, XCB_PROP_MODE_REPLACE, m_window, (*reply).atom, 4, 32, 1, &(*m_atomWMDeleteWindow).atom);
    free(reply);

    // Set WM_NAME property
    {
        xcb_change_property(m_connection, XCB_PROP_MODE_REPLACE, m_window, XCB_ATOM_WM_NAME, XCB_ATOM_STRING, 8,
        static_cast<uint32_t>(name.length()), name.c_str());
    }

    // Set WM_CLASS property
    {
        std::string classInstance = name +std::string("\0", 1) + name;
        xcb_change_property(m_connection, XCB_PROP_MODE_REPLACE, m_window, XCB_ATOM_WM_CLASS, XCB_ATOM_STRING, 8,
            static_cast<uint32_t>(classInstance.length()), classInstance.c_str());
    }

    // Set WM_NORMAL_HINTS property
    {
        xcb_size_hints_t hints = {};
        hints.flags = XCB_ICCCM_SIZE_HINT_P_MIN_SIZE;
        hints.min_width = 320;
        hints.min_height = 240;
        xcb_change_property(m_connection, XCB_PROP_MODE_REPLACE, m_window, XCB_ATOM_WM_NORMAL_HINTS, XCB_ATOM_WM_SIZE_HINTS, 32,
            static_cast<uint32_t>(sizeof(xcb_size_hints_t)), &hints);
    }

    // Create cursors
    {
        CreateCursors();
        SetCursor(CursorType::Arrow);
        GetCursorPos(m_lastCursorPosX, m_lastCursorPosY);
    }

    xcb_map_window(m_connection, m_window);
    xcb_flush(m_connection);

    xcb_generic_event_t* event = nullptr;
    HandleSizeEvent(width, height);
    while ((event = xcb_wait_for_event(m_connection)) != nullptr) {
        auto type = (event->response_type & ~0x80);
        if (type == XCB_CONFIGURE_NOTIFY) {
            const auto* configureEvent = reinterpret_cast<const xcb_configure_notify_event_t*>(event);
            HandleSizeEvent(configureEvent->width, configureEvent->height);
        } else if (type == XCB_EXPOSE) {
            free(event);
            break;
        }
        free (event);
    }

    if (m_keySymbols == nullptr) {
        m_keySymbols = xcb_key_symbols_alloc(m_connection);
    }
}

void WindowVulkanLinux::Destroy() {
    if (m_keySymbols != nullptr) {
        xcb_key_symbols_free(m_keySymbols);
        m_keySymbols = nullptr;
    }

    DestroyCursors();

    if (m_atomWMDeleteWindow != nullptr) {
        free(m_atomWMDeleteWindow);
        m_atomWMDeleteWindow = nullptr;
    }

    if (m_connection != nullptr) {
        xcb_destroy_window(m_connection, m_window);
        xcb_disconnect(m_connection);
        m_connection = nullptr;
        m_window = 0;
        m_screen = nullptr;
    }
}

void WindowVulkanLinux::SetTitle(const std::string& title) {
    auto titleLen = static_cast<uint32_t>(title.length());
    xcb_change_property(m_connection, XCB_PROP_MODE_REPLACE, m_window, XCB_ATOM_WM_NAME, XCB_ATOM_STRING, 8, titleLen, title.c_str());
    xcb_flush(m_connection);
}

void WindowVulkanLinux::GetCursorPos(int& x, int& y) {
    xcb_query_pointer_reply_t *pointer = xcb_query_pointer_reply(m_connection,
				xcb_query_pointer(m_connection, m_window), nullptr);

    if (pointer == nullptr) {
        std::runtime_error("failed to get cursor position");
    }
    x = pointer->win_x;
    y = pointer->win_y;
}

void WindowVulkanLinux::SetCursorPos(int x, int y) {
    m_lastCursorPosX = x;
    m_lastCursorPosY = y;
    xcb_warp_pointer(m_connection, XCB_NONE, m_window, 0, 0, 0, 0, static_cast<int16_t>(x), static_cast<int16_t>(y));
    xcb_flush(m_connection);
}

void WindowVulkanLinux::SetCursor(CursorType value) {
    if ((m_currentCursorType == value) || (value >= CursorType::Undefined)) {
        return;
    }

    if (m_currentCursorType == CursorType::Disabled) {
        EnableCursor();
        SetCursorPos(m_visibleCursorPosX, m_visibleCursorPosY);
    }

    m_currentCursorType = value;

    uint32_t cursor[1];
    if (value <= CursorType::LastStandartCursor) {
        cursor[0] = m_cursors[static_cast<uint>(value)];
    } else {
        cursor[0] = m_hiddenCursor;
    }

    xcb_change_window_attributes(m_connection, m_window, XCB_CW_CURSOR, reinterpret_cast<const void*>(&cursor));
    if (value == CursorType::Disabled) {
        GetCursorPos(m_visibleCursorPosX, m_visibleCursorPosY);
        DisableCursor();
        SetCursorPos(m_windowCenterX, m_windowCenterY);
    }
    xcb_flush(m_connection);
}

void WindowVulkanLinux::ProcessEvents() {
    m_eventHandler->OnNewFrame();
    xcb_generic_event_t* event = nullptr;
    while ((event = xcb_poll_for_event(m_connection)) != nullptr) {
        switch (event->response_type & 0x7f) { // 0b1111111
            // 0b100001
            case XCB_CLIENT_MESSAGE: {
                const auto* typedEvent = reinterpret_cast<const xcb_client_message_event_t*>(event);
                if (typedEvent->data.data32[0] == m_atomWMDeleteWindow->atom) {
                    m_eventHandler->OnWindowDestroy();
                }
            }
            break;

            // 0b10001
            case XCB_DESTROY_NOTIFY:
                m_eventHandler->OnWindowDestroy();
                break;

            // 0b10110
            case XCB_CONFIGURE_NOTIFY: {
                const auto* typedEvent = reinterpret_cast<const xcb_configure_notify_event_t*>(event);
                HandleSizeEvent(typedEvent->width, typedEvent->height);
            }
            break;

            // 0b10
            case XCB_KEY_PRESS: {
                const auto* typedEvent = reinterpret_cast<const xcb_key_press_event_t*>(event);
                HandleKeyEvent(KeyAction::Press, typedEvent->detail, typedEvent->state);
            }
            break;

            // 0b11
            case XCB_KEY_RELEASE: {
                const auto* typedEvent = reinterpret_cast<const xcb_key_release_event_t*>(event);
                HandleKeyEvent(KeyAction::Release, typedEvent->detail, typedEvent->state);
            }
            break;

            // 0b100
            case XCB_BUTTON_PRESS: {
                const auto* typedEvent = reinterpret_cast<const xcb_button_press_event_t*>(event);
                HandleMouseButtonEvent(KeyAction::Press, typedEvent->detail, typedEvent->state);
            }
            break;

            // 0b101
            case XCB_BUTTON_RELEASE: {
                const auto* typedEvent = reinterpret_cast<const xcb_button_release_event_t*>(event);
                HandleMouseButtonEvent(KeyAction::Release, typedEvent->detail, typedEvent->state);
            }
            break;

            // 0b110
            case XCB_MOTION_NOTIFY: {
                const auto* typedEvent = reinterpret_cast<const xcb_motion_notify_event_t*>(event);
                if (m_currentCursorType == CursorType::Disabled) {
                    if ((typedEvent->event_x == m_lastCursorPosX) && (typedEvent->event_y == m_lastCursorPosY)) {
                        break;
                    }
                    auto dtX = static_cast<double>(typedEvent->event_x - m_lastCursorPosX);
                    auto dtY = static_cast<double>(typedEvent->event_y - m_lastCursorPosY);
                    m_virtualCursorX += dtX;
                    m_virtualCursorY += dtY;
                    m_eventHandler->OnCursorPosition(m_virtualCursorX, m_virtualCursorY);
                } else {
                    m_eventHandler->OnCursorPosition(static_cast<double>(typedEvent->event_x), static_cast<double>(typedEvent->event_y));
                }
                m_lastCursorPosX = typedEvent->event_x;
                m_lastCursorPosY = typedEvent->event_y;
            }
            break;

            default:
                break;
        }
        free(event);
    }

    if ((m_currentCursorType == CursorType::Disabled) && ((m_lastCursorPosX != m_windowCenterX) || (m_lastCursorPosY != m_windowCenterY))) {
        SetCursorPos(m_windowCenterX, m_windowCenterY);
    }
}

void WindowVulkanLinux::CreateCursors() {
    if (xcb_cursor_context_new(m_connection, m_screen, &m_cursorContext) < 0) {
        throw std::runtime_error("failed create xbc cursor context");
    }
    m_cursors[static_cast<uint>(CursorType::Arrow)] = xcb_cursor_load_cursor(m_cursorContext, "left_ptr");
    m_cursors[static_cast<uint>(CursorType::IBeam)] = xcb_cursor_load_cursor(m_cursorContext, "xterm");
    m_cursors[static_cast<uint>(CursorType::Crosshair)] = xcb_cursor_load_cursor(m_cursorContext, "crosshair");
    m_cursors[static_cast<uint>(CursorType::Hand)] = xcb_cursor_load_cursor(m_cursorContext, "hand2");
    m_cursors[static_cast<uint>(CursorType::ResizeH)] = xcb_cursor_load_cursor(m_cursorContext, "sb_h_double_arrow");
    m_cursors[static_cast<uint>(CursorType::ResizeV)] = xcb_cursor_load_cursor(m_cursorContext, "sb_v_double_arrow");

    const xcb_pixmap_t pixmap = xcb_generate_id(m_connection);
    m_hiddenCursor = xcb_generate_id(m_connection);
    xcb_create_pixmap(m_connection,
        1, // bits per pixel
        pixmap,
        m_screen->root,
        1, // width
        1  // height
    );

    xcb_create_cursor(m_connection, m_hiddenCursor, pixmap, pixmap, 0, 0, 0, 0, 0, 0, 0, 0);
    xcb_free_pixmap(m_connection, pixmap);
}

void WindowVulkanLinux::DestroyCursors() {
    if (m_cursorContext == nullptr) {
        return;
    }

    for (uint i=0; i!=static_cast<uint>(CursorType::LastStandartCursor) + 1; ++i) {
        if (m_cursors[i] != 0) {
            xcb_free_cursor(m_connection, m_cursors[i]);
            m_cursors[i] = 0;
        }
    }

    if (m_hiddenCursor != 0) {
        xcb_free_cursor(m_connection, m_hiddenCursor);
    }

    xcb_cursor_context_free(m_cursorContext);
    m_cursorContext = nullptr;
}

void WindowVulkanLinux::DisableCursor() {
    xcb_grab_pointer_cookie_t cookie = xcb_grab_pointer(m_connection, 1, m_window,
        XCB_EVENT_MASK_BUTTON_PRESS | XCB_EVENT_MASK_BUTTON_RELEASE | XCB_EVENT_MASK_POINTER_MOTION,
        XCB_GRAB_MODE_ASYNC, XCB_GRAB_MODE_ASYNC,
        m_window,
        m_hiddenCursor,
        XCB_CURRENT_TIME);

    xcb_grab_pointer_reply_t* grabReply = xcb_grab_pointer_reply(m_connection, cookie, nullptr);
    if ((grabReply == nullptr) || (grabReply->status != XCB_GRAB_STATUS_SUCCESS)) {
        std::runtime_error("failed to disable cursor");
    }
    xcb_flush(m_connection);
}

void WindowVulkanLinux::EnableCursor() {
    xcb_ungrab_pointer(m_connection, XCB_TIME_CURRENT_TIME);
    xcb_flush(m_connection);
}

void WindowVulkanLinux::HandleSizeEvent(uint32_t width, uint32_t height) {
    m_windowCenterX = width / 2;
    m_windowCenterY = height / 2;
    m_eventHandler->OnWindowSizeEvent(width, height);
}

void WindowVulkanLinux::HandleKeyEvent(KeyAction action, uint8_t code, uint state) {
    m_eventHandler->OnKeyEvent(action, KeySymToKey(xcb_key_symbols_get_keysym(m_keySymbols, code, 0)), StateToModifiers(state));
}

void WindowVulkanLinux::HandleMouseButtonEvent(KeyAction action, uint8_t code, uint state) {
    switch (code) {
        case XCB_BUTTON_INDEX_1:
            m_eventHandler->OnKeyEvent(action, Key::MouseLeft, StateToModifiers(state));
            break;
        case XCB_BUTTON_INDEX_2:
            m_eventHandler->OnKeyEvent(action, Key::MouseMiddle, StateToModifiers(state));
            break;
        case XCB_BUTTON_INDEX_3:
            m_eventHandler->OnKeyEvent(action, Key::MouseRight, StateToModifiers(state));
            break;
        case XCB_BUTTON_INDEX_4:
            m_eventHandler->OnScroll(1);
            break;
        case XCB_BUTTON_INDEX_5:
            m_eventHandler->OnScroll(-1);
            break;
        default:
            break;
    }
}
