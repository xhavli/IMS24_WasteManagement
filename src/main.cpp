#include "main.hpp"

// **Debug Mode**
bool debug = false;            // Set to false to disable debugging
FILE *debugFile = nullptr;    // Debug output file

// **Global SIMLIB Objects**
Store Boilers("Boilers", 2);
Queue WasteQueue("WasteQueue");
Stat RejectedWaste("RejectedWaste");
Stat ProcessedWaste("ProcessedWaste");


// calculates the time when does the next shift start
double startNextDay(){
    return (floor(Time / DAY) + 1) * DAY + WORK_START;
}

// calculates the time of the current day in minutes
double timeOfCurrentDay(){
    return fmod(Time, DAY);
}

// true if it's still working shift hours
bool isWorkingTime(double currentTime){
    return currentTime >= WORK_START && currentTime < WORK_END;
}


// Waste behavior implementation
void Waste::Behavior() {
    if (Boilers.Full()) {
        if (WasteQueue.Length() < HEAP_CAPACITY) {
            Into(WasteQueue);
            Passivate();
        } else {
            RejectedWaste(1);
            if (debug) {
                fprintf(debugFile, "Waste rejected at time: %f\n", Time);
            }
            return;
        }
    }

    Enter(Boilers, 1);
    ProcessedWaste(1);
    Wait(HOUR / Uniform(14, 16));
    Leave(Boilers, 1);

    if (!WasteQueue.Empty()) {
        Waste *nextWaste = static_cast<Waste *>(WasteQueue.GetFirst());
        nextWaste->Activate();
    }
}

// WasteGenerator behavior implementation
void WasteGenerator::Behavior() {
    double nextArrival;
    double currentTime = timeOfCurrentDay();

    if (isWorkingTime(currentTime)) {
        int numWastes = Uniform(7, 16);
        for (int i = 0; i < numWastes; ++i) {
            (new Waste())->Activate();
        }
        nextArrival = Time + Exponential(TRUCKS_ARRIVAL);

        if (debug) {
            fprintf(debugFile, "Generated %d wastes at time: %f hours\n", numWastes, currentTime / HOUR);
        }
    } else {
        nextArrival = startNextDay();

        if (debug) {
            fprintf(debugFile, "End of day %.0f\n", floor(Time / DAY) + 1);
        }
    }

    Activate(nextArrival);
}


// Main function
int main() {
    if (debug) {
        debugFile = fopen("debug.out", "w");
        if (debugFile == NULL) {
            fprintf(stderr, "Failed to open debug file\n");
            return EXIT_FAILURE;
        }
    }

    SetOutput("results.out");
    Init(0, YEAR);

    (new WasteGenerator)->Activate(WORK_START);
    Run();

    Boilers.Output();
    WasteQueue.Output();
    RejectedWaste.Output();
    ProcessedWaste.Output();
    SIMLIB_statistics.Output();

    if (debug) {
        fclose(debugFile);
    }
    return 0;
}
