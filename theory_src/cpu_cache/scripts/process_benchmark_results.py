import csv
import os
import subprocess
import sys

def process_benchmark_results(executable_path, output_file, real_time_only=False):
    print(f"Running benchmark: {executable_path}...")
    
    try:
        # Run the benchmark executable
        result = subprocess.run(
            [executable_path, "--benchmark_format=csv"], 
            capture_output=True, 
            text=True, 
            check=True
        )
        lines = result.stdout.splitlines()
    except subprocess.CalledProcessError as e:
        print(f"Error running benchmark: {e}")
        print(f"stderr: {e.stderr}")
        return
    except FileNotFoundError:
        print(f"Error: Benchmark executable not found at {executable_path}")
        return

    # Find the start of the CSV data
    start_index = 0
    for i, line in enumerate(lines):
        if line.startswith("name,"):
            start_index = i
            break
    
    if start_index == 0 and not (lines and lines[0].startswith("name,")):
         # fallback or no data found
         pass

    csv_lines = lines[start_index:]
    
    # Parse CSV
    reader = csv.DictReader(csv_lines)
    
    # Extract data
    filtered_data = []
    
    if real_time_only:
        print(f"{'Real Time (ns)':<15}")
        print("-" * 15)
        fieldnames = ['real_time']
    else:
        print(f"{'Name':<40} | {'Real Time (ns)':<15}")
        print("-" * 60)
        fieldnames = ['name', 'real_time']
    
    for row in reader:
        name = row.get('name', '')
        real_time = row.get('real_time', '')
        
        # Print to console
        if real_time_only:
             print(f"{real_time:<15}")
             filtered_data.append({'real_time': real_time})
        else:
             print(f"{name:<40} | {real_time:<15}")
             filtered_data.append({'name': name, 'real_time': real_time})

    # Write to output file
    with open(output_file, 'w', newline='') as f:
        writer = csv.DictWriter(f, fieldnames=fieldnames)
        writer.writeheader()
        writer.writerows(filtered_data)
        
    print(f"\nFiltered results saved to {output_file}")


if __name__ == "__main__":
    import argparse

    parser = argparse.ArgumentParser(description="Run benchmark and filter results.")
    parser.add_argument("benchmark_exe", help="Path to the benchmark executable")
    parser.add_argument("--output", default="filtered_results.csv", help="Path to the output CSV file")
    parser.add_argument("--real-time-only", action="store_true", help="Only capture the real_time metric")

    args = parser.parse_args()

    benchmark_exe = args.benchmark_exe
    output_csv = args.output
    real_time_only = args.real_time_only
    
    if os.path.exists(benchmark_exe):
        process_benchmark_results(benchmark_exe, output_csv, real_time_only=real_time_only)
    else:
        print(f"Error: Benchmark executable not found at {benchmark_exe}")
        sys.exit(1)
