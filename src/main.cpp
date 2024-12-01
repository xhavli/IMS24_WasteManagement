#include "simlib.h" // Include the SIMLIB library
#include <math.h>
#include <time.h>

// Constants
const int MINUTE = 1;       // simulation step equals one minute
const int TRUCKS_ARRIVAL = 20 * MINUTE;
const int HOUR = 60 * MINUTE;
const int DAY = 24 * HOUR;
const int YEAR = 365 * DAY;
const double HEAP_CAPACITY = 200.0;    // 200 tons
const double BOILER_CAPACITY = 15.0;   // 15 tons per hour per boiler
const double WORK_START = 6.0 * HOUR;         // Start of working hours (6:00 AM)
const double WORK_END = 14.0 * HOUR;          // End of working hours (2:00 PM)
// const double WORK_END_BOILER = 20.0;   // End of working hours (8:00 PM)
const double HOURS_IN_DAY = 24.0;      // Duration of a day in hours

FILE *outputFile = fopen("results.out", "w");

// Global SIMLIB objects
Store Boilers("Boilers", 2);         // Incineration plant has 2 boilers
Queue WasteQueue("WasteQueue");      // Queue for excess waste
Stat RejectedWaste("RejectedWaste"); // Statistics for rejected waste
Stat ProcessedWaste("ProcessedWaste"); // Statistics for processed waste

// Define the Waste Process
class Waste : public Process {
    double amount; // Amount of waste in tons

public:
    Waste(double amt) : amount(amt) {}

    void Behavior() override {
        if (Boilers.Full()) { // Check if both boilers are busy
            if (WasteQueue.Length() + amount <= HEAP_CAPACITY) {
                // Queue the waste if there is space
                Into(WasteQueue);
                Passivate(); // Wait in the queue
            } else {
                // If no space in queue, reject the waste
                RejectedWaste(amount);
                fprintf(outputFile, "Waste rejected at time: %f, amount: %.2f\n", fmod(Time, DAY)/HOURS_IN_DAY, amount);
                return;
            }
        }

        // When a boiler becomes available
        Enter(Boilers, 1);
        ProcessedWaste(amount); // Log the amount processed
        Wait(HOUR * amount / BOILER_CAPACITY); // Simulate processing time
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
        double currentTime = fmod(Time, DAY)/HOURS_IN_DAY; // Get the time of day (modulo 24 hours)
        double nextArrival;

        // If the time is within working hours, generate a Waste
        if (currentTime >= WORK_START && currentTime < WORK_END) {
            double garbageAmount = Uniform(7, 15); // Random amount of garbage between 7-15 tons
            (new Waste(garbageAmount))->Activate(); // Activate a new Waste process
            nextArrival = Time + Exponential(10);
        } else {
            // If outside working hours, schedule for the next working day
            double timeUntilNextWorkStart = (floor(Time / HOURS_IN_DAY) + 1) * HOURS_IN_DAY + WORK_START;
            nextArrival = timeUntilNextWorkStart;
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
    Init(0, YEAR);

    // Create the first generator
    (new WasteGenerator)->Activate(WORK_START);

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
