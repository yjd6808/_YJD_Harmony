import java.util.ArrayDeque;
import java.util.Deque;
import java.util.LinkedList;
import java.util.Stack;

public class Main {
    public static void main(String[] args) {
        LinkedList<Integer> list = new LinkedList<>();
        list.add(1);
        list.add(2);
        list.add(3);
        list.add(2);

        ArrayDeque<Integer> list3 = new ArrayDeque<>();
        Deque<Integer> list2 = new LinkedList<>();
        Stack<Integer> list3 = new Stack<>();
        list3.stream().collect();

        var f = list.stream();
        f.forEach((x) -> {
            System.out.println(x);
        });

        list.stream()
                .distinct()
                .forEach(x -> System.out.println((x)));
    }
}
