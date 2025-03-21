#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "image.h"
#include "label.h"
#include "mlp.h"

#define NUM_IMAGES 10000
#define IMAGE_SIZE (28 * 28)

#define RUN_IMAGE_START 0
#define RUN_IMAGE_NUM   10000
#define PRINT_RESULTS   0

int main()
{
    unsigned int correct_predictions = 0;
    for (unsigned int i = 0; i < NUM_IMAGES; ++i)
    {
        const int   *sample = image_data + i * IMAGE_SIZE;
        unsigned int class_index;
        run_mlp(sample, 1, &class_index);

        if (class_index == label_data[i])
            ++correct_predictions;

        if (PRINT_RESULTS)
            printf("Sample %u: Predicted class: %u, Actual class: %u\n", i, class_index, label_data[i]);
    }

    printf("Accuracy: %.2f%% in samples %d to %d", (correct_predictions / (float)RUN_IMAGE_NUM) * 100.0, RUN_IMAGE_START,
           RUN_IMAGE_START + RUN_IMAGE_NUM - 1);
    return 0;
}
