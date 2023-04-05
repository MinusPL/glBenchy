#include "performance_monitor.h"

#include "../../imgui/imgui.h"

double PerfMonitor::logicUpdateTime = 0.0;
double PerfMonitor::renderTime = 0.0;

void PerfMonitor::DrawUI()
{
    ImGui::Begin("Performance Monitor");

    ImGui::End();
}
