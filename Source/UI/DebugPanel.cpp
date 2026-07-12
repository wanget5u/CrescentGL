#include "UI/DebugPanel.h"

#include "imgui.h"
#include "imgui_internal.h"
#include "Core/Application.h"
#include "Core/Time.h"
#include "Render/RenderStats.h"
#include "Scene/Scene.h"

namespace Crescent::UI {

DebugPanel::DebugPanel(const std::string_view name) : Panel(name) {}

void DebugPanel::OnUpdate(const f32 deltaTime) {
    m_Timer += deltaTime;
    m_FrameCount++;

    RenderStats& stats = RenderStats::Instance();
    m_AccumulatedCpuTimeMs += stats.CPUFrameTimeMs;
    m_AccumulatedGpuTimeMs += stats.GPUFrameTimeMs;

    if (m_Timer >= MetricsUpdateRate) {
        const f32 avgFactor = 1.0f / static_cast<f32>(m_FrameCount);
        m_CachedFPS           = static_cast<f32>(m_FrameCount) / m_Timer;
        m_CachedCpuTimeMs     = m_AccumulatedCpuTimeMs * avgFactor;
        m_CachedGpuTimeMs     = m_AccumulatedGpuTimeMs * avgFactor;
        m_CachedDrawCalls     = stats.DrawCalls;
        m_CachedTriangleCount = stats.TriangleCount;
        m_CachedVertexCount   = stats.VertexCount;

        Scene* activeScene = Application::Instance().GetActiveScene();
        m_CachedSceneName      = activeScene->GetName();
        m_CachedCameraSpeed    = activeScene->GetCameraSpeed();
        m_CachedCameraPosition = activeScene->GetCameraPosition();
        m_CachedCameraZoom     = activeScene->GetCameraZoom();

        if (activeScene->IsLit()) {
            m_SceneState = "Lit";
        }
        else if (activeScene->IsUnlit()) {
            m_SceneState = "Unlit";
        }
        else if (activeScene->IsWireframe()) {
            m_SceneState = "Wireframe";
        }
        else {
            m_SceneState = "Unknown";
        }

        m_AccumulatedCpuTimeMs = 0.0f;
        m_AccumulatedGpuTimeMs = 0.0f;
        m_Timer = 0.0f;
        m_FrameCount = 0;
    }
}

void DebugPanel::OnRender() {
    if (m_IsVisible == false) {
        return;
    }
    ImGuiViewport* viewport = ImGui::GetMainViewport();
    const f32 resolutionScale = GetResolutionScale();
    const f32 fontHeight = ImGui::GetTextLineHeight() * resolutionScale;
    const f32 verticalPadding = 4.0f * resolutionScale;
    const f32 dynamicPanelHeight = fontHeight + (verticalPadding * 2.0f);

    ImVec2 windowPos(viewport->WorkPos.x, viewport->WorkPos.y + viewport->WorkSize.y - dynamicPanelHeight);
    ImVec2 windowSize(viewport->WorkSize.x, dynamicPanelHeight);

    ImGui::SetNextWindowPos(windowPos, ImGuiCond_Always);
    ImGui::SetNextWindowSize(windowSize, ImGuiCond_Always);
    ImGui::SetNextWindowViewport(viewport->ID);

    constexpr ImGuiWindowFlags windowFlags = ImGuiWindowFlags_NoTitleBar |
                                             ImGuiWindowFlags_NoResize |
                                             ImGuiWindowFlags_NoMove |
                                             ImGuiWindowFlags_NoScrollbar |
                                             ImGuiWindowFlags_NoSavedSettings |
                                             ImGuiWindowFlags_NoDocking |
                                             ImGuiWindowFlags_NoFocusOnAppearing;

    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(12.0f * resolutionScale, verticalPadding));
    ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.08f, 0.08f, 0.08f, 1.0f));

    if (ImGui::Begin(m_Name.c_str(), &m_IsVisible, windowFlags)) {
        BeginScaledFont();

        SegmentLayoutContext context{};
        context.ResolutionScale  = resolutionScale;
        context.SeparatorSpacing = 12.0f * resolutionScale;
        context.SeparatorWidth = 1.0f * resolutionScale;
        context.CurrentX = ImGui::GetCursorPosX();
        context.MaxX = context.CurrentX + ImGui::GetContentRegionAvail().x;
        context.HasSpace = true;

        RenderLeftSection(context);
        const f32 leftEndPosX = context.CurrentX;

        f32 rightStartPosX = context.MaxX;
        RenderRightSection(context, leftEndPosX, rightStartPosX);
        RenderCenterSection(context, leftEndPosX, rightStartPosX);

        EndScaledFont();
    }
    ImGui::End();
    ImGui::PopStyleColor();
    ImGui::PopStyleVar(3);
}

