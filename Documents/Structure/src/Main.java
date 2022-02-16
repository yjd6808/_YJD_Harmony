import java.lang.reflect.Array;
import java.util.*;
import java.util.stream.Stream;

public class Main {
    public static void main(String[] args) {
        HashMap<Integer, Integer> i = new HashMap<>();
        ArrayList<Integer> z2 = new ArrayList<>();
        LinkedList<Integer> z = new LinkedList<>();

        z.add(5);
        z.add(4);
        z.add(3);
        z.add(2);


        var ggg = z.stream()
                .sorted()
                .toArray();
        i.put(1, 1);
        i.put(2, 2);
        i.put(3, 3);
        i.put(4, 4);
        Stream<Integer> gg = i.values().stream();
        System.out.printf("%s\n", gg.getClass().getName());
        System.out.printf("%s\n", gg.sorted().getClass().getName());
        System.out.printf("%s\n", z.stream().getClass().getName());
        System.out.printf("%s\n", z.stream().sorted().getClass().getName());
        System.out.printf("%s\n", z2.stream().getClass().getName());
        System.out.printf("%s\n", z2.stream().sorted().getClass().getName());

        var f= i.keySet();
        var g = i.values();
    }
}
