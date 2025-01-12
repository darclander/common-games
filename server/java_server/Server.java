import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.net.ServerSocket;
import java.net.Socket;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.HashMap;
import java.util.List;
import java.util.Map;
import java.util.concurrent.ExecutorService;
import java.util.concurrent.Executors;
import java.util.Random;
import java.util.Timer;
import java.util.TimerTask;

// Server class
public class Server {

    private static final int PORT = 12345;
    private static List<Socket> clientList = new ArrayList<>();
    private static ExecutorService executorService = Executors.newCachedThreadPool();
    private static Map<Integer, Player> players = new HashMap<>();
    private static int playerIDCounter = 0;
    private static PlayingField playingField = new PlayingField(40, 30);

    public static void main(String[] args) {
        try {
            ServerSocket serverSocket = new ServerSocket(PORT);
            System.out.println("Server listening on port " + PORT);

            // Start the game loop
            executorService.execute(() -> handleGame());

            while (true) {
                Socket clientSocket = serverSocket.accept();
                System.out.println("Client connected: " + clientSocket.getInetAddress());

                // Add the client socket to the list
                clientList.add(clientSocket);

                // Handle client communication in a separate thread
                executorService.execute(() -> handleClient(clientSocket));
            }
        } catch (IOException e) {
            e.printStackTrace();
        }
    }

    
    private static void handleGame(){

        try {
            String msg;
            
            long elapsedTime;
            long startTime;
            long ADD_SCORE_COUNTER = 0;
            long ADD_SCORE_INTERVAL = 5 * 1000;
            long counter2 = 0;
            while (true) {
                startTime = System.currentTimeMillis();
                

                if(ADD_SCORE_COUNTER > ADD_SCORE_INTERVAL) {
                    playingField.spawnScore("berry", 1); // ADD_SCORE;type;magnitude;xPos;yPos
                    ADD_SCORE_COUNTER = 0;
                }

                if(counter2 > 30 * 1000) {
                    counter2 = 0;
                    // addPowerUp();
                }

                
                Thread.sleep(250);
                elapsedTime = (System.currentTimeMillis() - startTime);
                
                ADD_SCORE_COUNTER += elapsedTime;
                counter2 += elapsedTime;
            }
            
        } catch (Exception e) {
            e.printStackTrace();
        }
    }

    
    private static void handleClient(Socket clientSocket) {
        try {
            InputStream inputStream = clientSocket.getInputStream();
            OutputStream outputStream = clientSocket.getOutputStream();
            String msg;
            byte[] buffer = new byte[1024];


            while (true) {
                // Wait for data from the client
                int bytesRead = inputStream.read(buffer);
                if (bytesRead == -1) {
                    // Client disconnected
                    System.out.println("Client disconnected: " + clientSocket.getInetAddress());
                    clientList.remove(clientSocket);
                    break;
                }

                // Process the received data and parse it into a command
                String receivedData = new String(buffer, 0, bytesRead);
                System.out.println("Client " + clientSocket.getInetAddress() + ": " + receivedData);
                Command command = Command.parse(receivedData);

                String cmd = command.getCommand();
                List<String> params = command.getParameters();

                switch (cmd) {

                    case "NEW_PLAYER_JOINED": // NEW_PLAYER_JOINED;name
                        msg = appendDelimitor("NEW_PLAYER_JOINED", params.get(0));
                        broadcast(msg, clientSocket);
                        break;
                    
                    case "PLAYER_UPDATE_POSITION": // PLAYER_UPDATE_POSITION;pid;xPos;yPos
                        int playerID = Integer.parseInt(params.get(0));
                        Player player = players.get(playerID);
                        int xPos = Integer.parseInt(params.get(1));
                        int yPos = Integer.parseInt(params.get(2));
                        Square square = playingField.getField()[yPos][xPos];

                        String moveResponse = playingField.checkPosition(playerID, xPos, yPos);

                        if (player.getxPos() == xPos && player.getYPos() == yPos) {
                            return; // Ignore if the player doesn't move
                        }

                        if (moveResponse == "berry"){// SCORE_COLLECTED;pid;type;amount;xPos;yPos
                            msg = appendDelimitor("SCORE_COLLECTED", params.get(0), "berry", 1, Integer.parseInt(params.get(1)), Integer.parseInt(params.get(2))); // SCORE_COLLECTED;pid;type;amount;xPos;yPos
                            broadcast(msg);
                        } 
                        // else if (moveResponse == "outOfBounds") {
                        //     msg = appendDelimitor("MOVE_OUT_OF_BOUNDS", params.get(0), params.get(1), params.get(2)); // MOVE_OUT_OF_BOUNDS;pid;xPos;yPos     -- Parameters might be uncessary
                        //     // send(msg, outputStream);
                        //     break;
                        // }

                        if (player != null) {
                            // Remove old position
                            for (BodySegment segment : player.getBody()) {
                                playingField.getField()[segment.getyPos()][segment.getxPos()].clear();
                            }
                            player.move(xPos, yPos);
                        
                            // New head position
                            for (BodySegment segment : player.getBody()) {
                                playingField.getField()[segment.getyPos()][segment.getxPos()].setType("playerbody");
                                playingField.getField()[segment.getyPos()][segment.getxPos()].setPlayerID(playerID);
                            }
                            Square headSquare = playingField.getField()[player.getYPos()][player.getxPos()];
                            headSquare.setType("head");
                            headSquare.setPlayerID(playerID);
                        }

                        msg = appendDelimitor("PLAYER_NEW_POS", params.get(0), params.get(1), params.get(2));
                        broadcast(msg, clientSocket); // PLAYER_NEW_POS;pid;xPos;yPos
                        System.out.println("Player " + playerID + " moved to: " + xPos + ", " + yPos);
                        break;

                    case "ADD_NEW_PLAYER": // ADD_NEW_PLAYER;name;color
                        Player newPlayer = new Player(clientSocket, playerIDCounter++, params.get(0), params.get(1), 0, 0); // 0 = pid, 1 = name, 2 = color, 3 = xPosition, 4 = yPosition
                        players.put(newPlayer.getPid(), newPlayer);


                        msg = appendDelimitor("NEW_PLAYER_RESPONSE", newPlayer.getPid(), newPlayer.getxPos(), newPlayer.getYPos(), playingField.getWidth(), playingField.getHeight());
                        send(msg, outputStream); // NEW_PLAYER_RESPONSE;pid;xPos;yPos;fieldWidth;fieldHeight

                        //msg = appendDelimitor("PLAYING_FIELD", playingField.getWidth(), playingField.getHeight(), playingField.encodeField());
// SEND PLAYING FIELD   send(msg, outputStream); // PLAYING_FIELD;fieldWidth;fieldHeight;|e|e|e|h/1|e|e|e|b/1|e|

                        msg = appendDelimitor("NEW_PLAYER", newPlayer.getPid(), newPlayer.getName(), newPlayer.getColor(), newPlayer.getxPos(), newPlayer.getYPos());
                        broadcast(msg, clientSocket); // NEW_PLAYER;pid;name;color;xPos;yPos
                        
                        for (Player p : players.values()) {
                            if (p == newPlayer) continue;
                            msg = appendDelimitor("PLAYER_INFO", p.getPid(), p.getName(), p.getColor(), (p.getPid() + 1) * 50, (p.getPid() + 1)); // Merge with NEW_PLAYER????
                            send(msg, outputStream); // PLAYER_INFO;pid;name;color;xPos;yPos
                        }

                        break;


                    // TEST CALLS
                    case "BROADCAST_TEST": // BROADCAST_TEST;message
                        System.out.println(params.get(0));
                        broadcast(params.get(0));
                        break;

                    case "BERRY_TEST": // BERRY_TEST;(xPos;yPos)
                        if (params.size() >= 2) {
                            playingField.spawnScore("berry", 1, Integer.parseInt(params.get(0)), Integer.parseInt(params.get(1)));
                        } else {
                            playingField.spawnScore("berry", 1);
                        }
                        break;

                    case "XY": // XY;x;y
                        System.out.println("X:" + params.get(0) + " Y:" + params.get(1) + " = " + playingField.getField()[Integer.parseInt(params.get(1))][Integer.parseInt(params.get(0))].getType());
                        break;

                    case "TEST":
                        System.out.println("Test command received");
                        System.out.println(playingField.encodeField());
                        break;

                    default:
                        System.out.println("Unknown command: " + command.getCommand());
                }


            }
        } catch (IOException e) {
            if (e instanceof java.net.SocketException && e.getMessage().equals("Connection reset")) {
                // Client disconnected abruptly
                System.out.println("Client disconnected abruptly: " + clientSocket.getInetAddress());
                for (Player p : players.values()) {
                    if (p.getPlayerSocket() == clientSocket) {
                        players.remove(p.getPid());
                        break;
                    }
                }
                clientList.remove(clientSocket);
            } else {
                e.printStackTrace();
            }
        }
    }


