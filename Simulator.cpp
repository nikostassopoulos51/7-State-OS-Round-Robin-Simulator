#include <iostream>
#include <algorithm>

#include "Simulator.h"
#include "Utils.h"

// =====================================================
// Constructor
// =====================================================

Simulator::Simulator(
    int quantum,
    int endTime)
    :
    quantum(quantum),
    endTime(endTime),
    currentTime(0)
{
}

// =====================================================
// Process Management
// =====================================================

void Simulator::addProcess(
    const std::string& pid)
{
    if(hasProcess(pid))
    {
        return;
    }

    Process p;

    p.pid = pid;
    p.state = State::NEW;

    processes[pid] = p;
}

bool Simulator::hasProcess(
    const std::string& pid) const
{
    return processes.find(pid)
        != processes.end();
}

// =====================================================
// Event Management
// =====================================================

void Simulator::addEvent(
    int time,
    const std::string& pid,
    EventType type)
{
    Event event;

    event.time = time;
    event.pid = pid;
    event.type = type;

    eventQueue.push(event);
}

void Simulator::addQuery(
    int time)
{
    queries.push_back(time);
}

// =====================================================
// Initial Configuration
// =====================================================

void Simulator::setInitialRunning(
    const std::string& pid)
{
    if(!hasProcess(pid))
    {
        addProcess(pid);
    }

    auto& process =
        processes[pid];

    process.state =
        State::RUNNING;

    process.arrivalTime = 0;

    runningProcess = pid;

    dispatchStartTime[pid] = 0;

    scheduleTimeout(pid);
}

void Simulator::addInitialReady(
    const std::string& pid)
{
    if(!hasProcess(pid))
    {
        addProcess(pid);
    }

    auto& process =
        processes[pid];

    process.state =
        State::READY;

    process.arrivalTime = 0;

    process.lastReadyTime = 0;

    readyQueue.push(pid);
}

// =====================================================
// CPU Bookkeeping
// =====================================================

void Simulator::scheduleTimeout(
    const std::string& pid)
{
    addEvent(
        currentTime + quantum,
        pid,
        EventType::TIMEOUT
    );
}

void Simulator::recordCpuBurst(
    const std::string& pid)
{
    auto it =
        dispatchStartTime.find(pid);

    if(it ==
       dispatchStartTime.end())
    {
        return;
    }

    int start =
        it->second;

    int duration =
        currentTime - start;

    processes[pid].cpuTime +=
        duration;

    TimelineEntry entry;

    entry.start = start;
    entry.end = currentTime;
    entry.pid = pid;

    cpuTimeline.push_back(
        entry);

    dispatchStartTime.erase(it);
}

// =====================================================
// DISK Bookkeeping
// =====================================================

void Simulator::startDiskIO(
    const std::string& pid)
{
    diskProcess = pid;

    diskStartTime[pid] =
        currentTime;
}

void Simulator::finishDiskIO(
    const std::string& pid)
{
    auto it =
        diskStartTime.find(pid);

    if(it ==
       diskStartTime.end())
    {
        return;
    }

    TimelineEntry entry;

    entry.start =
        it->second;

    entry.end =
        currentTime;

    entry.pid =
        pid;

    diskTimeline.push_back(
        entry);

    diskStartTime.erase(it);

    diskProcess.clear();

    if(!diskQueue.empty())
    {
        std::string next =
            diskQueue.front();

        diskQueue.pop();

        startDiskIO(next);
    }
}

// =====================================================
// Round Robin Dispatcher
// =====================================================

void Simulator::dispatch()
{
    if(!runningProcess.empty())
    {
        return;
    }

    if(readyQueue.empty())
    {
        return;
    }

    runningProcess =
        readyQueue.front();

    readyQueue.pop();

    auto& process =
        processes[runningProcess];

    if(process.lastReadyTime != -1)
    {
        process.waitingTime +=
            currentTime -
            process.lastReadyTime;
    }

    process.state =
        State::RUNNING;

    dispatchStartTime[
        runningProcess
    ] = currentTime;

    std::cout
        << "[t="
        << currentTime
        << "] "
        << runningProcess
        << " DISPATCHED\n";

    scheduleTimeout(
        runningProcess);
}

