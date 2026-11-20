"use strict";

const form = document.querySelector("#planner");
const parcels = document.querySelector("#parcels");
const distanceInputs = document.querySelectorAll("#road-distances input");
const template = document.querySelector("#parcel-template");
const status = document.querySelector("#status");
const results = document.querySelector("#results");
const calculate = form.querySelector("[type=submit]");
const add = document.querySelector("#add");
const parcelCount = document.querySelector("#parcel-count");
const planContent = document.querySelector("#plan-content");
const priorities = ["", "Urgent", "Express", "Normal"];
const serverHelp = "Start the planner server with python python/app.py and open http://127.0.0.1:8000.";
const formatDistance = new Intl.NumberFormat(undefined, {maximumFractionDigits: 3}).format;
let nextId = 101;

function updateParcelCount() {
  const count = parcels.children.length;
  parcelCount.textContent = `${count} ${count === 1 ? "parcel" : "parcels"}`;
  add.disabled = count >= 1000;
}

function addParcel(destination = "Sanjauli", weight = "", priority = 3, focus = false) {
  const row = template.content.firstElementChild.cloneNode(true);
  const id = nextId++;
  row.querySelector("legend").textContent = `Parcel entry ${id}`;
  row.querySelector("[name=id]").value = id;
  row.querySelector("[name=destination]").value = destination;
  const weightInput = row.querySelector("[name=weight]");
  weightInput.value = weight;
  row.querySelector("[name=priority]").value = priority;
  row.querySelector(".remove").addEventListener("click", () => {
    const neighbor = row.nextElementSibling ?? row.previousElementSibling;
    row.remove();
    updateParcelCount();
    (neighbor?.querySelector("[name=id]") ?? add).focus();
  });
  parcels.append(row);
  updateParcelCount();
  if (focus) weightInput.focus();
}

function element(tag, content, className) {
  const node = document.createElement(tag);
  if (content !== undefined) node.textContent = content;
  if (className) node.className = className;
  return node;
}

function render(plan) {
  const metrics = element("dl", undefined, "metrics");
  for (const [label, value] of [
    ["Trips saved", plan.tripsSaved],
    ["Distance saved", `${formatDistance(plan.distanceSaved)} km`],
    ["Round trips", `${plan.baselineTrips} → ${plan.plannedTrips}`],
    ["Round-trip distance", `${formatDistance(plan.baselineDistance)} → ${formatDistance(plan.plannedDistance)} km`],
  ]) {
    const metric = element("div");
    metric.append(element("dt", label), element("dd", value));
    metrics.append(metric);
  }
  const trips = element("ol", undefined, "trips");
  plan.trips.forEach((trip, index) => {
    const item = element("li", undefined, "trip");
    const roundTrip = trip.route.concat(trip.route.slice(0, -1).reverse());
    item.append(
      element("h3", `Trip ${index + 1} · ${trip.destination}`),
      element("p", roundTrip.join(" → "), "trip-route"),
      element("p", `${priorities[trip.priority]} · ${trip.load} kg · ${formatDistance(trip.distance)} km`, "trip-details"),
      element("p", `Parcels ${trip.parcelIds.join(", ")}`),
    );
    trips.append(item);
  });
  results.replaceChildren(metrics);
  if (plan.trips.length) {
    results.append(element("h3", "Dispatch order", "trips-title"), trips);
  }
}

add.addEventListener("click", () => addParcel("Sanjauli", "", 3, true));
form.addEventListener("submit", async (event) => {
  event.preventDefault();
  const payload = {
    capacity: document.querySelector("#capacity").valueAsNumber,
    distances: Object.fromEntries(Array.from(distanceInputs, (input) => [input.name, input.valueAsNumber])),
    parcels: Array.from(parcels.children, (row) => ({
      id: row.querySelector("[name=id]").valueAsNumber,
      destination: row.querySelector("[name=destination]").value,
      weight: row.querySelector("[name=weight]").valueAsNumber,
      priority: Number(row.querySelector("[name=priority]").value),
    })),
  };
  planContent.dataset.state = "loading";
  status.textContent = "Calculating delivery routes…";
  results.replaceChildren();
  calculate.disabled = true;
  try {
    if (window.location.protocol === "file:") {
      throw new Error(serverHelp);
    }
    const response = await fetch("/api/plan", {
      method: "POST",
      headers: {"Content-Type": "application/json"},
      body: JSON.stringify(payload),
    }).catch(() => {
      throw new Error(`Could not reach the planner server. ${serverHelp}`);
    });
    if (!response.headers.get("content-type")?.includes("application/json")) {
      throw new Error(`The planner API is unavailable on this page. ${serverHelp}`);
    }
    const plan = await response.json();
    if (!response.ok) throw new Error(plan.error);
    render(plan);
    planContent.dataset.state = "ready";
    status.textContent = plan.trips.length
      ? `Plan ready. ${plan.plannedTrips} ${plan.plannedTrips === 1 ? "trip" : "trips"} scheduled.`
      : "The manifest is empty.";
  } catch (error) {
    planContent.dataset.state = "error";
    status.textContent = error.message || "Could not calculate the plan.";
  } finally {
    calculate.disabled = false;
  }
});

addParcel("Sanjauli", 2.5, 1);
addParcel("Sanjauli", 1.5, 3);
addParcel("Dhalli", 3, 2);
addParcel("Sanjauli", 2, 1);
addParcel("Dhalli", 1, 3);
