#include "platforms/linux/vulkan_window.h"

#include <thread>
#include <cstring>

#include <X11/Xlib-xcb.h>
#include <xcb/xcb_icccm.h>
#include <xcb/xcb_cursor.h>
#include <xcb/xcb_keysyms.h>

#ifdef Bool
#    undef Bool
#endif
#ifdef True
#    undef True
#endif
#ifdef False
#    undef False
#endif

#include <DiligentCore/Primitives/interface/Errors.h>
#include <DiligentCore/Platforms/Linux/interface/LinuxPlatformDefinitions.h>

#include "linux/x11_key_map.h"
#include "linux/x11_input_handler.h"

enum ATOM : uint32_t {
    WM_PROTOCOLS = 0,
    WM_DELETE_WINDOW = 1,
    TARGETS = 2,
    TIMESTAMP = 3,
    CLIPBOARD = 4,
    UTF8_STRING = 5,
    ATOMS_NUMBER = 6,
};

static const char* ATOMS_STR[ATOMS_NUMBER] = {"WM_PROTOCOLS", "WM_DELETE_WINDOW", "TARGETS", "TIMESTAMP", "CLIPBOARD", "UTF8_STRING"};

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

WindowVulkanLinux::WindowVulkanLinux(const WindowDesc& desc, const std::shared_ptr<WindowEventsHandler>& handler)
    : RenderWindow(desc, handler)
    , m_inputParser(new X11InputHandler(handler))
    , m_atoms(new uint32_t[ATOMS_NUMBER]) {

}

WindowVulkanLinux::~WindowVulkanLinux() {
    if (m_inputParser != nullptr) {
        delete m_inputParser;
        m_inputParser = nullptr;
    }
    if (m_atoms != nullptr) {
        delete[] m_atoms;
        m_atoms = nullptr;
    }
    Destroy();
}

void WindowVulkanLinux::SetTitle(const char* text) {
    auto textLen = static_cast<uint32_t>(strlen(text));
    xcb_change_property(m_connection, XCB_PROP_MODE_REPLACE, m_window, XCB_ATOM_WM_NAME, XCB_ATOM_STRING, 8, textLen, text);
    xcb_flush(m_connection);
}

void WindowVulkanLinux::SetClipboard(const char* text) {
    auto cookie = xcb_set_selection_owner(m_connection, m_window, m_atoms[CLIPBOARD], XCB_CURRENT_TIME);
    xcb_generic_error_t* err = xcb_request_check(m_connection, cookie);
    if (err == nullptr) {
        m_clipboard = text;
    } else {
        free(err);
    }
}

const char* WindowVulkanLinux::GetClipboard() {
    xcb_get_selection_owner_cookie_t cookie = xcb_get_selection_owner(m_connection, m_atoms[CLIPBOARD]);
    xcb_get_selection_owner_reply_t *reply = xcb_get_selection_owner_reply(m_connection, cookie, nullptr);
    std::shared_ptr<xcb_get_selection_owner_reply_t> autoFree(reply, free);
    if ((reply != nullptr) && (reply->owner == m_window)) {
        return m_clipboard.c_str();
    }
    m_clipboard.clear();
    if ((reply == nullptr) || (reply->owner == 0)) {
        return m_clipboard.c_str();
    }

    xcb_convert_selection(m_connection, m_window, m_atoms[CLIPBOARD], m_atoms[UTF8_STRING], m_atoms[CLIPBOARD], XCB_CURRENT_TIME);
    xcb_flush(m_connection);

    auto now = std::chrono::high_resolution_clock().now() + std::chrono::milliseconds(500);
    while (now > std::chrono::high_resolution_clock().now()) {
        xcb_generic_event_t* event = xcb_poll_for_event(m_connection);
        if (event == nullptr) {
            std::this_thread::sleep_for(std::chrono::microseconds(1));
            continue;
        }
        if ((event->response_type & 0x7f) == XCB_SELECTION_NOTIFY) {
            m_clipboard = HandleSelectionNotify(reinterpret_cast<const xcb_selection_notify_event_t*>(event));
            free(event);
            break;
        }
        m_events.push_back(event);
    }

    return m_clipboard.c_str();
}