// =====================================================
// ARRIVE
// =====================================================

void Simulator::handleArrive(
    const std::string& pid)
{
    auto& process =
        processes[pid];

    process.state =
        State::READY;

    process.arrivalTime =
        currentTime;

    process.lastReadyTime =
        currentTime;

    readyQueue.push(pid);

    std::cout
        << "[t="
        << currentTime
        << "] "
        << pid
        << " ARRIVE\n";

    dispatch();
}

// =====================================================
// IO REQUEST
// =====================================================

void Simulator::handleIORequest(
    const std::string& pid)
{
    if(runningProcess != pid)
    {
        return;
    }

    recordCpuBurst(pid);

    processes[pid].state =
        State::BLOCKED;

    std::cout
        << "[t="
        << currentTime
        << "] "
        << pid
        << " IO_REQUEST\n";

    if(diskProcess.empty())
    {
        startDiskIO(pid);
    }
    else
    {
        diskQueue.push(pid);
    }

    runningProcess.clear();

    dispatch();
}

// =====================================================
// IO COMPLETE
// =====================================================

void Simulator::handleIOComplete(
    const std::string& pid)
{
    if(processes[pid].state !=
       State::BLOCKED)
    {
        return;
    }

    finishDiskIO(pid);

    processes[pid].state =
        State::READY;

    processes[pid].lastReadyTime =
        currentTime;

    readyQueue.push(pid);

    std::cout
        << "[t="
        << currentTime
        << "] "
        << pid
        << " IO_COMPLETE\n";

    dispatch();
}

// =====================================================
// TIMEOUT
// =====================================================

void Simulator::handleTimeout(
    const std::string& pid)
{
    if(runningProcess != pid)
    {
        return;
    }

    if(processes[pid].state !=
       State::RUNNING)
    {
        return;
    }

    recordCpuBurst(pid);

    processes[pid].state =
        State::READY;

    processes[pid].lastReadyTime =
        currentTime;

    readyQueue.push(pid);

    std::cout
        << "[t="
        << currentTime
        << "] "
        << pid
        << " TIMEOUT\n";

    runningProcess.clear();

    dispatch();
}

// =====================================================
// EXIT
// =====================================================

void Simulator::handleExit(
    const std::string& pid)
{
    if(runningProcess != pid)
    {
        return;
    }

    recordCpuBurst(pid);

    processes[pid].state =
        State::TERMINATED;

    processes[pid].finishTime =
        currentTime;

    std::cout
        << "[t="
        << currentTime
        << "] "
        << pid
        << " EXIT\n";

    runningProcess.clear();

    dispatch();
}

// =====================================================
// SUSPEND
// =====================================================

void Simulator::handleSuspend(
    const std::string& pid)
{
    auto& process =
        processes[pid];

    if(process.state ==
       State::READY)
    {
        process.state =
            State::READY_SUSPENDED;
    }
    else if(process.state ==
            State::BLOCKED)
    {
        process.state =
            State::BLOCKED_SUSPENDED;
    }
}

// =====================================================
// RESUME
// =====================================================

void Simulator::handleResume(
    const std::string& pid)
{
    auto& process =
        processes[pid];

    if(process.state ==
       State::READY_SUSPENDED)
    {
        process.state =
            State::READY;

        process.lastReadyTime =
            currentTime;

        readyQueue.push(pid);
    }
    else if(process.state ==
            State::BLOCKED_SUSPENDED)
    {
        process.state =
            State::BLOCKED;
    }

    dispatch();
}

// =====================================================
// Snapshot
// =====================================================

