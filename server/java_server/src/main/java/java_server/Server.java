/*

javac -d . *.java
java java_server.Server

*/

package java_server;

import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.net.ServerSocket;
import java.net.Socket;
import java.net.SocketException;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.HashMap;
import java.util.List;
import java.util.Map;
import java.util.concurrent.ExecutorService;
import java.util.concurrent.Executors;
import java.util.Random;

// Server class
public class Server {

    private static final int PORT = 12345;
    private static List<Socket> clientList = new ArrayList<>();
    private static ExecutorService executorService = Executors.newCachedThreadPool();
    private static int playerIDCounter = 0;
    private static PlayingField playingField = new PlayingField(40, 30);

    public static void main(String[] args) {
        ServerSocket serverSocket = null;
        try {
            serverSocket = new ServerSocket(PORT);
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
        } finally {
            if (serverSocket != null && !serverSocket.isClosed()) {
                try {
                    serverSocket.close();
                } catch (IOException e) {
                    e.printStackTrace();
                }
            }
        }
    }

    
    private static void handleGame() {
        try {
            double elapsedTime;
            double lastTime = System.currentTimeMillis() / 1000.0; // convert to seconds
    
            // Initialize counters and intervals for each power-up
            List<Double> counters = Arrays.asList(0.0, 0.0, 0.0, 0.0, 0.0, 0.0);
            List<Double> intervals = Arrays.asList(3.0, 30.0, 45.0, 60.0, 75.0, 90.0); // intervals in seconds
            List<Runnable> actions = Arrays.asList(
                () -> {
                    playingField.spawnScore("berry", 1); // ADD_SCORE;type;magnitude;xPos;yPos
                },
                () -> { /* Add logic to spawn power-up 1 */ },
                () -> { /* Add logic to spawn power-up 2 */ },
                () -> { /* Add logic to spawn power-up 3 */ },
                () -> { /* Add logic to spawn power-up 4 */ },
                () -> { /* Add logic to spawn power-up 5 */ }
            );
    
            while (true) {
                double currentTime = System.currentTimeMillis() / 1000.0; // convert to seconds
                elapsedTime = currentTime - lastTime;
                lastTime = currentTime;
    
                for (int i = 0; i < counters.size(); i++) {
                    if (counters.get(i) >= intervals.get(i)) {
                        actions.get(i).run();
                        counters.set(i, 0.0);
                    }
                }
    
                // Update counters
                for (int i = 0; i < counters.size(); i++) {
                    counters.set(i, counters.get(i) + elapsedTime);
                }
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
                LoggerUtil.logMessage("Client " + clientSocket.getInetAddress() + ": " + receivedData);
                Command command = Command.parse(receivedData);

                String cmd = command.getCommand();
                List<String> params = command.getParameters();

                switch (cmd) {

                    case "NEW_PLAYER_JOINED": // NEW_PLAYER_JOINED;name             ?? What is this used for? 
                        msg = appendDelimitor("NEW_PLAYER_JOINED", params.get(0));
                        broadcast(msg, clientSocket);
                        break;
                    
                    case "PLAYER_UPDATE_POSITION": // PLAYER_UPDATE_POSITION;pid;xPos;yPos      // FIX NON EXISTENT SNAKE!!!!
                        int playerID = Integer.parseInt(params.get(0));
                        Player player = playingField.getPlayer(playerID);
                        int xPos = Integer.parseInt(params.get(1));
                        int yPos = Integer.parseInt(params.get(2));


                        if (xPos < 0 || xPos >= playingField.getWidth() || yPos < 0 || yPos >= playingField.getHeight()) {
                            System.out.println("Invalid position: " + xPos + ", " + yPos);
                            break;
                        }

                        String moveResponse = playingField.checkPosition(playerID, xPos, yPos);

                        if (player.getXPos() == xPos && player.getYPos() == yPos) {
                            break; // Ignore if the player doesn't move
                        }

                        if (moveResponse == "berry"){ // SCORE_COLLECTED;pid;type;magnitude;xPos;yPos
                            player.setLength(player.getLength() + 1);
                            msg = appendDelimitor("SCORE_COLLECTED", params.get(0), "berry", 1, Integer.parseInt(params.get(1)), Integer.parseInt(params.get(2))); // SCORE_COLLECTED;pid;type;amount;xPos;yPos
                            broadcast(msg);
                        } 
                        // else if (moveResponse == "outOfBounds") {
                        //     msg = appendDelimitor("MOVE_OUT_OF_BOUNDS", params.get(0), params.get(1), params.get(2)); // MOVE_OUT_OF_BOUNDS;pid;xPos;yPos     -- Parameters might be uncessary
                        //     // send(msg, outputStream);
                        //     break;
                        // }

                        player.move(xPos, yPos);

                        msg = appendDelimitor("PLAYER_NEW_POS", params.get(0), params.get(1), params.get(2));
                        broadcast(msg, clientSocket); // PLAYER_NEW_POS;pid;xPos;yPos
                        break;

                    case "ADD_NEW_PLAYER": // ADD_NEW_PLAYER;name;color

                        Player newPlayer = new Player(clientSocket, playerIDCounter++, params.get(0), params.get(1)); // 0 = pid, 1 = name, 2 = color
                        playingField.addPlayer(newPlayer);
                        playingField.spawnPlayer(newPlayer);

                        msg = appendDelimitor("NEW_PLAYER_RESPONSE", newPlayer.getPid(), newPlayer.getXPos(), newPlayer.getYPos(), playingField.getWidth(), playingField.getHeight());
                        send(msg, outputStream); // NEW_PLAYER_RESPONSE;pid;xPos;yPos;fieldWidth;fieldHeight

                        sendGameState(newPlayer, outputStream);

                        //msg = appendDelimitor("PLAYING_FIELD", playingField.getWidth(), playingField.getHeight(), playingField.encodeField());
// SEND PLAYING FIELD   send(msg, outputStream); // PLAYING_FIELD;fieldWidth;fieldHeight;|e|e|e|h/1|e|e|e|b/1|e|

                        msg = appendDelimitor("NEW_PLAYER", newPlayer.getPid(), newPlayer.getName(), newPlayer.getColor(), newPlayer.getXPos(), newPlayer.getYPos());
                        broadcast(msg, clientSocket); // NEW_PLAYER;pid;name;color;xPos;yPos
                        

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
                        // playingField.printPlayingField();
                        break;

                    default:
                        System.out.println("Unknown command: " + command.getCommand());
                }


            }
        } catch (IOException e) {
            if (e instanceof java.net.SocketException && e.getMessage().equals("Connection reset")) {
                // Client disconnected abruptly
                System.out.println("Client disconnected abruptly: " + clientSocket.getInetAddress());
                removeClient(clientSocket);
            } else {
                e.printStackTrace();
            }
        }
    }

