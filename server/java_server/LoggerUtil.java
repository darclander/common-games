package java_server;

import java.util.HashMap;
import java.util.Map;

public class LoggerUtil {
    private static final Map<String, Boolean> blacklist = new HashMap<>();


    private static boolean blacklistEnabled = false; // Set to false to disable entire blacklist
    static {
        blacklist.put("dddwafw", true);
        blacklist.put("PLAYER_UPDATE_POSITION", true);
        blacklist.put("PLAYER_NEW_POS", true);
    }


    public static void setBlacklistEnabled(boolean enabled) {
        blacklistEnabled = enabled;
    }

    public static void setBlacklistWordEnabled(String word, boolean enabled) {
        if (blacklist.containsKey(word)) {
            blacklist.put(word, enabled);
        }
    }

    public static void logMessage(String message) {
        if (blacklistEnabled) {
            boolean isBlacklisted = blacklist.entrySet().stream()
                .anyMatch(entry -> entry.getValue() && message.contains(entry.getKey()));
            if (!isBlacklisted) {
                System.out.println(message);
            }
        } else {
            System.out.println(message);
        }
    }
}