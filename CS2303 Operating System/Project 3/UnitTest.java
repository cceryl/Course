import java.util.Comparator;

public class UnitTest {
    public static void main(String[] args) {
        // Generate random array
        int size = 20;

        Integer[] randomArray = new Integer[size];
        for (int i = 0; i < size; i++)
            randomArray[i] = (int) (Math.random() * 100);

        Float[] randomFloatArray = new Float[size];
        for (int i = 0; i < size; i++)
            randomFloatArray[i] = (float) (Math.random() * 100);

        Comparator<Integer> integerComparator = (a, b) -> b - a;
        Comparator<Float> floatComparator = (a, b) -> (int) (b - a);

        // Test MergeSort
        System.out.println("MergeSort");

        MergeSort<Integer> mergeSort = new MergeSort<>();
        System.out.println("    Integer Array");

        mergeSort.sort(randomArray);
        for (int i = 0; i < size; i++)
            System.out.print(randomArray[i] + " ");
        System.out.println();

        mergeSort.sort(randomArray, integerComparator);
        for (int i = 0; i < size; i++)
            System.out.print(randomArray[i] + " ");
        System.out.println();

        MergeSort<Float> mergeSortFloat = new MergeSort<>();
        System.out.println("    Float Array");

        mergeSortFloat.sort(randomFloatArray);
        for (int i = 0; i < size; i++)
            System.out.print(String.format("%.2f", randomFloatArray[i]) + " ");
        System.out.println();

        mergeSortFloat.sort(randomFloatArray, floatComparator);
        for (int i = 0; i < size; i++)
            System.out.print(String.format("%.2f", randomFloatArray[i]) + " ");
        System.out.println();

        // Test QuickSort
        System.out.println("QuickSort");

        QuickSort<Integer> quickSort = new QuickSort<>();
        System.out.println("    Integer Array");

        quickSort.sort(randomArray);
        for (int i = 0; i < size; i++)
            System.out.print(randomArray[i] + " ");
        System.out.println();

        quickSort.sort(randomArray, integerComparator);
        for (int i = 0; i < size; i++)
            System.out.print(randomArray[i] + " ");
        System.out.println();

        QuickSort<Float> quickSortFloat = new QuickSort<>();
        System.out.println("    Float Array");

        quickSortFloat.sort(randomFloatArray);
        for (int i = 0; i < size; i++)
            System.out.print(String.format("%.2f", randomFloatArray[i]) + " ");
        System.out.println();

        quickSortFloat.sort(randomFloatArray, floatComparator);
        for (int i = 0; i < size; i++)
            System.out.print(String.format("%.2f", randomFloatArray[i]) + " ");
        System.out.println();

        System.out.println("Test complete");
    }
}