// Lifetime.java - Java has no destructors; try-with-resources calls close()
// when the block ends, which gives the same deterministic cleanup.
public class Lifetime {
    static final class LogFile implements AutoCloseable {
        private final String name;
        LogFile(String name) { this.name = name; System.out.println("open " + name); }
        void write(String line) { System.out.println("write " + line); }
        @Override public void close() { System.out.println("close " + name); }
    }

    public static void main(String[] args) {
        try (LogFile log = new LogFile("app.log")) {
            log.write("started");
        }                                          // close() runs here
        System.out.println("after the block");
    }
}
