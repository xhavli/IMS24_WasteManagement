/* 
 * @file main.cpp
 * @author Adam Havlik (xhavli59)
 * @author Martin Knor (xknorm01)
 * @date 11.2024
 * @brief program entry point
 */

#include <iostream> // Include the input/output stream library
#include "simlib.h"


int MINUTE = 1;             // simulation step equals one minute
int HOUR = MINUTE * 60;
int DAY = HOUR * 24;
int YEAR = DAY * 365;

int main() {
    Init(0, YEAR); // Initialize the simulation
    Run();        // Run the simulation
    return 0;
}
