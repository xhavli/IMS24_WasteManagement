#include "simlib.h" // Include the SIMLIB library
#include <math.h>
#include <time.h>

// Constants
const int MINUTE = 1;       // Simulation step equals one minute
const int HOUR = 60 * MINUTE;            // One hour in minutes
const int DAY = 24 * HOUR;               // One day in minutes
const int YEAR = 365 * DAY;              // One year in minutes
const double WORK_START = 6 * HOUR;      // Start of working hours (6:00 AM in minutes)
const double WORK_END = 16 * HOUR;       // End of working hours (2:00 PM in minutes)

const int TRUCKS_ARRIVAL = 7 * MINUTE;  // Mean arrival time for trucks
//const double BOILER_CAPACITY_PER_MINUTE = 15.0 / HOUR; // 15 tons/hour per boiler, converted to tons per minute

const double HEAP_CAPACITY = 5000.0;      // 200 tons queue capacity
FILE *outputFile = fopen("debug.out", "w");

// Global SIMLIB objects
Store Boilers("Boilers", 2);         // Incineration plant has 2 boilers
Queue WasteQueue("WasteQueue");      // Queue for excess waste
Stat RejectedWaste("RejectedWaste"); // Statistics for rejected waste
Stat ProcessedWaste("ProcessedWaste"); // Statistics for processed waste

// Define the Waste Process
class Waste : public Process {
public:
    void Behavior() override {
        if (Boilers.Full()) { // Check if both boilers are busy
            if (WasteQueue.Length() < HEAP_CAPACITY) {
                // Queue the waste if there is space
                Into(WasteQueue);
                Passivate(); // Wait in the queue
            } else {
                // If no space in queue, reject the waste
                RejectedWaste(1);
                fprintf(outputFile, "   Ton of waste rejected at time: %f\n", Time);
                return;
            }
        }

        // When a boiler becomes available
        Enter(Boilers, 1);
        ProcessedWaste(1); // Log the amount processed
        Wait(HOUR/Uniform(14,16)); // Simulate processing time in minutes
        Leave(Boilers, 1);

        // If there is waste in the queue, activate it
        if (!WasteQueue.Empty()) {
            Waste *nextWaste = static_cast<Waste *>(WasteQueue.GetFirst());
            nextWaste->Activate();
        }
    }
};

// Define the Generator
class WasteGenerator : public Event {
    void Behavior() override {
        double nextArrival;
        double currentTime = fmod(Time, DAY); // Get the time of day

        // If the time is within working hours, generate a Waste
        if (currentTime >= WORK_START && currentTime < WORK_END) {
            int numWastes = Uniform(7, 16); // Random number of waste processes between 7- 15   //TODO mention this in documentation (15.99 to int is 15)
            for (int i = 0; i < numWastes; ++i) {
                (new Waste())->Activate(); // Activate a new Waste process
            }
            nextArrival = Time + Exponential(TRUCKS_ARRIVAL); // Next arrival based on exponential distribution
            fprintf(outputFile, "waste generator activated at: %f h and generated %d\n", currentTime/HOUR, numWastes);

        } else {
            // If outside working hours, schedule for the next working day
            double timeUntilNextWorkStart = (floor(Time / DAY) + 1) * DAY + WORK_START;
            nextArrival = timeUntilNextWorkStart;
            fprintf(outputFile, "------------END OF DAY %.0f------------\n", floor(Time/DAY)+1);
        }

        // Activate the generator for the next Waste
        Activate(nextArrival);
    }
};

int main() {
    // RandomSeed(time(nullptr));
    if (outputFile == NULL)
        return EXIT_FAILURE;
    SetOutput("results.out");

    // Initialize simulation (start at 0, run for 1 year)
    // Init(0, DAY);
    Init(0, YEAR);
    // Init(0, YEAR);

    // Create the first generator
    (new WasteGenerator)->Activate(WORK_START);

    fprintf(outputFile, "start the simulation\n");

    // Run the simulation
    Run();

    // Output results
    Boilers.Output();
    WasteQueue.Output();
    RejectedWaste.Output();
    ProcessedWaste.Output();
    SIMLIB_statistics.Output();

    fclose(outputFile);
    return 0;
}