    private static void send(String message, OutputStream outputStream) {
        try {
            System.out.println(("Sending: ") + message);
            outputStream.write(message.getBytes());
        } catch (IOException e) {
                e.printStackTrace();
        }

    }


    private static String appendDelimitor(Object... parameters) {
        StringBuilder sb = new StringBuilder();
        for (Object parameter : parameters) {
            sb.append(parameter).append(";");
        }
        // Remove the last semicolon
        if (sb.length() > 0) {
            sb.setLength(sb.length() - 1);
        }
        return sb.toString();
    }


    private static void broadcast(String message, Socket... excludeClients) {
        List<String> blacklist = Arrays.asList("dddwafw", "blacklisted_word2");
        boolean isBlacklisted = blacklist.stream().anyMatch(message::contains);
        int broadcastCount = 0;
    
        for (Socket clientSocket : clientList) {
            if (Arrays.asList(excludeClients).contains(clientSocket)) {
                continue;
            }
            try {
                OutputStream outputStream = clientSocket.getOutputStream();
                outputStream.write(message.getBytes());
                broadcastCount++;
            } catch (IOException e) {
                e.printStackTrace();
            }
        }
    
        if (!isBlacklisted) {
            System.out.println("Broadcasted: '" + message + "' to " + broadcastCount + " clients.");
        }
    }


