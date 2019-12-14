#include "linux/vulkan_window.h"

#include <xcb/xcb_icccm.h>
#include "core/common/exception.h"


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

void WindowVulkanLinux::Create(int16_t posX, int16_t posY, uint16_t width, uint16_t height, int screenNumber, const std::string& title) {
    m_width = width;
    m_height = height;

    m_connection = xcb_connect(nullptr, &screenNumber);
    if (int err = xcb_connection_has_error(m_connection); err != 0) {
        throw EngineError("unable to make an XCB connection, {}", ParseXCBConnectError(err));
    }

    const xcb_setup_t* setup = xcb_get_setup(m_connection);
    xcb_screen_iterator_t screenIt = xcb_setup_roots_iterator(setup);
    while (screenNumber-- > 0) {
        xcb_screen_next(&screenIt);
    }

    auto screen = screenIt.data;
    uint32_t valueMask = XCB_CW_BACK_PIXEL | XCB_CW_EVENT_MASK;
    uint32_t valueList[32];
    valueList[0] = screen->black_pixel;
    valueList[1] =
        XCB_EVENT_MASK_KEY_RELEASE |
        XCB_EVENT_MASK_KEY_PRESS |
        XCB_EVENT_MASK_EXPOSURE |
        XCB_EVENT_MASK_STRUCTURE_NOTIFY |
        XCB_EVENT_MASK_POINTER_MOTION |
        XCB_EVENT_MASK_BUTTON_PRESS |
        XCB_EVENT_MASK_BUTTON_RELEASE;

    m_window = xcb_generate_id(m_connection);
    xcb_create_window(m_connection, XCB_COPY_FROM_PARENT, m_window, screen->root, posX, posY, m_width, m_height, 0,
                      XCB_WINDOW_CLASS_INPUT_OUTPUT, screen->root_visual, valueMask, valueList);

    // Magic code that will send notification when window is destroyed
    xcb_intern_atom_cookie_t cookie = xcb_intern_atom(m_connection, 1, 12, "WM_PROTOCOLS");
    xcb_intern_atom_reply_t* reply  = xcb_intern_atom_reply(m_connection, cookie, 0);

    xcb_intern_atom_cookie_t cookie2 = xcb_intern_atom(m_connection, 0, 16, "WM_DELETE_WINDOW");
    m_atomWMDeleteWindow = xcb_intern_atom_reply(m_connection, cookie2, 0);

    xcb_change_property(m_connection, XCB_PROP_MODE_REPLACE, m_window, (*reply).atom, 4, 32, 1, &(*m_atomWMDeleteWindow).atom);
    free(reply);

    xcb_change_property(m_connection, XCB_PROP_MODE_REPLACE, m_window, XCB_ATOM_WM_NAME, XCB_ATOM_STRING, 8, title.length(), title.c_str());

    // https://stackoverflow.com/a/27771295
    xcb_size_hints_t hints = {};
    hints.flags = XCB_ICCCM_SIZE_HINT_P_SIZE;
    hints.min_width = 320;
    hints.min_height = 240;
    xcb_change_property(m_connection, XCB_PROP_MODE_REPLACE, m_window, XCB_ATOM_WM_NORMAL_HINTS, XCB_ATOM_WM_SIZE_HINTS,
        32, sizeof(xcb_size_hints_t), &hints);

    xcb_map_window(m_connection, m_window);

    // Force the x/y coordinates to 100,100 results are identical in consecutive runs
    const uint32_t coords[] = {100, 100};
    xcb_configure_window(m_connection, m_window, XCB_CONFIG_WINDOW_X | XCB_CONFIG_WINDOW_Y, coords);
    xcb_flush(m_connection);

    xcb_generic_event_t* e;
    while ((e = xcb_wait_for_event(m_connection))) {
        if ((e->response_type & ~0x80) == XCB_EXPOSE) break;
    }
}

void WindowVulkanLinux::Destroy() {
    xcb_destroy_window(m_connection, m_window);
    xcb_disconnect(m_connection);
}

void WindowVulkanLinux::Process(const xcb_generic_event_t* event) {
    const auto* cfgEvent = reinterpret_cast<const xcb_configure_notify_event_t*>(event);
    if ((cfgEvent->width != m_width) || (cfgEvent->height != m_height)) {
        m_width  = cfgEvent->width;
        m_height = cfgEvent->height;
    }
}
