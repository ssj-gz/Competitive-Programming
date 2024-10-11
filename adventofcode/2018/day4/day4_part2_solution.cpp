#define _GLIBCXX_DEBUG       // Iterator safety; out-of-bounds access for Containers, etc.
#include <iostream>
#include <vector>
#include <map>
#include <regex>
#include <iomanip>

#include <cassert>

using namespace std;

struct GuardEvent
{
    int guardId = -1;
    int year = -1;
    int month = -1;
    int day = -1;
    int hour = -1;
    int minute = -1;
    enum EventType { StartsShift, FallsAsleep, WakesUp };
    EventType eventType;
    bool operator<(const GuardEvent& other)
    {
        if (year != other.year)
            return year < other.year;
        if (month != other.month)
            return month < other.month;
        if (day != other.day)
            return day < other.day;
        if (hour != other.hour)
            return hour < other.hour;
        if (minute != other.minute)
            return minute < other.minute;
        if (guardId != other.guardId)
            return guardId < other.guardId;
        if (eventType != other.eventType)
            return eventType < other.eventType;
        return false;
    }
};

ostream& operator<<(ostream& os, const GuardEvent& guardEvent)
{
    // [1518-11-01 00:00] Guard #10 begins shift
    os << "[" << std::setw(4) << setfill('0') << guardEvent.year << "-" << std::setw(2) << setfill('0') << guardEvent.month << "-" << std::setw(2) << setfill('0') << guardEvent.day << " " << std::setw(2) << setfill('0') << guardEvent.hour << ":" << std::setw(2) << setfill('0') << guardEvent.minute << "] ";
    switch (guardEvent.eventType)
    {
        case GuardEvent::StartsShift:
            os << "Guard #" << guardEvent.guardId << " begins shift";
            break;
        case GuardEvent::FallsAsleep:
            os << "falls asleep";
            break;
        case GuardEvent::WakesUp:
            os << "wakes up";
            break;
        default:
            assert(false);
    }
    return os;
}

int main()
{
    std::regex eventRegex(R"(^\[(\d\d\d\d)-(\d\d)-(\d\d) (\d\d):(\d\d)\]\s*((Guard #(\d+) begins shift)|(falls asleep)|(wakes up))$)");

    string eventLine;
    vector<GuardEvent> guardEvents;
    while (getline(cin, eventLine))
    {
        std::smatch eventMatch;
        std::cout << "eventLine: " << eventLine << std::endl;
        const bool matchSuccessful = std::regex_match(eventLine, eventMatch, eventRegex);
        assert(matchSuccessful);
        GuardEvent guardEvent;
        guardEvent.year = std::stoi(eventMatch[1]);
        guardEvent.month = std::stoi(eventMatch[2]);
        guardEvent.day = std::stoi(eventMatch[3]);
        guardEvent.hour = std::stoi(eventMatch[4]);
        guardEvent.minute = std::stoi(eventMatch[5]);
        if (eventMatch[6] == "falls asleep")
        {
            guardEvent.eventType = GuardEvent::FallsAsleep; 
        }
        else if (eventMatch[6] == "wakes up")
        {
            guardEvent.eventType = GuardEvent::WakesUp; 
        }
        else
        {
            guardEvent.eventType = GuardEvent::StartsShift; 
            guardEvent.guardId = std::stoi(eventMatch[8]);
        }
        guardEvents.push_back(guardEvent);
    }

    std::sort(guardEvents.begin(), guardEvents.end());
    int currentGuardId = -1;
    for (auto& guardEvent : guardEvents)
    {
        if (guardEvent.eventType == GuardEvent::StartsShift)
        {
            currentGuardId = guardEvent.guardId;
        }
        else
        {
            assert(currentGuardId != -1);
            assert(guardEvent.guardId == -1);
            guardEvent.guardId = currentGuardId;
        }
        std::cout << guardEvent << std::endl;
    }
    auto guardEventIter = guardEvents.begin();

    std::cout << "Simulating" << std::endl;
    currentGuardId = -1;
    auto shiftBeginIter = guardEvents.end();
    map<int, int> guardIdMinutesAsleep;
    map<int, map<int, int>> guardIdTimesAsleepAtMinute;
    while (guardEventIter != guardEvents.end())
    {
        std::cout << "Hopefully a shift start: " << *guardEventIter << std::endl;
        assert(guardEventIter->eventType == GuardEvent::StartsShift);
        assert(currentGuardId == -1);
        currentGuardId = guardEventIter->guardId;
        std::cout << *guardEventIter << std::endl;
        shiftBeginIter = guardEventIter;
        bool guardIsAwake = true;
        int& numMinutesAsleep = guardIdMinutesAsleep[currentGuardId];
        map<int, int>& timesAsleepAtMinute = guardIdTimesAsleepAtMinute[currentGuardId];
        guardEventIter++;

        for (int minute = 0; minute < 60; minute++)
        {
            if (guardEventIter != guardEvents.end() && guardEventIter->eventType != GuardEvent::StartsShift)
            {
                if (guardEventIter->minute == minute)
                {
                    std::cout << " processing shift event: " << *guardEventIter << std::endl;
                    if (guardEventIter->eventType == GuardEvent::WakesUp)
                    {
                        guardIsAwake = true;
                    }
                    else if (guardEventIter->eventType == GuardEvent::FallsAsleep)
                    {
                        guardIsAwake = false;
                    }
                    guardEventIter++;

                }

            }
            std::cout << " minute: " << minute << " guard " << currentGuardId << " is " << (guardIsAwake ? "awake" : "asleep") << std::endl;
            if (!guardIsAwake)
            {
                numMinutesAsleep++;
                timesAsleepAtMinute[minute]++;
            }
        }
        currentGuardId = -1;
        shiftBeginIter = guardEvents.end();
    }
    int sleepiestGuardId = -1;
    int sleepiestMinute = -1;
    int highestTimesAsleepAtMinute = -1;
    for (const auto& [guardId, timesAsleepAtMinute] : guardIdTimesAsleepAtMinute)
    {
        for (const auto& [minute, timesAsleep] : timesAsleepAtMinute)
        {
            if (timesAsleep > highestTimesAsleepAtMinute)
            {
                highestTimesAsleepAtMinute = timesAsleep;
                sleepiestGuardId = guardId;
                sleepiestMinute = minute;
            }
        }
    }

    std::cout << "sleepiestGuardId: " << sleepiestGuardId << std::endl;
    std::cout << "sleepiestMinute: " << sleepiestMinute << std::endl;
    std::cout << "highestTimesAsleepAtMinute: " << highestTimesAsleepAtMinute << std::endl;
    std::cout << "result: " << (sleepiestGuardId * sleepiestMinute) << std::endl;
    return 0;
}