void Simulator::printSnapshot(
    int time) const
{
    std::cout
        << "\n===== QUERY t="
        << time
        << " =====\n";

    for(const auto& [pid, p]
        : processes)
    {
        std::cout
            << pid
            << "="
            << stateToString(
                   p.state)
            << '\n';
    }

    std::cout << '\n';
}

// =====================================================
// RUN
// =====================================================

void Simulator::run()
{
    std::sort(
        queries.begin(),
        queries.end());

    size_t queryIndex = 0;

    while(!eventQueue.empty())
    {
        Event event =
            eventQueue.top();

        eventQueue.pop();

        while(queryIndex <
              queries.size() &&
              queries[queryIndex]
              <= event.time)
        {
            currentTime =
                queries[queryIndex];

            printSnapshot(
                currentTime);

            ++queryIndex;
        }

        if(event.time > endTime)
        {
            break;
        }

        currentTime =
            event.time;

        switch(event.type)
        {
            case EventType::ARRIVE:
                handleArrive(event.pid);
                break;

            case EventType::IO_REQUEST:
                handleIORequest(event.pid);
                break;

            case EventType::IO_COMPLETE:
                handleIOComplete(event.pid);
                break;

            case EventType::TIMEOUT:
                handleTimeout(event.pid);
                break;

            case EventType::EXIT:
                handleExit(event.pid);
                break;

            case EventType::SUSPEND:
                handleSuspend(event.pid);
                break;

            case EventType::RESUME:
                handleResume(event.pid);
                break;
        }
    }

    while(queryIndex <
          queries.size())
    {
        currentTime =
            queries[queryIndex];

        printSnapshot(
            currentTime);

        ++queryIndex;
    }

    currentTime =
        endTime;

    std::cout
        << "\nSimulation finished at t="
        << currentTime
        << "\n";
}

// =====================================================
// CPU Timeline
// =====================================================

void Simulator::printTimeline() const
{
    std::cout
        << "\n===== CPU TIMELINE =====\n";

    for(const auto& entry
        : cpuTimeline)
    {
        std::cout
            << entry.start
            << "-"
            << entry.end
            << " : "
            << entry.pid
            << '\n';
    }
}

// =====================================================
// DISK Timeline
// =====================================================

void Simulator::printDiskTimeline() const
{
    std::cout
        << "\n===== DISK TIMELINE =====\n";

    for(const auto& entry
        : diskTimeline)
    {
        std::cout
            << entry.start
            << "-"
            << entry.end
            << " : "
            << entry.pid
            << '\n';
    }
}

// =====================================================
// Statistics
// =====================================================

void Simulator::printStatistics() const
{
    std::cout
        << "\n===== STATISTICS =====\n";

    for(const auto& [pid, p]
        : processes)
    {
        int turnaround = -1;

        if(p.finishTime != -1)
        {
            turnaround =
                p.finishTime -
                p.arrivalTime;
        }

        std::cout
            << "\nPID: "
            << pid
            << '\n';

        std::cout
            << "Arrival: "
            << p.arrivalTime
            << '\n';

        std::cout
            << "Finish: "
            << p.finishTime
            << '\n';

        std::cout
            << "CPU Time: "
            << p.cpuTime
            << '\n';

        std::cout
            << "Waiting Time: "
            << p.waitingTime
            << '\n';

        std::cout
            << "Turnaround: "
            << turnaround
            << '\n';
    }
}

// =====================================================
// Process Table
// =====================================================

void Simulator::printProcesses() const
{
    std::cout
        << "\n===== PROCESS TABLE =====\n";

    for(const auto& [pid, p]
        : processes)
    {
        std::cout
            << pid
            << " = "
            << stateToString(
                   p.state)
            << '\n';
    }
}

// =====================================================
// Ready Queue
// =====================================================

void Simulator::printReadyQueue() const
{
    std::queue<std::string> temp =
        readyQueue;

    std::cout
        << "\nREADY QUEUE: ";

    while(!temp.empty())
    {
        std::cout
            << temp.front()
            << " ";

        temp.pop();
    }

    std::cout << '\n';
}