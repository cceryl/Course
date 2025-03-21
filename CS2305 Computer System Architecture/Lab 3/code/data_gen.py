import numpy as np
import argparse


def rand_matrix(row, col):
    return np.random.randint(-256, 256, (row, col), dtype=np.int32)


def save_matrix(filename, matrix):
    with open(filename, 'wb') as f:
        f.write(matrix.tobytes())


def save_config(filename, config):
    with open(filename, 'w') as f:
        f.write("#ifndef CONFIG_H\n")
        f.write("#define CONFIG_H\n")
        for key, value in config.items():
            f.write(f"#define {key} {value}\n")
        f.write("#endif // CONFIG_H\n")


parser = argparse.ArgumentParser(
    description='Generate matrix multiplication data files')
parser.add_argument('--L_row', type=int, default=2048,
                    help='Row number of left matrix')
parser.add_argument('--L_col', type=int, default=2048,
                    help='Column number of left matrix')
parser.add_argument('--R_row', type=int, default=2048,
                    help='Row number of right matrix')
parser.add_argument('--R_col', type=int, default=2048,
                    help='Column number of right matrix')

args = parser.parse_args()

config = {
    'L_ROW': args.L_row,
    'L_COL': args.L_col,
    'R_ROW': args.R_row,
    'R_COL': args.R_col
}

print("Saving L matrix with shape", args.L_row, args.L_col)
L = rand_matrix(args.L_row, args.L_col)
save_matrix('L.dat', L)

print("Saving R matrix with shape", args.R_row, args.R_col)
R = rand_matrix(args.R_row, args.R_col)
save_matrix('R.dat', R)

print("Calcing true value")
true_value = np.dot(L, R)
print("Saving true value matrix with shape", true_value.shape)
save_matrix('true_value.dat', true_value)

save_config('config.h', config)
