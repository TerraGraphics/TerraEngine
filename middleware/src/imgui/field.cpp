#include "middleware/imgui/field.h"

#include "imgui/imgui.h"
#include "imgui/internal.h"
#include "imgui/gui_helpers.h"
#include "middleware/imgui/imgui_math.h"



namespace {

static int InputTextExCallback(ImGuiInputTextCallbackData* data) {
    if (data->EventFlag == ImGuiInputTextFlags_CallbackResize) {
        std::string* str = reinterpret_cast<std::string*>(data->UserData);
        IM_ASSERT(data->Buf == str->c_str());
        str->resize(static_cast<size_t>(data->BufTextLen));
        data->Buf = str->data();
    }

    return 0;
}

}

namespace gui {

bool TextField(std::string_view strId, std::string& text, const FieldStyle& style, math::RectF* /* outWidgetRect */) {
    const char label[] = "";
    const char* hint = nullptr;
    char* buf = text.data();
    int bufSize = static_cast<int>(text.capacity()) + 1;
    const ImVec2 sizeArg = ImVec2(0, 0);
    ImGuiInputTextFlags flags = ImGuiInputTextFlags_CallbackResize;
    ImGuiInputTextCallback callback = InputTextExCallback;
    void* callbackUserData = reinterpret_cast<void*>(&text);

    ImGui::PushID(strId.cbegin(), strId.cend());
    bool changed = ImGui::InputTextEx(label, hint, buf, bufSize, sizeArg, flags, callback, callbackUserData);
    ImGui::PopID();
    DrawTooltip(&style);

    return changed;
}

} // end namespace gui
