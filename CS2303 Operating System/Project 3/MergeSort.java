import java.util.Arrays;
import java.util.Comparator;
import java.util.concurrent.ForkJoinPool;
import java.util.concurrent.RecursiveAction;

public class MergeSort<DataType extends Comparable<DataType>> {

    public void sort(DataType[] array) {
        ForkJoinPool pool = new ForkJoinPool();
        pool.invoke(new MergeSortTask(array, 0, array.length - 1, Comparator.naturalOrder()));
        pool.shutdown();
    }

    public void sort(DataType[] array, Comparator<DataType> comparator) {
        ForkJoinPool pool = new ForkJoinPool();
        pool.invoke(new MergeSortTask(array, 0, array.length - 1, comparator));
        pool.shutdown();
    }

    private class MergeSortTask extends RecursiveAction {

        private DataType[] array;
        private int low;
        private int high;
        private Comparator<DataType> comparator;

        public MergeSortTask(DataType[] array, int low, int high, Comparator<DataType> comparator) {
            this.array = array;
            this.low = low;
            this.high = high;
            this.comparator = comparator;
        }

        @Override
        protected void compute() {
            if (high <= low)
                return;

            int mid = (low + high) / 2;

            MergeSortTask leftTask = new MergeSortTask(array, low, mid, comparator);
            MergeSortTask rightTask = new MergeSortTask(array, mid + 1, high, comparator);

            leftTask.fork();
            rightTask.fork();

            leftTask.join();
            rightTask.join();

            merge(array, low, mid, high, comparator);
        }

        private void merge(DataType[] array, int low, int mid, int high, Comparator<DataType> comparator) {
            DataType[] leftArray = Arrays.copyOfRange(array, low, mid + 1);
            DataType[] rightArray = Arrays.copyOfRange(array, mid + 1, high + 1);

            int i = 0;
            int j = 0;
            int k = low;

            while (i < leftArray.length && j < rightArray.length)
                if(comparator.compare(leftArray[i], rightArray[j]) <= 0)
                    array[k++] = leftArray[i++];
                else
                    array[k++] = rightArray[j++];
            while (i < leftArray.length)
                array[k++] = leftArray[i++];
            while(j < rightArray.length)
                array[k++] = rightArray[j++];
        }
    }
}