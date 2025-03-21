import java.util.Comparator;
import java.util.concurrent.ForkJoinPool;
import java.util.concurrent.RecursiveAction;

public class QuickSort<DataType extends Comparable<DataType>> {

    public void sort(DataType[] array) {
        ForkJoinPool pool = new ForkJoinPool();
        pool.invoke(new QuickSortTask(array, 0, array.length - 1, Comparator.naturalOrder()));
        pool.shutdown();
    }

    public void sort(DataType[] array, Comparator<DataType> comparator) {
        ForkJoinPool pool = new ForkJoinPool();
        pool.invoke(new QuickSortTask(array, 0, array.length - 1, comparator));
        pool.shutdown();
    }

    private class QuickSortTask extends RecursiveAction {
        private final DataType[] array;
        private final int low;
        private final int high;
        private final Comparator<DataType> comparator;

        public QuickSortTask(DataType[] array, int low, int high, Comparator<DataType> comparator) {
            this.array = array;
            this.low = low;
            this.high = high;
            this.comparator = comparator;
        }

        @Override
        protected void compute() {
            if (high <= low)
                return;

            int pivot = partition(array, low, high, comparator);

            QuickSortTask leftTask = new QuickSortTask(array, low, pivot - 1, comparator);
            QuickSortTask rightTask = new QuickSortTask(array, pivot + 1, high, comparator);

            leftTask.fork();
            rightTask.fork();

            leftTask.join();
            rightTask.join();
        }

        private int partition(DataType[] array, int low, int high, Comparator<DataType> comparator) {
            DataType pivot = array[high];
            int i = low;
    
            for (int j = low; j < high; j++) {
                if (comparator.compare(array[j], pivot) <= 0) {
                    swap(array, i, j);
                    i++;
                }
            }
    
            swap(array, i, high);
    
            return i;
        }
    
        private void swap(DataType[] array, int i, int j) {
            DataType tmp = array[i];
            array[i] = array[j];
            array[j] = tmp;
        }
    
    }
}