void DebugPanel::OnAttach() {

}

void DebugPanel::OnDetach() {

}

void DebugPanel::RenderLeftSection(SegmentLayoutContext& context) const {
    const ImVec4 colorDefault = ImGui::GetStyleColorVec4(ImGuiCol_Text);
    constexpr ImVec4 colorGreen = ImVec4(0.4f, 1.0f, 0.4f, 1.0f);
    char buf[64];
    snprintf(buf, sizeof(buf), "FPS: %.1f", m_CachedFPS);
    DrawLeftItem(context, 100.0f, true, colorGreen, buf);
    snprintf(buf, sizeof(buf), "CPU: %.2f ms", m_CachedCpuTimeMs);
    DrawLeftItem(context, 115.0f, false, colorDefault, buf);
    snprintf(buf, sizeof(buf), "GPU: %.2f ms", m_CachedGpuTimeMs);
    DrawLeftItem(context, 115.0f, false, colorDefault, buf);
    snprintf(buf, sizeof(buf), "Draw: %u", m_CachedDrawCalls);
    DrawLeftItem(context, 85.0f, false, colorDefault, buf);
    snprintf(buf, sizeof(buf), "Tris: %u", m_CachedTriangleCount);
    DrawLeftItem(context, 85.0f, false, colorDefault, buf);
    snprintf(buf, sizeof(buf), "Vert: %u", m_CachedVertexCount);
    DrawLeftItem(context, 85.0f, false, colorDefault, buf);
}

void DebugPanel::DrawLeftItem(SegmentLayoutContext& context, const f32 minWidthUnscaled, const bool isFirst, const ImVec4& color, const char* text) const {
    if (context.HasSpace == false) {
        return;
    }
    const f32 textWidth = ImGui::CalcTextSize(text).x;
    const f32 segmentWidth = Math::Max(textWidth, minWidthUnscaled * context.ResolutionScale);
    f32 requiredSpace = segmentWidth;
    if (isFirst == false) {
        requiredSpace += (context.SeparatorSpacing * 2.0f) + context.SeparatorWidth;
    }
    if (context.CurrentX + requiredSpace > context.MaxX) {
        context.HasSpace = false;
        return;
    }
    if (isFirst == false) {
        ImGui::SameLine();
        ImGui::SetCursorPosX(context.CurrentX + context.SeparatorSpacing);
        ImGui::SeparatorEx(ImGuiSeparatorFlags_Vertical);
        context.CurrentX += (context.SeparatorSpacing * 2.0f) + context.SeparatorWidth;
        ImGui::SameLine();
    }
    ImGui::SetCursorPosX(context.CurrentX);
    ImGui::TextColored(color, "%s", text);
    context.CurrentX += segmentWidth;
}

