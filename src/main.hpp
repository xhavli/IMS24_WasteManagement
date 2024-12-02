#ifndef WASTE_SIMULATION_H
#define WASTE_SIMULATION_H

#include "simlib.h"
#include <cstdio>
#include <cmath>

// **Constants**
// time constants
const int MINUTE = 1;                   // simulation step = 1 minute
const int HOUR = 60 * MINUTE;
const int DAY = 24 * HOUR;
const int YEAR = 365 * DAY;
const double WORK_START = 6 * HOUR;     // the shift starts at 6:00 AM
const double WORK_END = 16 * HOUR;      // the shift ends at 4:00 PM (10 hours)
const int TRUCKS_ARRIVAL = 5 * MINUTE;  // rate at which trucks come in 
const double HEAP_CAPACITY = 5000.0;    // the size of the garbage queue in tons

// **Debug Mode**
extern bool debug;           // Global debug flag
extern FILE *debugFile;      // Debug output file

// **Global SIMLIB Objects**
extern Store Boilers;
extern Queue WasteQueue;
extern Stat RejectedWaste;
extern Stat ProcessedWaste;

// **Class Definitions**
class Waste : public Process {
public:
    void Behavior() override;
};

class WasteGenerator : public Event {
public:
    void Behavior() override;
};



// calculates the time when does the next shift start
double startNextDay();
// calculates the time of the current day in minutes
double timeOfCurrentDay();
// true if it's still working shift hours
bool isWorkingTime(double currentTime);

#endif // WASTE_SIMULATION_H
