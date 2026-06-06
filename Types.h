#pragma once

#include <string>

enum class State
{
    NEW,
    READY,
    RUNNING,
    BLOCKED,
    READY_SUSPENDED,
    BLOCKED_SUSPENDED,
    TERMINATED
};

enum class EventType
{
    ARRIVE,
    IO_REQUEST,
    IO_COMPLETE,
    TIMEOUT,
    EXIT,

    SUSPEND,
    RESUME
};

struct Process
{
    std::string pid;

    State state = State::NEW;

    int arrivalTime = -1;
    int finishTime = -1;

    int cpuTime = 0;
    int waitingTime = 0;

    int lastReadyTime = -1;
};

struct Event
{
    int time;
    std::string pid;
    EventType type;
};

struct TimelineEntry
{
    int start;
    int end;
    std::string pid;
};

inline int eventPriority(EventType type)
{
    switch(type)
    {
        case EventType::IO_COMPLETE: return 1;
        case EventType::EXIT:        return 2;
        case EventType::IO_REQUEST:  return 3;
        case EventType::TIMEOUT:     return 4;
        case EventType::ARRIVE:      return 5;
        case EventType::SUSPEND:     return 6;
        case EventType::RESUME:      return 7;
    }

    return 100;
}

struct EventCompare
{
    bool operator()(const Event& a,
                    const Event& b) const
    {
        if(a.time != b.time)
        {
            return a.time > b.time;
        }

        return eventPriority(a.type)
             > eventPriority(b.type);
    }
};