void DebugPanel::RenderRightSection(const SegmentLayoutContext& context, const f32 leftEndPosX, f32& outRightStartPosX) const {
    const ImVec4 colorDefault = ImGui::GetStyleColorVec4(ImGuiCol_Text);
    constexpr ImVec4 colorGreen = ImVec4(0.4f, 1.0f, 0.4f, 1.0f);
    constexpr ImVec4 colorRed = ImVec4(1.0f, 0.4f, 0.4f, 1.0f);
    constexpr ImVec4 colorBlue = ImVec4(0.4f, 0.4f, 1.0f, 1.0f);

    constexpr u8 itemCount = 6;
    RightSegmentItem items[itemCount] = {
        { "", colorDefault,  75.0f, 0.0f, 0.0f },
        { "", colorDefault,  80.0f, 0.0f, 0.0f },
        { "", colorDefault,  35.0f, 0.0f, 0.0f },
        { "", colorRed,      70.0f, 0.0f, 0.0f },
        { "", colorGreen,    70.0f, 0.0f, 0.0f },
        { "", colorBlue,     70.0f, 0.0f, 0.0f }
    };
    snprintf(items[0].Text, sizeof(items[0].Text), "%s", m_SceneState.c_str());
    snprintf(items[1].Text, sizeof(items[1].Text), "Speed: %.2f", m_CachedCameraSpeed);
    snprintf(items[2].Text, sizeof(items[2].Text), "x%.2f", m_CachedCameraZoom);
    snprintf(items[3].Text, sizeof(items[3].Text), "X: %.2f", m_CachedCameraPosition.x);
    snprintf(items[4].Text, sizeof(items[4].Text), "Y: %.2f", m_CachedCameraPosition.y);
    snprintf(items[5].Text, sizeof(items[5].Text), "Z: %.2f", m_CachedCameraPosition.z);

    for (RightSegmentItem& item : items) {
        item.TextWidth = ImGui::CalcTextSize(item.Text).x;
        item.ActualWidth = Math::Max(item.TextWidth, item.MinWidthUnscaled * context.ResolutionScale);
    }

    u8 firstVisibleIndex = itemCount;
    for (u8 a = 0; a < itemCount; ++a) {
        const f32 width = CalculateRightGroupWidth(items, itemCount, a, context.SeparatorSpacing, context.SeparatorWidth);
        const f32 startX = ImGui::GetWindowContentRegionMax().x - width;
        if (startX >= leftEndPosX + context.SeparatorSpacing) {
            firstVisibleIndex = a;
            break;
        }
    }
    f32 rightCurrentX{0.0f};
    if (firstVisibleIndex < itemCount) {
        rightCurrentX = ImGui::GetWindowContentRegionMax().x -
            CalculateRightGroupWidth(items, itemCount, firstVisibleIndex,
                context.SeparatorSpacing, context.SeparatorWidth
            );
    }
    else {
        rightCurrentX = ImGui::GetWindowContentRegionMax().x;
    }
    outRightStartPosX = rightCurrentX;

    if (firstVisibleIndex < itemCount) {
        f32 slotStartX = rightCurrentX;
        for (u8 a = firstVisibleIndex; a < itemCount; ++a) {
            if (a > firstVisibleIndex) {
                ImGui::SameLine();
                ImGui::SetCursorPosX(slotStartX + context.SeparatorSpacing);
                ImGui::SeparatorEx(ImGuiSeparatorFlags_Vertical);
                slotStartX += (context.SeparatorSpacing * 2.0f) + context.SeparatorWidth;
            }
            ImGui::SameLine();
            const f32 textPosX = slotStartX + (items[a].ActualWidth - items[a].TextWidth);
            ImGui::SetCursorPosX(textPosX);
            ImGui::TextColored(items[a].Color, "%s", items[a].Text);
            slotStartX += items[a].ActualWidth;
        }
    }
}

f32 DebugPanel::CalculateRightGroupWidth(const RightSegmentItem* items, const u8 itemCount, const u8 startIdx, const f32 separatorSpacing, const f32 separatorWidth) const {
    if (startIdx >= itemCount) {
        return 0.0f;
    }
    f32 width = 0.0f;
    for (u8 a = startIdx; a < itemCount; ++a) {
        width += items[a].ActualWidth;
    }
    if (startIdx < itemCount - 1) {
        width += (itemCount - 1 - startIdx) * ((separatorSpacing * 2.0f) + separatorWidth);
    }
    return width;
}

void DebugPanel::RenderCenterSection(const SegmentLayoutContext& context, const f32 leftEndPosX, const f32 rightStartPosX) const {
    if (m_CachedSceneName.empty() == true) {
        return;
    }
    const f32 sceneNameWidth = ImGui::CalcTextSize(m_CachedSceneName.c_str()).x;
    const f32 centerStartX = (ImGui::GetWindowContentRegionMax().x + ImGui::GetWindowContentRegionMin().x) * 0.5f - (sceneNameWidth * 0.5f);
    const bool canDrawCenter = (centerStartX - (context.SeparatorSpacing * 2.0f) - context.SeparatorWidth >= leftEndPosX) &&
                               (centerStartX + sceneNameWidth + (context.SeparatorSpacing * 2.0f) + context.SeparatorWidth <= rightStartPosX);

    if (canDrawCenter ==  true) {
        ImGui::SameLine();
        ImGui::SetCursorPosX(centerStartX - context.SeparatorSpacing - context.SeparatorWidth);
        ImGui::SeparatorEx(ImGuiSeparatorFlags_Vertical);

        ImGui::SameLine();
        ImGui::SetCursorPosX(centerStartX);
        ImGui::TextUnformatted(m_CachedSceneName.c_str());

        ImGui::SameLine();
        ImGui::SetCursorPosX(centerStartX + sceneNameWidth + context.SeparatorSpacing);
        ImGui::SeparatorEx(ImGuiSeparatorFlags_Vertical);
    }
}

}