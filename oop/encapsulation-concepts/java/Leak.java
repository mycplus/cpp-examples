// Leak.java - returning the private list hands it to the caller.
// LeakyTeam's limit of three members can be broken from outside; Team's cannot.
import java.util.ArrayList;
import java.util.Collections;
import java.util.List;

public final class Leak {
    static final int LIMIT = 3;

    static final class LeakyTeam {
        private final List<String> members = new ArrayList<>();

        void add(String name) {
            if (members.size() >= LIMIT) throw new IllegalStateException("team is full");
            members.add(name);
        }
        List<String> members() { return members; }  // the private list itself
    }

    static final class Team {
        private final List<String> members = new ArrayList<>();

        void add(String name) {
            if (members.size() >= LIMIT) throw new IllegalStateException("team is full");
            members.add(name);
        }
        List<String> members() { return Collections.unmodifiableList(members); }  // read-only view
    }

    public static void main(String[] args) {
        LeakyTeam leaky = new LeakyTeam();
        for (String n : List.of("Ana", "Ben", "Cy")) leaky.add(n);
        System.out.println("leaky: added Ana, Ben, Cy -> " + leaky.members().size() + " members");
        leaky.members().add("Dee");
        leaky.members().add("Eve");
        System.out.println("leaky: caller appended twice -> " + leaky.members().size()
                           + " members (limit is " + LIMIT + ")");

        Team team = new Team();
        for (String n : List.of("Ana", "Ben", "Cy")) team.add(n);
        System.out.println("safe: added Ana, Ben, Cy -> " + team.members().size() + " members");
        try {
            team.members().add("Dee");
        } catch (UnsupportedOperationException e) {
            System.out.println("safe: add rejected (UnsupportedOperationException), team still has "
                               + team.members().size() + " members");
        }
    }
}
