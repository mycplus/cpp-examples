// FragileBase.java - the same dependency on a base class's implementation.
import java.util.HashSet;
import java.util.List;
import java.util.Set;

public class FragileBase {
    static class Tags {
        private final Set<String> tags = new HashSet<>();
        void add(String tag) { tags.add(tag); }
        void addAll(List<String> list) { for (String t : list) add(t); }
        int size() { return tags.size(); }
    }

    static class CountingTags extends Tags {
        private int attempts = 0;
        @Override void add(String tag) { attempts++; super.add(tag); }
        @Override void addAll(List<String> list) { attempts += list.size(); super.addAll(list); }
        int attempts() { return attempts; }
    }

    public static void main(String[] args) {
        CountingTags t = new CountingTags();
        t.addAll(List.of("urgent", "billing", "eu"));
        System.out.println("tags stored: " + t.size() + ", attempts counted: " + t.attempts());
    }
}
