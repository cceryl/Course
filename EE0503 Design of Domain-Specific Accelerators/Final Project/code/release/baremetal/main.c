#include <stdint.h>
#include <stdlib.h>

#include "image.h"
#include "kprintf.h"
#include "label.h"
#include "mlp.h"

#define NUM_IMAGES 10000
#define IMAGE_SIZE (28 * 28)

#define RUN_IMAGE_START 0
#define RUN_IMAGE_NUM   10

int main()
{
    unsigned int correct_predictions = 0;
    for (unsigned int i = RUN_IMAGE_START; i < RUN_IMAGE_START + RUN_IMAGE_NUM; ++i)
    {
        const int   *sample = image_data + i * IMAGE_SIZE;
        unsigned int class_index;
        run_mlp(sample, 1, &class_index);

        if (class_index == label_data[i])
            ++correct_predictions;

        printf("Sample %u: Predicted class: %u, Actual class: %u\n", i, class_index, label_data[i]);
    }

    printf("Correct predictions: %u/%u\n", correct_predictions, RUN_IMAGE_NUM);
    return 0;
}
