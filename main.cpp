#include <iostream>

#include "Types.h"
#include "Utils.h"
#include "InputParser.h"
#include "Simulator.h"
#include "Utils.cpp"
#include "InputParser.cpp"
#include "Simulator.cpp"


int main()
{
    try
    {
        SimulationInput input =
            InputParser::parse(
                "input.txt");

        Simulator sim(
            input.quantum,
            input.endTime);

        //------------------------------------------------
        // Initial Running Process
        //------------------------------------------------

        if(!input.initialRunning.empty())
        {
            sim.addProcess(
                input.initialRunning);

            sim.setInitialRunning(
                input.initialRunning);
        }

        //------------------------------------------------
        // Initial Ready Processes
        //------------------------------------------------

        for(const auto& pid
            : input.initialReady)
        {
            sim.addProcess(pid);

            sim.addInitialReady(pid);
        }

        //------------------------------------------------
        // Events
        //------------------------------------------------

        for(const auto& event
            : input.events)
        {
            if(!sim.hasProcess(
                event.pid))
            {
                sim.addProcess(
                    event.pid);
            }

            sim.addEvent(
                event.time,
                event.pid,
                event.type);
        }

        //------------------------------------------------
        // Queries
        //------------------------------------------------

        for(int query
            : input.queries)
        {
            sim.addQuery(query);
        }

        //------------------------------------------------
        // Run Simulation
        //------------------------------------------------

        sim.run();

        //------------------------------------------------
        // Output
        //------------------------------------------------

        sim.printTimeline();

        sim.printDiskTimeline();

        sim.printStatistics();

        sim.printProcesses();

        return 0;
    }
    catch(const std::exception& ex)
    {
        std::cerr
            << "Error: "
            << ex.what()
            << '\n';

        return 1;
    }
}

