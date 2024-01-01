#define IMGUI_DEFINE_MATH_OPERATORS
#include "hello_imgui/internal/borderless_movable.h"
#include "hello_imgui/hello_imgui.h"
#include "imgui_internal.h"


namespace HelloImGui
{
    void HandleBorderlessMovable(
        BackendApi::WindowPointer window,
        BackendApi::IBackendWindowHelper * backendWindowHelper,
        bool borderlessMovable,
        bool borderlessResizable,
        ImVec4 borderlessHighlightColor)
    {
        ImU32 highlightColorU32 = ImGui::GetColorU32(borderlessHighlightColor);
        if (borderlessHighlightColor.w == 0.f)
            highlightColorU32 = ImGui::GetColorU32(ImGuiCol_TitleBg, 0.6f);

        // Drag zone at the top of the window
        if (borderlessMovable)
        {
            ImVec2 dragSize(ImGui::GetMainViewport()->Size.x, ImGui::GetFontSize() * 1.5f);
            ImRect dragArea(ImGui::GetMainViewport()->Pos, ImGui::GetMainViewport()->Pos + dragSize);

            // Update dragging state
            ImVec2 mousePos = ImGui::GetMousePos();
            static bool isDragging = false;
            if (dragArea.Contains(mousePos) && !isDragging && ImGui::IsMouseDown(ImGuiMouseButton_Left))
            {
                isDragging = true;
            }
            if (!ImGui::IsMouseDown(ImGuiMouseButton_Left) && isDragging)
                isDragging = false;

            // Draw drag zone
            if (dragArea.Contains(mousePos) || isDragging)
            {
                ImGui::GetForegroundDrawList()->AddRectFilled(
                    dragArea.Min,
                    dragArea.Max,
                    highlightColorU32
                );
            }

            // Move window if dragging
            if (isDragging)
            {
                ImVec2 dragDelta = ImGui::GetMouseDragDelta(0);
                ImGui::ResetMouseDragDelta(0);
                auto windowBounds = backendWindowHelper->GetWindowBounds(window);
                windowBounds.position[0] += (int)dragDelta.x;
                windowBounds.position[1] += (int)dragDelta.y;
                backendWindowHelper->SetWindowBounds(window, windowBounds);
                ImGui::ResetMouseDragDelta(ImGuiMouseButton_Left);
            }

            // Set mouse cursor: probably not visible for moving (the cursor will be the classic arrow)
            if (dragArea.Contains(mousePos) || isDragging)
                ImGui::SetMouseCursor(ImGuiMouseCursor_Arrow);
        }

        // Drag zone at the bottom-right of the window
        if (borderlessResizable)
        {
            ImVec2 bottomRightCorner = ImGui::GetMainViewport()->Pos + ImGui::GetMainViewport()->Size;
            ImVec2 topLeftCorner = bottomRightCorner - HelloImGui::EmToVec2(1.5f, 1.5f);
            ImRect dragArea = ImRect(topLeftCorner, bottomRightCorner);

            // Update dragging state
            static bool isDragging = false;
            ImVec2 mousePos = ImGui::GetIO().MousePos;
            if (dragArea.Contains(mousePos) && !isDragging && ImGui::IsMouseDown(ImGuiMouseButton_Left))
                isDragging = true;
            if (!ImGui::IsMouseDown(ImGuiMouseButton_Left))
                isDragging = false;

            // Draw drag zone
            if (dragArea.Contains(mousePos) || isDragging)
            {
                // Small triangle in the bottom-right corner
                ImVec2 t1 = bottomRightCorner;
                ImVec2 t2 = bottomRightCorner - HelloImGui::EmToVec2(1.5f, 0.f);
                ImVec2 t3 = bottomRightCorner - HelloImGui::EmToVec2(0.f, 1.5f);
                ImGui::GetForegroundDrawList()->AddTriangleFilled(
                    t1, t2, t3,
                    highlightColorU32
                );
            }

            // Resize window if dragging
            if (isDragging)
            {
                ImVec2 dragDelta = ImGui::GetMouseDragDelta(0);
                ImGui::ResetMouseDragDelta(0);
                auto windowBounds = backendWindowHelper->GetWindowBounds(window);
                windowBounds.size[0] += (int)dragDelta.x;
                windowBounds.size[1] += (int)dragDelta.y;
                backendWindowHelper->SetWindowBounds(window, windowBounds);
                ImGui::ResetMouseDragDelta(ImGuiMouseButton_Left);
            }

            // Set mouse cursor: (not visible under glfw < 3.4, which does not implement this cursor)
            if (dragArea.Contains(mousePos) || isDragging)
                ImGui::SetMouseCursor(ImGuiMouseCursor_ResizeNWSE);
        }
    }
}