    public static class Player {
        private Socket playerSocket;
        private int pid;
        private String name;
        private String color;
        private int headXPos;
        private int headYPos;
        private int length;
        private List<BodySegment> body;

        public Player(Socket playerSocket, int pid, String name, String color, int headXPos, int headYPos) {
            this.playerSocket = playerSocket;
            this.pid = pid;
            this.name = name;
            this.color = color;
            this.headXPos = headXPos;
            this.headYPos = headYPos;
            this.length = 3; // NOT IMPLEMENTED
            this.body = new ArrayList<>(); // NOT IMPLEMENTED
            this.body.add(new BodySegment(headXPos, headYPos)); // NOT IMPLEMENTED
        }

        public Socket getPlayerSocket() { return playerSocket; }

        public int getPid() { return pid; }

        public String getName() { return name; }

        public String getColor() { return color; }

        public int getxPos() { return headXPos; }

        public int getYPos() { return headYPos; }

        public int getLength() { return length; }

        public List<BodySegment> getBody() { return body; }

        public void move(int newX, int newY) {
            // Add new head position
            body.add(0, new BodySegment(newX, newY));
            // Remove the last segment if the length is exceeded
            if (body.size() > length) {
                body.remove(body.size() - 1);
            }
            // Update head position
            headXPos = newX;
            headYPos = newY;
        }
    }


    public static class BodySegment {
        private int xPos;
        private int yPos;

