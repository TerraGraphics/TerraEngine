#include "middleware/imgui/field.h"

#include "imgui/imgui.h"
#include "imgui/internal.h"
#include "imgui/gui_helpers.h"
#include "middleware/imgui/imgui_math.h"



namespace {

struct BackupState {
    BackupState(const ImGuiWindowTempData& dc)
        : cursorPos(dc.CursorPos)
        , cursorPosPrevLine(dc.CursorPosPrevLine)
        , cursorStartPos(dc.CursorStartPos)
        , cursorMaxPos(dc.CursorMaxPos)
        , currLineSize(dc.CurrLineSize)
        , prevLineSize(dc.PrevLineSize)
        , currLineTextBaseOffset(dc.CurrLineTextBaseOffset)
        , prevLineTextBaseOffset(dc.PrevLineTextBaseOffset)
        , indent(dc.Indent)
        , columnsOffset(dc.ColumnsOffset)
        , groupOffset(dc.GroupOffset) {}

    void Restore(ImGuiWindowTempData& dc) const {
        dc.CursorPos = cursorPos;
        dc.CursorPosPrevLine = cursorPosPrevLine;
        dc.CursorStartPos = cursorStartPos;
        dc.CursorMaxPos = cursorMaxPos;
        dc.CurrLineSize = currLineSize;
        dc.PrevLineSize = prevLineSize;
        dc.CurrLineTextBaseOffset = currLineTextBaseOffset;
        dc.PrevLineTextBaseOffset = prevLineTextBaseOffset;
        dc.Indent = indent;
        dc.ColumnsOffset = columnsOffset;
        dc.GroupOffset = groupOffset;
    }

    const ImVec2 cursorPos;
    const ImVec2 cursorPosPrevLine;
    const ImVec2 cursorStartPos;
    const ImVec2 cursorMaxPos;
    const ImVec2 currLineSize;
    const ImVec2 prevLineSize;
    const float currLineTextBaseOffset;
    const float prevLineTextBaseOffset;
    const ImVec1 indent;
    const ImVec1 columnsOffset;
    const ImVec1 groupOffset;
};

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

bool TextField(std::string_view strId, std::string& text, const FieldStyle& style, math::RectF* outWidgetRect) {
    bool changed = false;

    ImGuiWindow* window = GetCheckedCurrentWindow(outWidgetRect);
    if (window == nullptr) {
        return changed;
    }

    ImGuiContext& g = *GImGui;
    const auto drawSize = math::SizeF(style.width, g.FontSize + g.Style.FramePadding.y * 2.0f);

    math::RectF fullRect;
    math::RectF drawRect;
    math::RectF widgetRect;
    PlaceWidgetCalc(&style, drawSize, &drawRect, &widgetRect, &fullRect);
    if (outWidgetRect != nullptr) {
        *outWidgetRect = widgetRect;
    }

    const char label[] = "";
    const char* hint = nullptr;
    char* buf = text.data();
    int bufSize = static_cast<int>(text.capacity()) + 1;
    const ImVec2 sizeArg = ImVec2(0, 0);
    ImGuiInputTextFlags flags = ImGuiInputTextFlags_CallbackResize;
    ImGuiInputTextCallback callback = InputTextExCallback;
    void* callbackUserData = reinterpret_cast<void*>(&text);

    const auto backupState = BackupState(window->DC);

    window->DC.CursorPos.x = drawRect.x;
    window->DC.CursorPos.y = drawRect.y;
    ImGui::SetNextItemWidth(drawRect.w);

    ImGui::PushID(strId.cbegin(), strId.cend());
    auto id = window->GetID(label);
    changed = ImGui::InputTextEx(label, hint, buf, bufSize, sizeArg, flags, callback, callbackUserData);
    ImGui::PopID();

    backupState.Restore(window->DC);

    ItemSize(fullRect.Size());
    if (!ItemAdd(id, widgetRect)) {
        return changed;
    }

    DrawTooltip(&style);

    return changed;
}

} // end namespace gui
