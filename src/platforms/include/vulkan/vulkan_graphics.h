#pragma once

#include <DiligentCore/Graphics/GraphicsEngine/interface/GraphicsTypes.h>

#include "graphics.h"


class VulkanGraphics : public Graphics {
public:
    VulkanGraphics() = delete;
    VulkanGraphics(uint32_t window, void* connection);
    ~VulkanGraphics() = default;

    void Create(int validationLevel = -1);
    void Destroy();

    Diligent::EngineVkCreateInfo& GetCreateInfo() noexcept { return m_createInfo; }

private:
    uint32_t m_window;
    void* m_connection;
    Diligent::EngineVkCreateInfo m_createInfo;
};
