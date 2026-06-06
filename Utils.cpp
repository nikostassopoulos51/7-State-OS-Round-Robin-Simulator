#include "Utils.h"

#include <algorithm>
#include <cctype>

std::string stateToString(State state)
{
    switch(state)
    {
        case State::NEW:
            return "NEW";

        case State::READY:
            return "READY";

        case State::RUNNING:
            return "RUNNING";

        case State::BLOCKED:
            return "BLOCKED";

        case State::READY_SUSPENDED:
            return "READY_SUSPENDED";

        case State::BLOCKED_SUSPENDED:
            return "BLOCKED_SUSPENDED";

        case State::TERMINATED:
            return "TERMINATED";
    }

    return "UNKNOWN";
}

std::string eventToString(EventType type)
{
    switch(type)
    {
        case EventType::ARRIVE:
            return "ARRIVE";

        case EventType::IO_REQUEST:
            return "IO_REQUEST";

        case EventType::IO_COMPLETE:
            return "IO_COMPLETE";

        case EventType::TIMEOUT:
            return "TIMEOUT";

        case EventType::EXIT:
            return "EXIT";

        case EventType::SUSPEND:
            return "SUSPEND";

        case EventType::RESUME:
            return "RESUME";
    }

    return "UNKNOWN";
}

State stringToState(
    const std::string& value)
{
    if(value == "NEW")
        return State::NEW;

    if(value == "READY")
        return State::READY;

    if(value == "RUNNING")
        return State::RUNNING;

    if(value == "BLOCKED")
        return State::BLOCKED;

    if(value == "READY_SUSPENDED")
        return State::READY_SUSPENDED;

    if(value == "BLOCKED_SUSPENDED")
        return State::BLOCKED_SUSPENDED;

    return State::TERMINATED;
}

EventType stringToEventType(
    const std::string& value)
{
    if(value == "ARRIVE")
        return EventType::ARRIVE;

    if(value == "IO_REQUEST")
        return EventType::IO_REQUEST;

    if(value == "IO_COMPLETE")
        return EventType::IO_COMPLETE;

    if(value == "TIMEOUT")
        return EventType::TIMEOUT;

    if(value == "EXIT")
        return EventType::EXIT;

    if(value == "SUSPEND")
        return EventType::SUSPEND;

    if(value == "RESUME")
        return EventType::RESUME;

    return EventType::ARRIVE;
}

std::string trim(
    const std::string& str)
{
    size_t start = 0;

    while(start < str.size() &&
          std::isspace(
            static_cast<unsigned char>(
                str[start])))
    {
        ++start;
    }

    size_t end = str.size();

    while(end > start &&
          std::isspace(
            static_cast<unsigned char>(
                str[end - 1])))
    {
        --end;
    }

    return str.substr(
        start,
        end - start);
}