#pragma once

#include <string>

#include "Types.h"

std::string stateToString(
    State state);

std::string eventToString(
    EventType type);

State stringToState(
    const std::string& value);

EventType stringToEventType(
    const std::string& value);

std::string trim(
    const std::string& str);