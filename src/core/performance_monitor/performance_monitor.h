#ifndef PERFORMANCE_MONITOR_H
#define PERFORMANCE_MONITOR_H

class PerfMonitor
{
public:
    static double logicUpdateTime;
    static double renderTime;

    static void DrawUI();
};

#endif