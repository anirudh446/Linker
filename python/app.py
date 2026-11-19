"""Local HTTP interface for the ParcelMerge C++ engine."""

import argparse
import json
import math
from functools import partial
from http import HTTPStatus
from http.server import SimpleHTTPRequestHandler, ThreadingHTTPServer
from pathlib import Path
import os
import subprocess

ROOT = Path(__file__).resolve().parents[1]
DESTINATIONS = {"Sanjauli", "Dhalli", "Mall Road"}


def positive_number(value, label):
    if type(value) not in (int, float) or not 0 < value <= 1e9 or not math.isfinite(value):
        raise ValueError(f"{label} must be a positive number up to 1 billion.")
    return value


def engine_input(payload):
    if not isinstance(payload, dict):
        raise ValueError("Expected a JSON object.")
    capacity = positive_number(payload.get("capacity"), "Vehicle capacity")
    parcels = payload.get("parcels")
    if not isinstance(parcels, list) or len(parcels) > 1000:
        raise ValueError("Provide a parcel list containing at most 1000 parcels.")
    records = [f"{capacity} {len(parcels)}"]
    for index, parcel in enumerate(parcels, 1):
        if not isinstance(parcel, dict):
            raise ValueError(f"Parcel row {index} must be an object.")
        parcel_id = parcel.get("id")
        priority = parcel.get("priority")
        destination = parcel.get("destination")
        weight = positive_number(parcel.get("weight"), f"Weight in row {index}")
        if type(parcel_id) is not int or not 1 <= parcel_id <= 2147483647:
            raise ValueError(f"Parcel ID in row {index} must be a positive 32-bit integer.")
        if type(priority) is not int or priority not in (1, 2, 3):
            raise ValueError(f"Choose a priority in row {index}.")
        if not isinstance(destination, str) or destination not in DESTINATIONS:
            raise ValueError(f"Choose a network destination in row {index}.")
        records.append(f'{parcel_id} {weight} {priority} "{destination}"')
    return "\n".join(records) + "\n"


def optimize(payload, executable):
    result = subprocess.run(
        [str(executable)], input=engine_input(payload), capture_output=True,
        text=True, encoding="utf-8", timeout=5, check=False,
    )
    if result.returncode == 1:
        raise ValueError(result.stderr.strip() or "The engine rejected the delivery plan.")
    result.check_returncode()
    return json.loads(result.stdout)


class ParcelHandler(SimpleHTTPRequestHandler):
    def __init__(self, *args, executable, **kwargs):
        self.executable = executable
        super().__init__(*args, directory=str(ROOT / "web"), **kwargs)

    def do_POST(self):
        if self.path != "/api/plan":
            self.send_json(HTTPStatus.NOT_FOUND, {"error": "Endpoint not found."})
            return
        try:
            length = int(self.headers.get("Content-Length", "0"))
            if not 0 < length <= 262144:
                raise ValueError("Request body must contain between 1 and 262144 bytes.")
            payload = json.loads(self.rfile.read(length))
        except ValueError as error:
            self.send_json(HTTPStatus.BAD_REQUEST, {"error": str(error)})
            return
        try:
            plan = optimize(payload, self.executable)
        except json.JSONDecodeError:
            self.send_json(HTTPStatus.BAD_GATEWAY, {"error": "The C++ engine returned invalid JSON."})
        except ValueError as error:
            self.send_json(HTTPStatus.BAD_REQUEST, {"error": str(error)})
        except (OSError, subprocess.SubprocessError) as error:
            self.log_error("C++ engine failed: %s", error)
            self.send_json(HTTPStatus.SERVICE_UNAVAILABLE,
                           {"error": "The C++ engine could not run. Check the server console."})
        else:
            self.send_json(HTTPStatus.OK, plan)

    def send_json(self, status, payload):
        body = json.dumps(payload).encode("utf-8")
        self.send_response(status)
        self.send_header("Content-Type", "application/json")
        self.send_header("Content-Length", str(len(body)))
        self.end_headers()
        self.wfile.write(body)


if __name__ == "__main__":
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--port", type=int, default=8000)
    suffix = ".exe" if os.name == "nt" else ""
    parser.add_argument("--engine", type=Path, default=ROOT / "cpp" / f"parcelmerge_engine{suffix}")
    args = parser.parse_args()
    if not args.engine.is_file():
        parser.error(f"Build the C++ engine first. Executable expected at {args.engine}")
    handler = partial(ParcelHandler, executable=args.engine.resolve())
    with ThreadingHTTPServer(("127.0.0.1", args.port), handler) as server:
        print(f"ParcelMerge is available at http://127.0.0.1:{server.server_port}", flush=True)
        server.serve_forever()
