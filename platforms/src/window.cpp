#include "platforms/window.h"

#include <algorithm>
#include <stdexcept>


RenderWindow::RenderWindow(const WindowDesc& desc, const std::shared_ptr<WindowEventsHandler>& handler)
    : m_desc(desc)
    , m_eventHandler(handler) {

}

void RenderWindow::SetMonitorSize(uint16_t monitorWidth, uint16_t monitorHeight) {
    m_desc.minWidth = std::max(m_desc.minWidth, uint16_t(10));
    m_desc.minHeight = std::max(m_desc.minHeight, uint16_t(10));

    if (m_desc.sizeType == WindowDesc::Size::Absolute) {
        m_desc.width = std::max(m_desc.width, m_desc.minWidth);
        m_desc.height = std::max(m_desc.height, m_desc.minWidth);
    } else if (m_desc.sizeType == WindowDesc::Size::PercentOfScreen) {
        auto width = static_cast<uint16_t>(static_cast<float>(monitorWidth) * m_desc.widthPercent / 100.f);
        auto height = static_cast<uint16_t>(static_cast<float>(monitorHeight) * m_desc.heightPercent / 100.f);
        m_desc.width = std::max(width, m_desc.minWidth);
        m_desc.height = std::max(height, m_desc.minHeight);
    } else if (m_desc.sizeType == WindowDesc::Size::Default) {
        auto width = static_cast<uint16_t>(static_cast<float>(monitorWidth) * 0.8f);
        auto height = static_cast<uint16_t>(static_cast<float>(monitorHeight) * 0.8f);
        m_desc.width = std::max(width, m_desc.minWidth);
        m_desc.height = std::max(height, m_desc.minHeight);
    } else {
        throw std::runtime_error("unknown value of sizeType = " + std::to_string(static_cast<uint8_t>(m_desc.sizeType)));
    }

    if (m_desc.positionType == WindowDesc::Position::Absolute) {
        // pass
    } else if (m_desc.positionType == WindowDesc::Position::CenterOfScreen) {
        m_desc.positionX = (monitorWidth - m_desc.width) / 2;
        m_desc.positionY = (monitorHeight - m_desc.height) / 2;
    } else if (m_desc.positionType == WindowDesc::Position::Default) {
        m_desc.positionX = (monitorWidth - m_desc.width) / 2;
        m_desc.positionY = (monitorHeight - m_desc.height) / 2;
    } else {
        throw std::runtime_error("unknown value of positionType = " + std::to_string(static_cast<uint8_t>(m_desc.positionType)));
    }
}
