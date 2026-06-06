#pragma once

#include <queue>
#include <vector>
#include <string>
#include <unordered_map>

#include "Types.h"

class Simulator
{
private:

    int quantum;
    int endTime;
    int currentTime;

    std::unordered_map<
        std::string,
        Process
    > processes;

    std::priority_queue<
        Event,
        std::vector<Event>,
        EventCompare
    > eventQueue;

    std::queue<std::string> readyQueue;

    std::string runningProcess;

    std::string diskProcess;

    std::queue<std::string> diskQueue;

    std::vector<int> queries;

    std::vector<TimelineEntry> cpuTimeline;

    std::vector<TimelineEntry> diskTimeline;

    std::unordered_map<
        std::string,
        int
    > dispatchStartTime;

    std::unordered_map<
        std::string,
        int
    > diskStartTime;

private:

    void dispatch();

    void scheduleTimeout(
        const std::string& pid);

    void recordCpuBurst(
        const std::string& pid);

    void startDiskIO(
        const std::string& pid);

    void finishDiskIO(
        const std::string& pid);

    void printSnapshot(
        int time) const;

public:

    Simulator(
        int quantum,
        int endTime);

    void addProcess(
        const std::string& pid);

    bool hasProcess(
        const std::string& pid) const;

    void addEvent(
        int time,
        const std::string& pid,
        EventType type);

    void addQuery(
        int time);

    void setInitialRunning(
        const std::string& pid);

    void addInitialReady(
        const std::string& pid);

    void handleArrive(
        const std::string& pid);

    void handleIORequest(
        const std::string& pid);

    void handleIOComplete(
        const std::string& pid);

    void handleTimeout(
        const std::string& pid);

    void handleExit(
        const std::string& pid);

    void handleSuspend(
        const std::string& pid);

    void handleResume(
        const std::string& pid);

    void run();

    void printTimeline() const;

    void printDiskTimeline() const;

    void printStatistics() const;

    void printProcesses() const;

    void printReadyQueue() const;
};