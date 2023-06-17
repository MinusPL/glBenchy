#ifndef PERFORMANCE_MONITOR_H
#define PERFORMANCE_MONITOR_H

#include <string>
#include <vector>
#include <map>

struct TestPoint
{
    double logicUpdateTime;
    double renderTime;
    double swapeventTime;
    double uiTime;
    double frameTime;
};

struct TestResult
{
    std::string testName;
    std::vector<TestPoint> data;
};

class PerfMonitor
{
public:
    static double logicUpdateTime;
    static double renderTime;
    static double UITime;
    static double glfwTime;
    static double frameTime;
    static std::string currentTestName;
    static std::map<std::string, TestResult> tests;

    static void clearPerformanceData();

    static void beginTest(std::string name);

    static void createSnapShot();

    static void DrawUI();

    static void saveTestResults();
};

#endif