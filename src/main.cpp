#include "simlib.h"
#include <random>

// Constants
FILE *resultsFile = fopen("results.out", "w");

const int MINUTE = 1;       // simulation step equals one minute
const int TRUCKS_ARRIVAL = 20 * MINUTE;
const int HOUR = 60 * MINUTE;
const int DAY = 24 * HOUR;
const int YEAR = 365 * DAY;
const double HEAP_CAPACITY = 200.0;     // 200 tons

// Variables
double heap = 0.0;             // Current waste on the heap
double flushedWaste = 0.0;     // Total flushed waste
double boilerWasteProcessed = 0.0; // Total waste processed by boilers

// Random number generator and distributions
std::random_device rd;
std::mt19937 gen(rd()); // Mersenne Twister engine seeded with random device
std::uniform_real_distribution<> truckWasteDist(7.0, 15.0); // For truck waste [7, 15]
std::uniform_real_distribution<> boilerConsumptionDist(14.0, 16.0); // For boiler consumption [14, 16]

// Histograms
Histogram heapSizeHistogram("Heap Size Distribution", 0.0, 25.0, 8); // 8 bins, 25 tons each
Histogram boilerConsumptionHistogram("Boiler Consumption per Operation", 0.0, 20.0, 10); // 10 bins, 2 tons each

// Garbage Truck Arrival Event
class GarbageTruckArrival : public Event {
    void Behavior() override {
        double waste = truckWasteDist(gen); // Generate waste (7-15 tons)

        if (heap + waste > HEAP_CAPACITY) {
            flushedWaste += (heap + waste) - HEAP_CAPACITY; // Increment flushed waste
            heap = HEAP_CAPACITY; // Heap remains at max capacity
            fprintf(resultsFile, "Heap overflow. Flushed waste: %.2f tons\n", flushedWaste);
        } else {
            heap += waste; // Add waste to heap
            fprintf(resultsFile, "Truck arrived. Added %.2f tons. Current heap: %.2f tons\n", waste, heap);
        }

        // Record heap size in histogram
        heapSizeHistogram(heap);

        Activate(Time + Exponential(TRUCKS_ARRIVAL)); // Schedule next truck
    }
};

// Boiler Operation Event
class BoilerOperation : public Event {
    void Behavior() override {
        for (int i = 0; i < 2; i++) { // Two boilers
            double consumption = boilerConsumptionDist(gen); // Each boiler's consumption
            if (heap >= consumption) {
                heap -= consumption;
                boilerWasteProcessed += consumption;
                fprintf(resultsFile, "Boiler %d processed %.2f tons. Current heap: %.2f tons\n", i + 1, consumption, heap);
            } else {
                boilerWasteProcessed += heap;
                fprintf(resultsFile, "Boiler %d processed %.2f tons (partial). Heap empty.\n", i + 1, heap);
                heap = 0.0; // Heap is empty
            }

            // Record boiler consumption in histogram
            boilerConsumptionHistogram(consumption);
        }

        // Record heap size in histogram after boilers' operation
        heapSizeHistogram(heap);

        Activate(Time + HOUR); // Schedule next boiler operation (hourly)
    }
};

// Main Program
int main() {
    // Initialize the simulation
    SetOutput("results.out");
    Init(0, YEAR); // Simulate for 1 year

    // Schedule the first events
    (new GarbageTruckArrival)->Activate(); // First garbage truck arrival
    (new BoilerOperation)->Activate();    // First boiler operation

    // Run the simulation
    Run();

    // Print final results
    fprintf(resultsFile, "====================================\n");
    fprintf(resultsFile, "Simulation Results:\n");
    fprintf(resultsFile, "Total waste flushed on dump: %.2f tons\n", flushedWaste);
    fprintf(resultsFile, "Total waste processed by boilers: %.2f tons\n", boilerWasteProcessed);
    fprintf(resultsFile, "====================================\n");

    // Output histogram statistics
    fprintf(resultsFile, "\nHeap Size Histogram:\n");
    heapSizeHistogram.Output();
    fprintf(resultsFile, "\nBoiler Consumption Histogram:\n");
    boilerConsumptionHistogram.Output();

    return 0;
}
