#pragma once

#include <string>
#include <vector>

#include "Types.h"

struct SimulationInput
{
    int quantum = 10;

    int endTime = 0;

    std::string initialRunning;

    std::vector<std::string>
        initialReady;

    std::vector<int>
        queries;

    std::vector<Event>
        events;
};

class InputParser
{
public:

    static SimulationInput parse(
        const std::string& filename);

private:

    static std::vector<std::string>
    split(
        const std::string& text,
        char delimiter);
};