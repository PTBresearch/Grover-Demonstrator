import pandas as pd
import matplotlib.pyplot as plt
import numpy as np
import os

# List of CSV files
csv_files = ["data_SIMULATOR.csv", "data_SIMULATOR_NOISE.csv", "data_REAL_NOISE_ibm_torino.csv", "data_REAL_ibm_torino.csv"]

# Create figure
fig, axes = plt.subplots(2, 2, figsize=(14, 12))
axes = axes.flatten()

for i, csv_file in enumerate(csv_files):
    # Load data
    data = pd.read_csv(csv_file, header=None)

    # Normalize each row to sum = 1
    data = data.div(data.sum(axis=1), axis=0).fillna(0)

    # Plot heatmap
    im = axes[i].imshow(data, cmap="viridis", aspect="auto")

    # Use filename (without extension) as title
    title = os.path.splitext(os.path.basename(csv_file))[0]
    axes[i].set_title(title, fontsize=14)

    # Label axes
    axes[i].set_xlabel("Buttons")
    axes[i].set_ylabel("Grover iterations")

    # Add colorbar
    fig.colorbar(im, ax=axes[i], fraction=0.046, pad=0.04)

plt.tight_layout()
plt.savefig("heatmaps.png", dpi=300)
plt.show()
