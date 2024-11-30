#include "simlib.h"

// Constants

// Constants
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

// Histograms
Histogram heapSizeHistogram("Heap Size Distribution", 0.0, 25.0, 8); // 8 bins, 25 tons each
Histogram boilerConsumptionHistogram("Boiler Consumption per Operation", 0.0, 20.0, 10); // 10 bins, 2 tons each

// Garbage Truck Arrival Event
class GarbageTruckArrival : public Event {
    void Behavior() override {
        double waste = Uniform(7, 15); // Generate waste (7-15 tons)

        if (heap + waste > HEAP_CAPACITY) {
            flushedWaste += (heap + waste) - HEAP_CAPACITY; // Increment flushed waste
            heap = HEAP_CAPACITY; // Heap remains at max capacity
            printf("Heap overflow. Flushed waste: %.2f tons\n", flushedWaste);
        } else {
            heap += waste; // Add waste to heap
            printf("Truck arrived. Added %.2f tons. Current heap: %.2f tons\n", waste, heap);
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
            double consumption = Uniform(14, 16); // Each boiler's consumption
            if (heap >= consumption) {
                heap -= consumption;
                boilerWasteProcessed += consumption;
                printf("Boiler %d processed %.2f tons. Current heap: %.2f tons\n", i + 1, consumption, heap);
            } else {
                boilerWasteProcessed += heap;
                printf("Boiler %d processed %.2f tons (partial). Heap empty.\n", i + 1, heap);
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
    Init(0, YEAR); // Simulate for 1 year

    // Schedule the first events
    (new GarbageTruckArrival)->Activate(); // First garbage truck arrival
    (new BoilerOperation)->Activate();    // First boiler operation

    // Run the simulation
    Run();

    // Print final results
    printf("====================================\n");
    printf("Simulation Results:\n");
    printf("Total waste flushed on dump: %.2f tons\n", flushedWaste);
    printf("Total waste processed by boilers: %.2f tons\n", boilerWasteProcessed);
    printf("====================================\n");

    // Output histogram statistics
    printf("\nHeap Size Histogram:\n");
    heapSizeHistogram.Output();
    printf("\nBoiler Consumption Histogram:\n");
    boilerConsumptionHistogram.Output();

    return 0;
}
