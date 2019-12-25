#include "platforms/window.h"


RenderWindow::RenderWindow(const std::shared_ptr<WindowEventsHandler>& handler)
    : m_eventHandler(handler) {
}