void WindowVulkanLinux::GetCursorPos(int& x, int& y) {
    xcb_query_pointer_reply_t *pointer = xcb_query_pointer_reply(m_connection,
				xcb_query_pointer(m_connection, m_window), nullptr);
    if (pointer != nullptr) {
        x = pointer->win_x;
        y = pointer->win_y;
        free(pointer);
    } else {
        std::runtime_error("failed to get cursor position");
    }
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
    }

    m_currentCursorType = value;

    uint32_t cursor[1];
    if (value != CursorType::Disabled) {
        cursor[0] = m_cursors[static_cast<uint>(value)];
        xcb_change_window_attributes(m_connection, m_window, XCB_CW_CURSOR, reinterpret_cast<const void*>(&cursor));
    } else {
        cursor[0] = m_cursors[static_cast<uint>(CursorType::Empty)];
        xcb_change_window_attributes(m_connection, m_window, XCB_CW_CURSOR, reinterpret_cast<const void*>(&cursor));
        GetCursorPos(m_visibleCursorPosX, m_visibleCursorPosY);
        DisableCursor();
        SetCursorPos(m_windowCenterX, m_windowCenterY);
    }

    xcb_flush(m_connection);
}

void WindowVulkanLinux::Create() {
    // Connect to X server
    m_display = XOpenDisplay(NULL);
    if (m_display == nullptr) {
        throw std::runtime_error("failed to open display");
    }

    m_connection = XGetXCBConnection(m_display);
    if (m_connection == nullptr) {
        XCloseDisplay(m_display);
        m_display = nullptr;
        throw std::runtime_error("could not cast the Display object to an XCBConnection object");
    }

    if (int err = xcb_connection_has_error(m_connection); err != 0) {
        XCloseDisplay(m_display);
        m_display = nullptr;
        throw std::runtime_error("unable to make an XCB connection: " + ParseXCBConnectError(err));
    }

    // now queue owner is XCB
    XSetEventQueueOwner(m_display, XCBOwnsEventQueue);

    // Get screen
    {
        auto preferredScreenNumber = DefaultScreen(m_display);
        const xcb_setup_t* setup = xcb_get_setup(m_connection);
        xcb_screen_iterator_t screenIt = xcb_setup_roots_iterator(setup);
        while (preferredScreenNumber-- > 0) {
            xcb_screen_next(&screenIt);
        }
        m_screen = screenIt.data;
        SetMonitorSize(m_screen->width_in_pixels, m_screen->height_in_pixels);
    }

    uint32_t valueMask = XCB_CW_BACK_PIXEL | XCB_CW_EVENT_MASK;
    uint32_t valueList[32];
    valueList[0] = m_screen->black_pixel;
    valueList[1] =
        XCB_EVENT_MASK_KEY_PRESS |
        XCB_EVENT_MASK_KEY_RELEASE |
        XCB_EVENT_MASK_BUTTON_PRESS |
        XCB_EVENT_MASK_BUTTON_RELEASE |
        XCB_EVENT_MASK_POINTER_MOTION |
        XCB_EVENT_MASK_EXPOSURE |
        XCB_EVENT_MASK_STRUCTURE_NOTIFY |
        XCB_EVENT_MASK_FOCUS_CHANGE |
        XCB_EVENT_MASK_PROPERTY_CHANGE;

    m_window = xcb_generate_id(m_connection);
    xcb_create_window(m_connection, XCB_COPY_FROM_PARENT, m_window, m_screen->root,
                    m_desc.positionX, m_desc.positionY, m_desc.width, m_desc.height, 0,
                    XCB_WINDOW_CLASS_INPUT_OUTPUT, m_screen->root_visual, valueMask, valueList);

    m_inputParser->Create(m_display, static_cast<Window>(m_window));

    GetAtoms();

    // Magic code that will send notification when window is destroyed
    xcb_change_property(m_connection, XCB_PROP_MODE_REPLACE, m_window, m_atoms[WM_PROTOCOLS], 4, 32, 1, &m_atoms[WM_DELETE_WINDOW]);

    // Set WM_NAME property
    {
        xcb_change_property(m_connection, XCB_PROP_MODE_REPLACE, m_window, XCB_ATOM_WM_NAME, XCB_ATOM_STRING, 8,
            static_cast<uint32_t>(m_desc.name.length()), m_desc.name.c_str());
    }

    // Set WM_CLASS property
    {
        std::string classInstance = m_desc.name +std::string("\0", 1) + m_desc.name;
        xcb_change_property(m_connection, XCB_PROP_MODE_REPLACE, m_window, XCB_ATOM_WM_CLASS, XCB_ATOM_STRING, 8,
            static_cast<uint32_t>(classInstance.length()), classInstance.c_str());
    }

    // Set WM_NORMAL_HINTS property
    {
        xcb_size_hints_t hints = {};
        hints.flags = XCB_ICCCM_SIZE_HINT_P_MIN_SIZE;
        hints.min_width = m_desc.minWidth;
        hints.min_height = m_desc.minHeight;
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
    HandleSizeEvent(m_desc.width, m_desc.height);
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

    if (m_window != 0) {
        xcb_destroy_window(m_connection, m_window);
        m_window = 0;
    }

    if (m_connection != nullptr) {
        m_connection = nullptr;
        m_screen = nullptr;
    }

    if (m_display != nullptr) {
        XCloseDisplay(m_display);
        m_display = nullptr;
    }
}

void WindowVulkanLinux::ProcessEvents() {
    m_eventHandler->OnNewFrame();
    for(auto* event: m_events) {
        ProcessEvent(event);
    }
    m_events.clear();

    xcb_generic_event_t* event = nullptr;
    while ((event = xcb_poll_for_event(m_connection)) != nullptr) {
        ProcessEvent(event);
        free(event);
    }

    if (m_focused && (m_currentCursorType == CursorType::Disabled) && ((m_lastCursorPosX != m_windowCenterX) || (m_lastCursorPosY != m_windowCenterY))) {
        SetCursorPos(m_windowCenterX, m_windowCenterY);
    }
}

void WindowVulkanLinux::ProcessEvent(xcb_generic_event_t* event) {
    switch (event->response_type & 0x7f) { // 0b1111111
        // 0b10
        case XCB_KEY_PRESS: {
            const auto* typedEvent = reinterpret_cast<const xcb_key_press_event_t*>(event);
            HandleKeyEvent(KeyAction::Press, typedEvent->detail, typedEvent->state);
            XKeyEvent x11Event;
            x11Event.type = KeyPress;
            x11Event.serial = typedEvent->sequence;
            x11Event.send_event = 1; //True;
            x11Event.display = m_display;
            x11Event.window = typedEvent->event;
            x11Event.root = typedEvent->root;
            x11Event.subwindow = typedEvent->child;
            x11Event.time = typedEvent->time;
            x11Event.x = typedEvent->event_x;
            x11Event.y = typedEvent->event_y;
            x11Event.x_root = typedEvent->root_x;
            x11Event.y_root = typedEvent->root_y;
            x11Event.state = typedEvent->state;
            x11Event.keycode = typedEvent->detail;
            x11Event.same_screen = typedEvent->same_screen;
            m_inputParser->Handle(&x11Event);
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

        // 0b1001
        case XCB_FOCUS_IN: {
            if (m_focused) {
                break;
            }
            m_focused = true;
            if (m_currentCursorType == CursorType::Disabled) {
                DisableCursor();
            }

            m_inputParser->FocusChange(m_focused);
            m_eventHandler->OnFocusChange(m_focused);
        }
        break;

        // 0b1010
        case XCB_FOCUS_OUT: {
            if (!m_focused) {
                break;
            }
            m_focused = false;
            if (m_currentCursorType == CursorType::Disabled) {
                EnableCursor();
            }

            m_inputParser->FocusChange(m_focused);
            m_eventHandler->OnFocusChange(m_focused);
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

        // 0b11101
        case XCB_SELECTION_CLEAR:
            m_clipboard.clear();
            break;

        // 0b11110
        case XCB_SELECTION_REQUEST:
            HandleSelectionRequest(reinterpret_cast<const xcb_selection_request_event_t*>(event));
            break;

        // 0b11111
        case XCB_SELECTION_NOTIFY:
            break;

        // 0b100001
        case XCB_CLIENT_MESSAGE: {
            const auto* typedEvent = reinterpret_cast<const xcb_client_message_event_t*>(event);
            if (typedEvent->data.data32[0] == m_atoms[WM_DELETE_WINDOW]) {
                m_eventHandler->OnWindowDestroy();
            }
        }
        break;

        default:
            break;
    }
}

void WindowVulkanLinux::GetAtoms() {
    xcb_intern_atom_cookie_t cookies[ATOMS_NUMBER];
    for (uint32_t i=0; i!=ATOMS_NUMBER; ++i) {
        cookies[i] = xcb_intern_atom(m_connection, 0, static_cast<uint16_t>(strlen(ATOMS_STR[i])), ATOMS_STR[i]);
    }

    for (uint32_t i=0; i!=ATOMS_NUMBER; ++i) {
        xcb_intern_atom_reply_t *reply = xcb_intern_atom_reply(m_connection, cookies[i], nullptr);
        if (reply == nullptr) {
            auto msg = std::string("can't create atom ") + ATOMS_STR[i];
            throw std::runtime_error(msg);
        }

        m_atoms[i] = reply->atom;
        free(reply);
    }
}

void WindowVulkanLinux::CreateCursors() {
    if (xcb_cursor_context_new(m_connection, m_screen, &m_cursorContext) < 0) {
        throw std::runtime_error("failed create xbc cursor context");
    }
    // see http://tronche.com/gui/x/xlib/appendix/b
    m_cursors[static_cast<uint>(CursorType::Arrow)] = xcb_cursor_load_cursor(m_cursorContext, "left_ptr");
    m_cursors[static_cast<uint>(CursorType::TextInput)] = xcb_cursor_load_cursor(m_cursorContext, "xterm");
    m_cursors[static_cast<uint>(CursorType::ResizeAll)] = xcb_cursor_load_cursor(m_cursorContext, "fleur");
    m_cursors[static_cast<uint>(CursorType::ResizeNS)] = xcb_cursor_load_cursor(m_cursorContext, "sb_v_double_arrow");
    m_cursors[static_cast<uint>(CursorType::ResizeEW)] = xcb_cursor_load_cursor(m_cursorContext, "sb_h_double_arrow");
    m_cursors[static_cast<uint>(CursorType::ResizeNESW)] = xcb_cursor_load_cursor(m_cursorContext, "fd_double_arrow"); // nesw-resize
    m_cursors[static_cast<uint>(CursorType::ResizeNWSE)] = xcb_cursor_load_cursor(m_cursorContext, "bd_double_arrow"); // nwse-resize
    m_cursors[static_cast<uint>(CursorType::Hand)] = xcb_cursor_load_cursor(m_cursorContext, "hand2");
    m_cursors[static_cast<uint>(CursorType::NotAllowed)] = xcb_cursor_load_cursor(m_cursorContext, "not-allowed");

    const xcb_pixmap_t pixmap = xcb_generate_id(m_connection);
    m_cursors[static_cast<uint>(CursorType::Empty)] = xcb_generate_id(m_connection);
    xcb_create_pixmap(m_connection,
        1, // bits per pixel
        pixmap,
        m_screen->root,
        1, // width
        1  // height
    );

    xcb_create_cursor(m_connection, m_cursors[static_cast<uint>(CursorType::Empty)], pixmap, pixmap, 0, 0, 0, 0, 0, 0, 0, 0);
    xcb_free_pixmap(m_connection, pixmap);
}

void WindowVulkanLinux::DestroyCursors() {
    if (m_cursorContext == nullptr) {
        return;
    }

    for (uint i=0; i!=static_cast<uint>(CursorType::CountRealCursor); ++i) {
        if (m_cursors[i] != 0) {
            xcb_free_cursor(m_connection, m_cursors[i]);
            m_cursors[i] = 0;
        }
    }

    xcb_cursor_context_free(m_cursorContext);
    m_cursorContext = nullptr;
}

void WindowVulkanLinux::DisableCursor() {
    xcb_grab_pointer_cookie_t cookie = xcb_grab_pointer(m_connection, 1, m_window,
        XCB_EVENT_MASK_BUTTON_PRESS | XCB_EVENT_MASK_BUTTON_RELEASE | XCB_EVENT_MASK_POINTER_MOTION,
        XCB_GRAB_MODE_ASYNC, XCB_GRAB_MODE_ASYNC,
        m_window,
        m_cursors[static_cast<uint>(CursorType::Empty)],
        XCB_CURRENT_TIME);

    xcb_grab_pointer_reply_t* grabReply = xcb_grab_pointer_reply(m_connection, cookie, nullptr);
    bool error = ((grabReply == nullptr) || (grabReply->status != XCB_GRAB_STATUS_SUCCESS));
    if (grabReply != nullptr) {
        free(grabReply);
    }
    if (error) {
        std::runtime_error("failed to disable cursor");
    }
}

void WindowVulkanLinux::EnableCursor() {
    xcb_ungrab_pointer(m_connection, XCB_TIME_CURRENT_TIME);
    SetCursorPos(m_visibleCursorPosX, m_visibleCursorPosY);
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

std::string WindowVulkanLinux::HandleSelectionNotify(const xcb_selection_notify_event_t* event) {
    // TODO: add support INCR
    if (event->property != m_atoms[CLIPBOARD]) {
        LOG_WARNING_MESSAGE("Receiving the clipboard cannot be performed, wrong property for event XCB_SELECTION_NOTIFY");
        return std::string();
    }

    std::string clipboard;
    size_t offset = 0;
    uint8_t propFormat;
    xcb_atom_t propType;
    auto chunkSize = 1024 * 1024; // 1 MB

    for (;;) {
        xcb_get_property_cookie_t cookie = xcb_get_property(m_connection, true, m_window, event->property, XCB_ATOM_ANY,
            static_cast<uint32_t>(offset / 4), static_cast<uint32_t>(chunkSize / 4));
        xcb_get_property_reply_t* reply = xcb_get_property_reply(m_connection, cookie, nullptr);
        std::shared_ptr<xcb_get_property_reply_t> autoFree(reply, free);

        // reply->format should be 8, 16 or 32
        if (reply == nullptr || (offset > 0 && (reply->format != propFormat || reply->type != propType)) || ((reply->format % 8) != 0)) {
            LOG_WARNING_MESSAGE("Receiving the clipboard cannot be performed, invalid return value from xcb_get_property_reply");
            return std::string();
        }

        if (int numItems = xcb_get_property_value_length(reply); numItems > 0) {
            size_t itemSize = reply->format / 8;
            size_t propSize = static_cast<size_t>(numItems) * itemSize;
            offset += propSize;
            const uint8_t* begin = reinterpret_cast<const uint8_t*>(xcb_get_property_value(reply));
            clipboard.append(begin, begin + propSize);
        }

        if (reply->bytes_after == 0) {
            break;
        }

        if ((offset % 4) != 0) {
            LOG_WARNING_MESSAGE("Receiving the clipboard cannot be performed, got more data but read data size is not a multiple of 4");
            return std::string();
        }
        if (offset == 0) {
            propType = reply->type;
            propFormat = reply->format;
        }
    }

    return clipboard;
}

void WindowVulkanLinux::HandleSelectionRequest(const xcb_selection_request_event_t* event) {
    xcb_atom_t target = event->target;
    xcb_atom_t property = event->property;
    if (event->property == XCB_NONE) {
        property = event->target;
    }

    if (target == m_atoms[TARGETS]) {
        xcb_atom_t targets[] = {
            m_atoms[TIMESTAMP],
            m_atoms[TARGETS],
            m_atoms[UTF8_STRING]
        };
        xcb_change_property(m_connection, XCB_PROP_MODE_REPLACE, event->requestor, property, XCB_ATOM_ATOM,
            sizeof(xcb_atom_t) * 8, _countof(targets), targets);
    } else if (target == m_atoms[TIMESTAMP]) {
        xcb_timestamp_t cur = XCB_CURRENT_TIME;
        xcb_change_property(m_connection, XCB_PROP_MODE_REPLACE, event->requestor, property, XCB_ATOM_INTEGER,
            sizeof(cur) * 8, 1, &cur);
    } else if ((target == m_atoms[UTF8_STRING]) && (event->selection == m_atoms[CLIPBOARD]) && !m_clipboard.empty()) {
        xcb_change_property(m_connection, XCB_PROP_MODE_REPLACE, event->requestor, property, target,
            8, static_cast<uint32_t>(m_clipboard.length()), m_clipboard.c_str());
    } else {
        property = XCB_NONE;
    }

    xcb_selection_notify_event_t notify;
    notify.response_type = XCB_SELECTION_NOTIFY;
    notify.pad0 = 0;
    notify.sequence = 0;
    notify.time = XCB_CURRENT_TIME;
    notify.requestor = event->requestor;
    notify.selection = event->selection;
    notify.target = target;
    notify.property = property;
    xcb_send_event(m_connection, false, event->requestor, XCB_EVENT_MASK_PROPERTY_CHANGE, reinterpret_cast<char *>(&notify));
    xcb_flush(m_connection);
}
