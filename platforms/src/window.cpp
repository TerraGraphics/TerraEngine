#include "platforms/window.h"


void RenderWindow::SetEventHandler(std::unique_ptr<WindowEventsHandler>&& handler) {
    m_eventHandler = std::move(handler);
}
