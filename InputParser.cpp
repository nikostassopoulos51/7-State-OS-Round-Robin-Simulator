#include "InputParser.h"
#include "Utils.h"

#include <fstream>
#include <sstream>
#include <stdexcept>

std::vector<std::string>
InputParser::split(
    const std::string& text,
    char delimiter)
{
    std::vector<std::string> result;

    std::stringstream ss(text);

    std::string token;

    while(std::getline(
        ss,
        token,
        delimiter))
    {
        result.push_back(
            trim(token));
    }

    return result;
}

SimulationInput
InputParser::parse(
    const std::string& filename)
{
    SimulationInput data;

    std::ifstream file(filename);

    if(!file.is_open())
    {
        throw std::runtime_error(
            "Cannot open input file: "
            + filename);
    }

    std::string line;

    while(std::getline(file, line))
    {
        line = trim(line);

        if(line.empty())
        {
            continue;
        }

        //------------------------------------------------
        // Quantum
        //------------------------------------------------

        if(line.find("Quantum") == 0)
        {
            auto pos = line.find('=');

            data.quantum =
                std::stoi(
                    trim(
                        line.substr(
                            pos + 1)));
            continue;
        }

        //------------------------------------------------
        // end_time
        //------------------------------------------------

        if(line.find("end_time") == 0)
        {
            auto pos = line.find('=');

            data.endTime =
                std::stoi(
                    trim(
                        line.substr(
                            pos + 1)));
            continue;
        }

        //------------------------------------------------
        // initial_running
        //------------------------------------------------

        if(line.find("initial_running") == 0)
        {
            auto pos = line.find('=');

            data.initialRunning =
                trim(
                    line.substr(
                        pos + 1));

            continue;
        }

        //------------------------------------------------
        // initial_ready
        //------------------------------------------------

        if(line.find("initial_ready") == 0)
        {
            auto pos = line.find('=');

            auto values =
                split(
                    line.substr(
                        pos + 1),
                    ',');

            data.initialReady =
                values;

            continue;
        }

        //------------------------------------------------
        // queries
        //------------------------------------------------

        if(line.find("queries") == 0)
        {
            auto pos = line.find('=');

            auto values =
                split(
                    line.substr(
                        pos + 1),
                    ',');

            for(const auto& v : values)
            {
                data.queries.push_back(
                    std::stoi(v));
            }

            continue;
        }

        //------------------------------------------------
        // Events
        //------------------------------------------------

        if(std::isdigit(line[0]))
        {
            auto parts =
                split(line, ',');

            if(parts.size() != 3)
            {
                continue;
            }

            Event event;

            event.time =
                std::stoi(parts[0]);

            event.pid =
                parts[1];

            event.type =
                stringToEventType(
                    parts[2]);

            data.events.push_back(
                event);
        }
    }

    return data;
}