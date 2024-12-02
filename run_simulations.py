import subprocess
import pandas as pd
import matplotlib.pyplot as plt

# Define range of TRUCKS_ARRIVAL rates and boiler capacities
trucks_arrival_rates = range(4, 13)  # 4 to 12 in steps of 1
boiler_capacities = [2, 3]  # Boilers with capacities 2 and 3

# Constants for lost heat and electricity calculations
HEAT_LOSS_PER_TON = 4_870_000_000  # Joules
ELECTRICITY_LOSS_PER_TON = 219_000  # Watts
JOULES_TO_GJ = 1e-9  # Conversion factor for Joules to GigaJoules
WATTS_TO_GWH = 1 / 1_000_000_000  # Conversion factor for Watts to GWh (assuming 1 hour)

# Results dictionary
results = []

# Run simulations
for capacity in boiler_capacities:
    for rate in trucks_arrival_rates:
        # Run the simulation with specified parameters
        result = subprocess.run(["./main", str(capacity), str(rate)],
                                stdout=subprocess.PIPE, text=True)
        for line in result.stdout.splitlines():
            if "RejectedWaste" in line:
                rejected_waste = float(line.split(":")[1])
                lost_heat_gj = rejected_waste * HEAT_LOSS_PER_TON * JOULES_TO_GJ
                lost_electricity_gwh = rejected_waste * ELECTRICITY_LOSS_PER_TON * WATTS_TO_GWH
                # Save results
                results.append({
                    "BoilerCapacity": capacity,
                    "TrucksArrivalRate": rate,
                    "RejectedWaste": rejected_waste,
                    "LostHeatGJ": lost_heat_gj,
                    "LostElectricityGWh": lost_electricity_gwh
                })
                break

# Convert results to a DataFrame for analysis
df = pd.DataFrame(results)

# Create plots for each metric
metrics = {
    "Rejected Waste (tons)": "RejectedWaste",
    "Lost Heat (GJ)": "LostHeatGJ",
    "Lost Electricity (GWh)": "LostElectricityGWh"
}

for metric_name, metric_column in metrics.items():
    plt.figure()  # Create a new figure for each metric
    for capacity in boiler_capacities:
        subset = df[df["BoilerCapacity"] == capacity]
        plt.plot(subset["TrucksArrivalRate"], subset[metric_column], label=f"Boiler Capacity = {capacity}")
    plt.title(f"Impact of Trucks Arrival Rate on {metric_name}")
    plt.xlabel("Trucks Arrival Rate (minutes)")
    plt.ylabel(metric_name)
    plt.legend()
    plt.grid()
    filename = f"impact_of_trucks_arrival_rate_{metric_column.lower()}.png"
    plt.savefig(filename)
    print(f"Graph saved as '{filename}'")
