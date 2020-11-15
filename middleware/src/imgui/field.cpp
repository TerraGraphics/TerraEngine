#include "middleware/imgui/field.h"

#include <cstddef>

#include "imgui/imgui.h"
#include "imgui/internal.h"
#include "imgui/gui_helpers.h"
#include "middleware/imgui/button.h"
#include "middleware/imgui/layout.h"


namespace {

struct BackupState {
    BackupState() = delete;
    explicit BackupState(const ImGuiWindowTempData& dc)
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

struct FieldData {
    bool integerFilter = false;
    bool floatingFilter = false;
    bool showStepButtons = false;
    std::string* text = nullptr;
};

static int InputTextExCallback(ImGuiInputTextCallbackData* data) {
    FieldData* fieldData = reinterpret_cast<FieldData*>(data->UserData);

    if (data->EventFlag == ImGuiInputTextFlags_CallbackResize) {
        IM_ASSERT(data->Buf == fieldData->text->c_str());
        fieldData->text->resize(static_cast<size_t>(data->BufTextLen));
        data->Buf = fieldData->text->data();
    } else if (data->EventFlag == ImGuiInputTextFlags_CallbackCharFilter) {
        if ((!fieldData->integerFilter) && (!fieldData->floatingFilter)) {
            return 0;
        }

        auto ch = data->EventChar;
        bool isValidForInteger = ((ch >= '0' && ch <= '9') || (ch == '-') || (ch == '+'));

        if (fieldData->integerFilter) {
            return isValidForInteger ? 0 : 1;
        }

        ImGuiContext& g = *GImGui;
        bool isDecimalPoint = (static_cast<decltype(ch)>(g.PlatformLocaleDecimalPoint) == ch);
        return (isValidForInteger || isDecimalPoint) ? 0 : 1;
    }

    return 0;
}

}

namespace gui {

NumberFieldAction TextFieldImpl(std::string_view strId, FieldData* fieldData, const FieldStyle* style, math::RectF* outWidgetRect) {
    NumberFieldAction result = NumberFieldAction::None;

    ImGuiWindow* window = GetCheckedCurrentWindow(outWidgetRect);
    if (window == nullptr) {
        return result;
    }

    math::RectF fullRect;
    math::RectF drawRect;
    math::RectF widgetRect;
    const auto drawSize = math::SizeF(style->width, GetDefaultFieldHeight());
    PlaceWidgetCalc(style, drawSize, &drawRect, &widgetRect, &fullRect);

    ButtonStyle stepButtonsStyle;
    if (fieldData->showStepButtons) {
        stepButtonsStyle.margin = style->margin;
        stepButtonsStyle.margin.left = 0;
        stepButtonsStyle.padding = style->padding;
        stepButtonsStyle.padding.left = 0;
        stepButtonsStyle.minWidgetSize.h = drawRect.h;
        stepButtonsStyle.minWidgetSize.w = GetStepButtonsWidth(drawRect.h);
        stepButtonsStyle.verticalAlign = style->verticalAlign;
        stepButtonsStyle.horisontalAlign = HorisontalAlign::Left;
        stepButtonsStyle.tooltip = style->tooltip;

        drawRect.w = std::max(drawRect.w - stepButtonsStyle.minWidgetSize.w, 0.f);
        widgetRect.Right(drawRect.Right());
        fullRect.Right(drawRect.Right());
        BeginHorizontal();
    }

    if (outWidgetRect != nullptr) {
        *outWidgetRect = widgetRect;
    }

    const char label[] = "";
    const char* hint = nullptr;
    char* buf = fieldData->text->data();
    int bufSize = static_cast<int>(fieldData->text->capacity()) + 1;
    const ImVec2 sizeArg = ImVec2(0, 0);
    ImGuiInputTextFlags flags = ImGuiInputTextFlags_CallbackResize;
    if (fieldData->integerFilter || fieldData->floatingFilter) {
        flags |= ImGuiInputTextFlags_CallbackCharFilter;
    }
    ImGuiInputTextCallback callback = InputTextExCallback;
    void* callbackUserData = reinterpret_cast<void*>(fieldData);

    const auto backupState = BackupState(window->DC);

    window->DC.CursorPos.x = drawRect.x;
    window->DC.CursorPos.y = drawRect.y;
    ImGui::SetNextItemWidth(drawRect.w);

    ImGui::PushID(strId.cbegin(), strId.cend());
    auto id = window->GetID(label);
    if (ImGui::InputTextEx(label, hint, buf, bufSize, sizeArg, flags, callback, callbackUserData)) {
        result = NumberFieldAction::Changed;
    }
    ImGui::PopID();

    backupState.Restore(window->DC);

    ItemSize(fullRect.Size());
    if (!ItemAdd(id, widgetRect)) {
        return result;
    }

    DrawTooltip(style);

    if (fieldData->showStepButtons) {
        StepButtonAction action = StepButtons(std::string(strId) + ".step_buttons", stepButtonsStyle);
        if (action == StepButtonAction::Up) {
            result = NumberFieldAction::StepUp;
        } else if (action == StepButtonAction::Down) {
            result = NumberFieldAction::StepDown;
        }
        auto fullWidgetRect = EndHorizontal();
        if (outWidgetRect != nullptr) {
            *outWidgetRect = fullWidgetRect;
        }
    }

    return result;
}

bool TextField(std::string_view strId, std::string& text, const FieldStyle& style, math::RectF* outWidgetRect) {
    auto fieldData = FieldData();
    fieldData.text = &text;

    return TextFieldImpl(strId, &fieldData, &style, outWidgetRect) != NumberFieldAction::None;
}

NumberFieldAction NumberField(std::string_view strId, std::string& text, const NumberFieldStyle& style, math::RectF* outWidgetRect) {
    auto fieldData = FieldData();
    fieldData.integerFilter = style.isInteger;
    fieldData.floatingFilter = !style.isInteger;
    fieldData.showStepButtons = style.showStepButtons;
    fieldData.text = &text;

    return TextFieldImpl(strId, &fieldData, &style, outWidgetRect);
}

} // end namespace gui
