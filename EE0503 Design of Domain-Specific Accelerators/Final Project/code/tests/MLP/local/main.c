#include "mlp.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#define IMAGE_SIZE  28 * 28 // MNIST images should be 28x28
#define NUM_CLASSES 10      // MNIST has 10 classes (digits 0-9)

// Mean and standard deviation for normalization
#define MEAN 0.1307
#define STD  0.3081

#define RUN_IMAGE_START 0
#define RUN_IMAGE_NUM   10000
#define PRINT_RESULTS   0

void load_images(const char *filename, float **data, unsigned int *num_samples)
{
    FILE *file = fopen(filename, "rb");
    if (!file)
    {
        fprintf(stderr, "Error: Could not open file %s\n", filename);
        exit(1);
    }

    // Read the header
    uint32_t magic_number, num_images, num_rows, num_cols;
    fread(&magic_number, sizeof(uint32_t), 1, file);
    fread(&num_images, sizeof(uint32_t), 1, file);
    fread(&num_rows, sizeof(uint32_t), 1, file);
    fread(&num_cols, sizeof(uint32_t), 1, file);
    magic_number = __builtin_bswap32(magic_number);
    num_images   = __builtin_bswap32(num_images);
    num_rows     = __builtin_bswap32(num_rows);
    num_cols     = __builtin_bswap32(num_cols);

    // Read the image data
    uint8_t *buffer = (uint8_t *)malloc(num_images * IMAGE_SIZE);
    fread(buffer, sizeof(uint8_t), num_images * IMAGE_SIZE, file);

    // Convert and normalize image data to float
    *data = (float *)malloc(num_images * IMAGE_SIZE * sizeof(float));
    for (unsigned int i = 0; i < num_images * IMAGE_SIZE; ++i)
        (*data)[i] = ((float)buffer[i] / 255.0 - MEAN) / STD;

    *num_samples = num_images;

    free(buffer);
    fclose(file);
}

void load_labels(const char *filename, unsigned int **labels, unsigned int *num_samples)
{
    FILE *file = fopen(filename, "rb");
    if (!file)
    {
        fprintf(stderr, "Error: Could not open file %s\n", filename);
        exit(1);
    }

    // Read the header
    uint32_t magic_number, num_items;
    fread(&magic_number, sizeof(uint32_t), 1, file);
    fread(&num_items, sizeof(uint32_t), 1, file);
    magic_number = __builtin_bswap32(magic_number);
    num_items    = __builtin_bswap32(num_items);

    // Read the label data
    uint8_t *buffer = (uint8_t *)malloc(num_items);
    fread(buffer, sizeof(uint8_t), num_items, file);

    // Convert label data to uint32
    *labels = (unsigned int *)malloc(num_items * sizeof(unsigned int));
    for (unsigned int i = 0; i < num_items; ++i)
        (*labels)[i] = (unsigned int)buffer[i];

    *num_samples = num_items;

    free(buffer);
    fclose(file);
}

int main()
{
    unsigned int  num_samples;
    float        *data;
    unsigned int *labels;

    // Load the MNIST test images and labels
    load_images("./data/t10k-images-idx3-ubyte", &data, &num_samples);
    load_labels("./data/t10k-labels-idx1-ubyte", &labels, &num_samples);
    printf("Loaded %u samples.\n", num_samples);

    // Allocate memory for a single sample and its predicted class
    int         *sample = (int *)malloc(IMAGE_SIZE * sizeof(int));
    unsigned int class_index;

    // Run the MLP on each sample and calculate accuracy
    unsigned int correct_predictions = 0;
    for (unsigned int i = RUN_IMAGE_START; i < RUN_IMAGE_START + RUN_IMAGE_NUM /* num_samples */; ++i)
    {
        // Copy the i-th sample to the sample array
        for (unsigned int j = 0; j < IMAGE_SIZE; ++j)
            sample[j] = data[i * IMAGE_SIZE + j] * (1 << 16); // Convert float to 16-fixed-point

        // Run the MLP on the single sample
        run_mlp(sample, 1, &class_index);

        // Check if the prediction is correct
        if (class_index == labels[i])
            ++correct_predictions;

        if (PRINT_RESULTS)
            printf("Sample %u: Predicted class: %u, Actual class: %u\n", i, class_index, labels[i]);
    }

    printf("Accuracy: %.2f%% in samples %d to %d", (correct_predictions / (float)RUN_IMAGE_NUM) * 100.0, RUN_IMAGE_START,
           RUN_IMAGE_START + RUN_IMAGE_NUM - 1);

    // Free the allocated memory
    free(data);
    free(labels);
    free(sample);

    return 0;
}
