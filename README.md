# ParcelMerge

ParcelMerge is a DSA-based delivery planner for the sample Warehouse, Sanjauli, Dhalli, and Mall Road network. C++ computes routes and packs trips, Python serves the planner API, and the browser uses HTML, CSS, and JavaScript.

## How the planner works

- An adjacency list stores the undirected road network. Dijkstra's min-heap computes shortest distances from the warehouse, and a predecessor map reconstructs the routes.
- A priority queue orders parcels by urgency, then by ascending ID. A hash map tracks the current open trip for each destination. A new trip opens when the current one cannot fit the next parcel. Trips dispatch in the order they open, with each trip taking the priority of its first parcel. A lower-priority parcel can ride with a higher-priority parcel.
- Each trip serves one destination using a greedy next-fit rule. Its round-trip distance is twice the shortest outbound distance. The baseline sends each parcel on a separate round trip. Subtracting the consolidated totals gives the trips and distance saved.
- The Python server validates inputs for `POST /api/plan`, invokes the C++ executable, and returns its JSON results to the browser.

Enter the one-way distances for the five roads before calculating. Distances are in kilometers and support up to three decimal places. Each road can be traveled in both directions at the same distance.
