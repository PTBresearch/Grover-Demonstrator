import csv
from platform import architecture

import numpy as np
from qiskit import QuantumCircuit, generate_preset_pass_manager
from qiskit_aer import AerSimulator
from qiskit_aer.noise import NoiseModel, depolarizing_error
from qiskit_ibm_runtime import QiskitRuntimeService, SamplerV2 as Sampler


# --------------------------
# Parameters parameters
# --------------------------
n_qubits = 6
shots = 64**2
iterations = 12
solution = "101010"
BACKEND = "REAL"
architecture = "ibm_torino"

csv_filename = f"data_{BACKEND}.csv"

# --------------------------
# Simulation Setup
# --------------------------
if BACKEND == "SIMULATOR":
    backend = AerSimulator()

elif BACKEND == "SIMULATOR_NOISE":
    # Simple depolarizing noise model
    noise_model = NoiseModel()
    error_1q = depolarizing_error(0.01, 1)
    error_2q = depolarizing_error(0.05, 2)
    noise_model.add_all_qubit_quantum_error(error_1q, ['h', 'x'])
    noise_model.add_all_qubit_quantum_error(error_2q, ['cx', 'mcx'])

    backend = AerSimulator(noise_model=noise_model)

elif BACKEND == "REAL_NOISE":
    service = QiskitRuntimeService()

    # Specify a QPU to use for the noise model
    real_backend = service.backend(architecture)
    backend = AerSimulator.from_backend(real_backend)

    csv_filename = f"data_{BACKEND}_{architecture}.csv"

elif BACKEND == "REAL":
    # For setting up real computer
    service = QiskitRuntimeService()
    # backend = service.least_busy(simulator=False, operational=True)
    backend = service.backend(architecture)

    csv_filename = f"data_{BACKEND}_{architecture}.csv"


sampler = Sampler(backend)

# --------------------------
# Setup Data File
# --------------------------

with open(csv_filename, mode="w", newline="") as csv_file:
    writer = csv.writer(csv_file)
    # header = ["step_label"] + [f"state_{i}" for i in range(2 ** n_qubits)]
    # writer.writerow(header)

# --------------------------
# Oracle and diffuser
# --------------------------
def create_oracle(n_qubits, solution):
    oracle = QuantumCircuit(n_qubits)
    for i, bit in enumerate(solution):
        if bit == '0':
            oracle.x(i)
    oracle.h(n_qubits - 1)
    oracle.mcx(list(range(n_qubits - 1)), n_qubits - 1)
    oracle.h(n_qubits - 1)
    for i, bit in enumerate(solution):
        if bit == '0':
            oracle.x(i)
    return oracle


def create_diffuser(n_qubits):
    diffuser = QuantumCircuit(n_qubits)
    diffuser.h(range(n_qubits))
    diffuser.x(range(n_qubits))
    diffuser.h(n_qubits - 1)
    diffuser.mcx(list(range(n_qubits - 1)), n_qubits - 1)
    diffuser.h(n_qubits - 1)
    diffuser.x(range(n_qubits))
    diffuser.h(range(n_qubits))
    return diffuser


def simulate_circuit_copy(qc, name):
    global csv_filename

    qc_copy = qc.copy()
    qc_copy.measure_all()

    pm = generate_preset_pass_manager(backend=backend, optimization_level=3)
    isa_qc = pm.run(qc_copy)
    result = sampler.run([isa_qc], shots=shots).result()

    pub_result = result[0]
    counts = pub_result.data.meas.get_counts()

    count_vector = np.zeros(2 ** n_qubits)
    for bitstring in counts.keys():
        count_vector[int(bitstring, 2)] = counts[bitstring]

    with open(csv_filename, mode="a", newline="") as csv_file:
        writer = csv.writer(csv_file)
        writer.writerow(list(count_vector))

    print(f"finished {name}")


# --------------------------
# Initialize circuit
# --------------------------
qc = QuantumCircuit(n_qubits, n_qubits)
qc.h(range(n_qubits))


# --------------------------
# Append Steps
# --------------------------
oracle = create_oracle(n_qubits, solution)
diffuser = create_diffuser(n_qubits)

simulate_circuit_copy(qc, f"iteration_0_oracle")
for i in range(iterations):
    # Oracle step
    qc.compose(oracle, inplace=True)
    # simulate_circuit_copy(qc, f"iteration_{i + 1}_oracle")

    # Diffuser step
    qc.compose(diffuser, inplace=True)
    simulate_circuit_copy(qc, f"iteration_{i + 1}_diffuser")

