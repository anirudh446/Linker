# ParcelMerge

ParcelMerge is a DSA-based delivery planner for the sample Warehouse, Sanjauli, Dhalli, and Mall Road network. C++ computes routes and packs trips, Python serves the planner API, and the browser uses HTML, CSS, and JavaScript.

## Run locally

From the repository root on Windows, with a C++17 compiler and Python 3 installed:

```powershell
g++ -std=c++17 -Wall -Wextra -Wpedantic cpp/engine.cpp cpp/planner.cpp cpp/graph.cpp cpp/parcel.cpp -o cpp/parcelmerge_engine.exe
python python/app.py
```

Open http://127.0.0.1:8000. On Linux or macOS, use `-o cpp/parcelmerge_engine` for the build. Use `--engine PATH` to select a different executable or `--port PORT` to change the server port.

The original console demonstration can still be built with `g++ -std=c++17 cpp/main.cpp cpp/graph.cpp cpp/parcel.cpp -o cpp/parcelmerge_demo.exe`.

## How the planner works

- An adjacency list stores the undirected road network. Dijkstra's min-heap computes shortest distances from the warehouse, and a predecessor map reconstructs the routes.
- A priority queue orders parcels by urgency, then by ascending ID. A hash map tracks the current open trip for each destination. A new trip opens when the current one cannot fit the next parcel. Trips dispatch in the order they open, with each trip taking the priority of its first parcel. A lower-priority parcel can ride with a higher-priority parcel.
- Each trip serves one destination using a greedy next-fit rule. Its round-trip distance is twice the shortest outbound distance. The baseline sends each parcel on a separate round trip. Subtracting the consolidated totals gives the trips and distance saved.
- The Python server validates inputs for `POST /api/plan`, invokes the C++ executable, and returns its JSON results to the browser.

With the preloaded five parcels and a 5 kg vehicle, the expected result is 3 trips instead of 5 and 36 km instead of 62 km. A standalone route to Mall Road goes through Sanjauli and is 18 km round trip.
