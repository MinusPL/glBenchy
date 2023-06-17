#include "performance_monitor.h"

#include "../../imgui/imgui.h"
#include "../../miniz/miniz.hpp"
#include <cstdio>
#include <iostream>

#ifdef EMSCRIPTEN
#include<emscripten/emscripten.h>
#endif

double PerfMonitor::logicUpdateTime = 0.0;
double PerfMonitor::renderTime = 0.0;
double PerfMonitor::UITime = 0.0;
double PerfMonitor::glfwTime = 0.0;
double PerfMonitor::frameTime = 0.0;

std::string PerfMonitor::currentTestName = "N/A";
std::map<std::string, TestResult> PerfMonitor::tests;

void PerfMonitor::beginTest(std::string name)
{
    currentTestName = name;
    tests[name] = TestResult();
}

void PerfMonitor::createSnapShot()
{
    TestPoint p = {logicUpdateTime, renderTime, glfwTime, UITime, frameTime};
    tests[currentTestName].data.push_back(p);
}

void PerfMonitor::DrawUI()
{
    ImGui::SetNextWindowPos(ImVec2(0,0));
    ImGui::SetNextWindowSize(ImVec2(400, 400));
    ImGui::Begin("Performance Monitor", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoMove);
    ImGui::Text("Test Name: %s", currentTestName.c_str());
    ImGui::Text("Logic Update Time: %.4f ms", logicUpdateTime*1000.0);
    ImGui::Text("Draw calls Time: %.4f ms", renderTime*1000.0);
    ImGui::Text("UI Time: %.4f ms", UITime*1000.0);
    ImGui::Text("EventLoop & Swap buffer Time: %.6f ms", glfwTime*1000.0);
    ImGui::Text("Frame Time: %.4f ms (%.2f FPS)", frameTime*1000.0, 1.0/frameTime);
    ImGui::End();
}

void PerfMonitor::saveTestResults()
{
    miniz_cpp::zip_file zfile;
    for(auto testresult : tests)
    {
        std::string path = testresult.first + ".csv";

        FILE* ftr = fopen(path.c_str(), "w");
        std::string dataLine = "Snapshot Number;Frame Time;Logic Update Time;Draw calls Time;UI Time;Swap and Event time\n";
        fwrite(dataLine.c_str(), sizeof(char), dataLine.length(), ftr);
        char data[512];
        for(int i = 0; i < testresult.second.data.size(); i++)
        {

            sprintf(data, "%d;%.4f;%.4f;%.4f;%.4f;%.4f\n",i+1, testresult.second.data[i].frameTime * 1000.0,
                                                                             testresult.second.data[i].logicUpdateTime * 1000.0,
                                                                             testresult.second.data[i].renderTime * 1000.0,
                                                                             testresult.second.data[i].uiTime * 1000.0,
                                                                             testresult.second.data[i].swapeventTime * 1000.0);
            std::string dataLine = data;
            fwrite(dataLine.c_str(), sizeof(char), dataLine.length(), ftr); 
        }
        fclose(ftr);
        zfile.write(path);
    }
    zfile.save("results.zip");

    //ZIP IT
    #ifdef EMSCRIPTEN
    std::string name = "saveFileFromMemoryFSToDisk('results.zip', 'results_WEB.zip')";
    emscripten_run_script(name.c_str());
    #endif
}