        public BodySegment(int xPos, int yPos) {
            this.xPos = xPos;
            this.yPos = yPos;
        }

        public int getxPos() {
            return xPos;
        }

        public int getyPos() {
            return yPos;
        }
    }


    public static class PlayingField {
        
        private int width;
        private int height;
        private Square[][] field;

        public PlayingField(int width, int height) {
            this.width = width;
            this.height = height;
            this.field = new Square[height][width];

            for (int i = 0; i < height; i++) {
                for (int j = 0; j < width; j++) {
                    field[i][j] = new Square();
                }
            }
        }

        public Square[][] getField() {
            return field;
        }

        public int getWidth() {
            return width;
        }

        public int getHeight() {
            return height;
        }

        public String encodeField() {
            StringBuilder sb = new StringBuilder();
            sb.append("|");
            for (int i = 0; i < height; i++) {
                for (int j = 0; j < width; j++) {
                    Square square = field[i][j];
                    if (square.getType().equals("head")) {
                        sb.append("H");
                        sb.append(square.getPlayerID());
                    } else if (square.getType().equals("playerbody")) {
                        sb.append("P");
                        sb.append(square.getPlayerID());
                    } else if (square.getType().equals("berry")) {
                        sb.append("B");
                        sb.append(square.getMagnitude());
                    } else {
                        sb.append(".");
                    }
                    sb.append("|");
                }            }
            return sb.toString();
        }

        // Spawns a score item on a random unoccupied square
        public void spawnScore(String type, int magnitude) { 
            int xPos;
            int yPos;
            do {
                xPos = new Random().nextInt(width);
                yPos = new Random().nextInt(height);
            } while ( !field[yPos][xPos].getType().equals("empty") );

            field[yPos][xPos].setType(type);
            field[yPos][xPos].setMagnitude(magnitude);
            String msg = appendDelimitor("ADD_SCORE", type, magnitude, xPos, yPos);
            broadcast(msg);
            System.out.println(type + " spawned at: " + xPos + ", " + yPos);
        }

        // @Overload    Spawns a score item on a specific square
        public void spawnScore(String type, int magnitude, int xPos, int yPos) { 
            field[yPos][xPos].setType(type);
            field[yPos][xPos].setMagnitude(magnitude);
            String msg = appendDelimitor("ADD_SCORE", type, magnitude, xPos, yPos);
            broadcast(msg);
        }



        public String checkPosition(int playerID, int x, int y) {
            if(x < 0 || x >= width || y < 0 || y >= height) {
                return "outOfBounds";
            }
    
            if(field[y][x].getType().equals("berry")) {
                field[y][x].setType("empty");
                return "berry";
            }
    
            return null;
        }

    }


    public static class Square {
        private String type;
        private int magnitude;
        private int playerID;

        public Square() {
            this.type = "empty";
        }

        public void clear() {
            this.type = "empty";
            this.magnitude = 0;
            this.playerID = 0;
        }

        public String getType() {
            return type;
        }

        public void setType(String type) {
            this.type = type;
        }

        public int getMagnitude() {
            return magnitude;
        }

        public void setMagnitude(int magnitude) {
            this.magnitude = magnitude;
        }

        public int getPlayerID() {
            return playerID;
        }

        public void setPlayerID(int playerID) {
            this.playerID = playerID;
        }
    }


    public static class Command {
        private String command;
        private List<String> parameters;

        private Command(String command, List<String> parameters) {
            this.command = command;
            this.parameters = parameters;
        }

        public String getCommand() {
            return command;
        }

        public List<String> getParameters() {
            return parameters;
        }

        public static Command parse(String receivedData) {
            String[] parts = receivedData.split(";");
            String command = parts[0];
            List<String> parameters = new ArrayList<>();
            if (parts.length > 1) {
                parameters = Arrays.asList(Arrays.copyOfRange(parts, 1, parts.length));
            }
            return new Command(command, parameters);
        }
    }
}
