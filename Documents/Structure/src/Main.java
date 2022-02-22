import java.lang.reflect.Array;
import java.time.Duration;
import java.time.Instant;
import java.util.*;
import java.util.stream.Collectors;
import java.util.stream.Stream;

public class Main {

    public static void main(String[] args) {
        for (int z = 0; z < 10; z++) {
            Random rand = new Random();
            ArrayList<Integer> list = new ArrayList<>();
            for (int i = 0; i < 1000000; i++) {
                list.add(rand.nextInt());
            }
            Instant s = Instant.now();
            list
                    .stream()
                    .sorted(Comparator.reverseOrder())
                    .forEach(x -> {
                        int c = x;
                    });
            Instant e = Instant.now();
            System.out.println(Duration.between(s, e));
        }

        int a = 40;
    }
}