    private static void removeClient(Socket clientSocket) {
        try {
            for (Player p : playingField.getPlayers().values()) {
                if (p.getPlayerSocket() == clientSocket) {
                    playingField.removePlayer(p);
                    break;
                }
            }
            clientList.remove(clientSocket);
            clientSocket.close();
        } catch (IOException e) {
            e.printStackTrace();
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
        int broadcastCount = 0;
    
        for (Socket clientSocket : clientList) {
            if (Arrays.asList(excludeClients).contains(clientSocket)) {
                continue;
            }
            try {
                OutputStream outputStream = clientSocket.getOutputStream();
                outputStream.write(message.getBytes());
                broadcastCount++;

            } catch (SocketException e) {
                System.out.println("Client disconnected abruptly: " + clientSocket.getInetAddress());
                clientList.remove(clientSocket);
                try {
                    clientSocket.close();
                } catch (IOException ex) {
                    ex.printStackTrace();
                }

            } catch (IOException e) {
                e.printStackTrace();
            }
        }
    
        LoggerUtil.logMessage("Broadcasted: '" + message + "' to " + broadcastCount + " clients.");
    }


    private static void sendGameState(Player newPlayer, OutputStream outputStream) throws IOException {
        String msg;
        // Send positions of all berries to the new player
        for (int y = 0; y < playingField.getHeight(); y++) {
            for (int x = 0; x < playingField.getWidth(); x++) {
                Square square = playingField.getField()[y][x];
                if (square.getType().equals("berry")) {
                    msg = appendDelimitor("BERRY_POSITION", x, y, square.getMagnitude());
                    send(msg, outputStream); // BERRY_POSITION;xPos;yPos;magnitude
                }
            }
        }

        // Send positions of all existing players to the new player
        for (Player p : playingField.getPlayers().values()) {
            if (p == newPlayer)
                continue;
            // Append head position
            StringBuilder playerInfo = new StringBuilder(
                    appendDelimitor("PLAYER_INFO", p.getPid(), p.getName(), p.getColor()));
            playerInfo.append(";").append(p.getXPos()).append(",").append(p.getYPos());

            // Append body segments positions
            for (BodySegment segment : p.getBody()) {
                playerInfo.append(";").append(segment.getPosition().getX()).append(",")
                        .append(segment.getPosition().getY());
            }

            msg = playerInfo.toString();
            send(msg, outputStream); // PLAYER_INFO;pid;name;color;headxPos,headyPos;31,12;31,13;
        }
    }



    public static class Player {
        private Socket playerSocket;
        private int pid;
        private String name;
        private String color;
        private Position headPosition;
        private int length;
        private List<BodySegment> body;

        public Player(Socket playerSocket, int pid, String name, String color) {
            this.playerSocket = playerSocket;
            this.pid = pid;
            this.name = name;
            this.color = color;
            this.length = 6;
            this.body = new ArrayList<>();
        }

        public Socket getPlayerSocket() { return playerSocket; }

        public int getPid() { return pid; }

        public String getName() { return name; }

        public String getColor() { return color; }

        public Position getHeadPosition() { return headPosition; }
        public int getXPos() { return headPosition.getX(); }
        public int getYPos() { return headPosition.getY(); }

        public void setHeadPos(int x, int y) { headPosition = new Position(x, y); }

        public int getLength() { return length; }

        public int setLength(int length) { return this.length = length; }

        public List<BodySegment> getBody() { return body; }

        public void move(int newX, int newY) {
            // Clear previous head position from the playing field
            playingField.getField()[headPosition.getY()][headPosition.getX()].clear();
        
            // Add new head position to the body
            body.add(0, new BodySegment(headPosition.getX(), headPosition.getY()));
            
            // Remove the last segment if the length is exceeded
            if (body.size() > length - 1) { // length - 1 because length includes the head
                BodySegment removedSegment = body.remove(body.size() - 1);
                // Clear the removed segment position from the playing field
                playingField.getField()[removedSegment.getPosition().getY()][removedSegment.getPosition().getX()].clear();
            }
            
            // Update head position
            headPosition = new Position(newX, newY);
        
            // Update the playing field with the new head position
            playingField.getField()[newY][newX].setType("head");
            playingField.getField()[newY][newX].setPlayerID(pid);
        
            // Update the playing field with the new body segments positions
            for (BodySegment segment : body) {
                playingField.getField()[segment.getPosition().getY()][segment.getPosition().getX()].setType("playerbody");
                playingField.getField()[segment.getPosition().getY()][segment.getPosition().getX()].setPlayerID(pid);
            }

        }
    }


    public static class BodySegment {
        private Position position;

        public BodySegment(int xPos, int yPos) {
            this.position = new Position(xPos, yPos);
        }

        public Position getPosition() { return position; }
    }


    public static class PlayingField {
        
        private int width;
        private int height;
        private Square[][] field;
        private Map<Integer, Player> players = new HashMap<>();

        public PlayingField(int width, int height) {
            this.width = width;
            this.height = height;
            this.field = new Square[height][width];
            this.players = new HashMap<>();

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

        public void spawnPlayer(Player player) {

            int xPos, yPos;
            boolean validPosition;
            int playerID = player.getPid();
        
            do {
                xPos = new Random().nextInt(width);
                yPos = new Random().nextInt(height);
                validPosition = field[yPos][xPos].getType().equals("empty");
            } while (!validPosition);
        
            // Place the player on the field
            field[yPos][xPos].setType("head");
            field[yPos][xPos].setPlayerID(playerID);
        
            player.setHeadPos(xPos, yPos);
        }

        public void addPlayer(Player player) {
            players.put(player.getPid(), player);
        }

        public void removePlayer(Player player) {
            players.remove(player.getPid());

            // Clear the head position
            Position headPosition = player.getHeadPosition();
            if (headPosition != null) {
                field[headPosition.getY()][headPosition.getX()].clear();
            }

            // Clear the body positions
            for (BodySegment segment : player.getBody()) {
                Position pos = segment.getPosition();
                field[pos.getY()][pos.getX()].clear();
            }

        }

        public Player getPlayer(int playerID) {
            return players.get(playerID);
        }

        public Map<Integer, Player> getPlayers() {
            return players;
        }

        public String encodeField() {
            StringBuilder sb = new StringBuilder();
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
                    } else if (!square.getType().equals("empty")) {
                        sb.append("?");
                    } else {
                        sb.append(".");
                    }
                    sb.append("|");
                }
                sb.append("\n");
            }
            return sb.toString();
        }

        public void printPlayingField() {
            for (int i = 0; i < height; i++) {
                for (int j = 0; j < width; j++) {
                    System.out.print(field[i][j].getType() + " ");
                }
                System.out.println();
            }
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

    public static class Position {
        private int x;
        private int y;

        public Position(int x, int y) {
            this.x = x;
            this.y = y;
        }

        @Override
        public String toString() {
            return "Position{" +
                    "x=" + x +
                    ", y=" + y +
                    '}';
        }

        public int getX() { return x; }

        public int getY() { return y; }
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
