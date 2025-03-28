
#include "nn_math.h"

void mat_mult(const int8_t *mat_l, const int8_t *mat_r, int *result, const unsigned int N, const unsigned int K, const unsigned int M)
{
    unsigned int n, k, m;
    unsigned int row, col;
    int          accumulator;

    for (m = 0; m < M; m++)
    {
        for (n = 0; n < N; n++)
        {
            row         = n * K;
            accumulator = 0;
            for (k = 0; k < K; k++)
            {
                col          = k * M;
                accumulator += mat_l[row + k] * mat_r[col + m];
            }
            result[n * M + m] = accumulator;
        }
    }
}

int get_output_dim(int input_dim, int kernel_size, int stride)
{
    int output_dim = (input_dim - (kernel_size - 1) - 1) / stride;
    return output_dim + 1;
}

void relu(int *tensor, const unsigned int size)
{
    unsigned int i;
    for (i = 0; i < size; i++)
        tensor[i] = MAX(tensor[i], 0);
}

void quantize(const int *tensor_in, int8_t *tensor_q, const int scale_factor, const int scale_factor_inv, const unsigned int size)
{
    unsigned int i;
    int          rounded_value, tensor_int, tensor_frac;
    // separation to integer and fraction parts
    int          scale_factor_int  = (scale_factor + ROUND_CONST) >> FXP_VALUE;
    int          scale_factor_frac = scale_factor - (scale_factor_int << FXP_VALUE);
    // element wise operation - we iterate throughout the entire length of the flattened tensor
    for (i = 0; i < size; i++)
    {
        tensor_int = (tensor_in[i] + ROUND_CONST) >> FXP_VALUE;
        if (tensor_int > INT8_MAX_VALUE * scale_factor_inv)
            tensor_q[i] = (int8_t)INT8_MAX_VALUE;
        else if (tensor_int < -INT8_MAX_VALUE * scale_factor_inv)
            tensor_q[i] = -(int8_t)INT8_MAX_VALUE;
        else
        {
            tensor_frac    = tensor_in[i] - (tensor_int << FXP_VALUE);
            // int * fxp = result is in fxp */
            rounded_value  = tensor_int * scale_factor_frac + scale_factor_int * tensor_frac;
            // fxp * fxp = fix-point multiplication with result is in fxp */
            rounded_value += (tensor_frac * scale_factor_frac + ROUND_CONST) >> FXP_VALUE;
            // convert fxp to int and add to integer parts as final value should be a rounded integer
            rounded_value  = ((rounded_value + ROUND_CONST) >> FXP_VALUE) + tensor_int * scale_factor_int;

            tensor_q[i] = (int8_t)rounded_value; /* store quantized value in output tensor */
        }
    }
}

void dequantize_per_row(int *mat_in, const int *scale_factor_w_inv, const int scale_factor_x_inv, const unsigned int N,
                        const unsigned int M)
{
    unsigned int k, n;

    int out_value;

    for (n = 0; n < N; n++)
    {
        for (k = 0; k < M; k++)
        {
            out_value = scale_factor_w_inv[k] * scale_factor_x_inv;
            if (out_value > (1 << FXP_VALUE))
                mat_in[n * M + k] *= ((out_value + ROUND_CONST) >> FXP_VALUE);
            else
                mat_in[n * M + k] = (out_value * mat_in[n * M + k] + ROUND_CONST) >> FXP_VALUE;
        }
    }
}

void dequantize_per_channel(int *tensor_in, const int *amax_w, const int amax_x, const unsigned int N, const unsigned int C,
                            const unsigned int K)
{
    unsigned int k, n, c;

    int out_value;

    for (n = 0; n < N; n++)
    {
        for (c = 0; c < C; c++)
        {
            for (k = 0; k < K; k++)
            {
                out_value = amax_w[c] * amax_x;
                if (out_value > (1 << FXP_VALUE))
                    tensor_in[n * C + c * K + k] *= ((out_value + ROUND_CONST) >> FXP_VALUE);
                else
                    tensor_in[n * C + c * K + k] = (out_value * tensor_in[n * C + c * K + k] + ROUND_CONST) >> FXP_VALUE;
            }
        }
    }
}

void argmax_over_cols(const int *mat_in, unsigned int *indices, const unsigned int N, const unsigned int M)
{
    // calculate max of each row
    unsigned int n, m, max_idx;
    int          row_max, value;
    for (n = 0; n < N; n++)
    {
        row_max = mat_in[n * M];
        max_idx = 0;
        for (m = 0; m < M; m++)
        {
            value = mat_in[n * M + m];
            if (value > row_max)
            {
                row_max = value;
                max_idx = m; // return column
            }
        }
        indices[n] = max_idx;
    }
}
