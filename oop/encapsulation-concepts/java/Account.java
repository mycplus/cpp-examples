// Account.java - encapsulation: the balance can change only through
// deposit() and withdraw(), which never let it go below zero.
import java.util.ArrayList;
import java.util.Collections;
import java.util.List;

public final class Account {
    private final String owner;
    private long balance = 0;
    private final List<Long> history = new ArrayList<>();

    public Account(String owner, long opening) {
        this.owner = owner;
        deposit(opening);
    }

    public void deposit(long amount) {
        requirePositive(amount);
        balance += amount;
        history.add(amount);
    }

    public void withdraw(long amount) {
        requirePositive(amount);
        if (amount > balance) {
            throw new IllegalArgumentException(
                "insufficient funds: balance " + balance + ", requested " + amount);
        }
        balance -= amount;
        history.add(-amount);
    }

    public String owner() { return owner; }
    public long balance() { return balance; }
    public List<Long> history() { return Collections.unmodifiableList(history); }  // read-only view

    private static void requirePositive(long amount) {
        if (amount <= 0) {
            throw new IllegalArgumentException("amount must be positive: " + amount);
        }
    }

    public static void main(String[] args) {
        Account acct = new Account("Ada", 100);
        System.out.println(acct.owner() + " opened with " + acct.balance());

        acct.deposit(50);
        System.out.println("deposit 50 -> balance " + acct.balance());
        acct.withdraw(30);
        System.out.println("withdraw 30 -> balance " + acct.balance());

        for (long amount : new long[] {500, -5}) {
            try {
                acct.withdraw(amount);
            } catch (IllegalArgumentException e) {
                System.out.println("error: " + e.getMessage());
            }
        }
        System.out.println("balance " + acct.balance());

        StringBuilder line = new StringBuilder("history:");
        for (long t : acct.history()) {
            line.append(' ').append(t > 0 ? "+" : "").append(t);
        }
        System.out.println(line);
    }
}
