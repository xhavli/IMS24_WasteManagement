import subprocess

num_runs = 20
total_rejected_waste = 0

for i in range(num_runs):
    result = subprocess.run(["./main"], stdout=subprocess.PIPE, text=True)
    for line in result.stdout.splitlines():
        if "RejectedWaste" in line:
            rejected_waste = float(line.split(":")[1])
            print(f"Run {i + 1}: Rejected Waste = {rejected_waste}")
            total_rejected_waste += rejected_waste


mean_rejected_waste = total_rejected_waste / num_runs
print(f"Mean Rejected Waste after {num_runs} runs: {mean_rejected_waste:.2f}")
