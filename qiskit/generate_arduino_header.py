import csv

# CSV file names
csv_files = ["data_SIMULATOR.csv", "data_SIMULATOR_NOISE.csv", "data_REAL_ibm_torino.csv"]
# Output .h file
output_file = "probabilities.h"

def read_csv(filename):
    """Read a CSV file into a 2D list of ints."""
    with open(filename, newline='') as f:
        reader = csv.reader(f)
        data = [[int(float(val)) for val in row] for row in reader]
    return data

def generate_header(csv_files, output_file):
    with open(output_file, 'w') as f_out:
        f_out.write('#include <Arduino.h>\n')
        f_out.write('#include <avr/pgmspace.h>\n\n')
        f_out.write('const uint16_t probabilities[3][26][64] PROGMEM = {\n')

        for sheet_index, fname in enumerate(csv_files):
            data = read_csv(fname)
            f_out.write('  {\n')
            for row_index, row in enumerate(data):
                f_out.write('    { ')
                row_str = ', '.join(f'{int(round(float(val))):4d}' for val in row)
                f_out.write(row_str)
                f_out.write(' }')
                if row_index != len(data)-1:
                    f_out.write(',')
                f_out.write('\n')
            f_out.write('  }')
            if sheet_index != len(csv_files)-1:
                f_out.write(',')
            f_out.write('\n')
        f_out.write('};\n')

if __name__ == "__main__":
    generate_header(csv_files, output_file)
