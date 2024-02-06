import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.net.ServerSocket;
import java.net.Socket;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.List;
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
    private static List<Player> players = new ArrayList<>();
    private static int playerIDCounter = 0;
    private static PlayingField playingField = new PlayingField(20, 20);

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
            long ADD_SCORE_INTERVAL = 1 * 1000;
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
                        String moveResponse  = playingField.checkPosition(Integer.parseInt(params.get(0)), Integer.parseInt(params.get(1)), Integer.parseInt(params.get(2)));


                        if (moveResponse == "berry"){
                            msg = appendDelimitor("SCORE_COLLECTED", params.get(0), "berry", 1, Integer.parseInt(params.get(1)), Integer.parseInt(params.get(2))); // SCORE_COLLECTED;pid;type;amount;xPos;yPos
                            broadcast(msg);
                        } else if (moveResponse == "outOfBounds") {
                            msg = appendDelimitor("MOVE_OUT_OF_BOUNDS", params.get(0), params.get(1), params.get(2)); // MOVE_OUT_OF_BOUNDS;pid;xPos;yPos     -- Parameters might be uncessary
                            send(msg, outputStream);
                            break;
                        }

                        msg = appendDelimitor("PLAYER_NEW_POS", params.get(0), params.get(1), params.get(2));
                        broadcast(msg, clientSocket); // PLAYER_NEW_POS;pid;xPos;yPos
                        break;

                    case "ADD_NEW_PLAYER": // ADD_NEW_PLAYER;name;color
                        Player newPlayer = new Player(playerIDCounter++, params.get(0), params.get(1), 0, 0); // 0 = pid, 1 = name, 2 = color, 3 = xPosition, 4 = yPosition
                        players.add(newPlayer);


                        msg = appendDelimitor("NEW_PLAYER_RESPONSE", newPlayer.getPid(), newPlayer.getPid()*50, newPlayer.getYPos());
                        send(msg, outputStream); // NEW_PLAYER_RESPONSE;pid;xPos;yPos;fieldWidth;fieldHeight

                        msg = appendDelimitor("NEW_PLAYER", newPlayer.getPid(), (newPlayer.getPid() + 1)*50, (newPlayer.getPid() + 1));
                        broadcast(msg, clientSocket); // NEW_PLAYER;pid;xPos;yPos
                        
                        for (Player p : players) {
                            if(p == newPlayer) continue;
                            msg = appendDelimitor("PLAYER_INFO", p.getPid(), (p.getPid() + 1)*50, (p.getPid() + 1));
                            send(msg, outputStream); // PLAYER_INFO;pid;xPos;yPos
                        }

                        break;


                    // TEST CALLS
                    case "BROADCAST_TEST": // BROADCAST_TEST;message
                        System.out.println(params.get(0));
                        broadcast(params.get(0));
                        break;

                    default:
                        System.out.println("Unknown command: " + command.getCommand());
                }


            }
        } catch (IOException e) {
            if (e instanceof java.net.SocketException && e.getMessage().equals("Connection reset")) {
                // Client disconnected abruptly
                System.out.println("Client disconnected abruptly: " + clientSocket.getInetAddress());
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
        for (Socket clientSocket : clientList) {
            if (Arrays.asList(excludeClients).contains(clientSocket)) {
                continue;
            }
            try {
                OutputStream outputStream = clientSocket.getOutputStream();
                outputStream.write(message.getBytes());
            } catch (IOException e) {
                e.printStackTrace();
            }
        }
        System.out.println("Broadcasted: '" + message + "' to " + clientList.size() + " clients.");
    }


    public static class Player {
        private int pid;
        private String name;
        private String color;
        private int headXPos;
        private int headYPos;
        private int length;
        private List<BodySegment> body;

        public Player(int pid, String name, String color, int headXPos, int headYPos) {
            this.pid = pid;
            this.name = name;
            this.color = color;
            this.headXPos = headXPos;
            this.headYPos = headYPos;
            this.length = 1; // NOT IMPLEMENTED
            this.body = new ArrayList<>(); // NOT IMPLEMENTED
            this.body.add(new BodySegment(headXPos, headYPos)); // NOT IMPLEMENTED
        }

        public int getPid() { return pid; }

        public String getName() { return name; }

        public String getColor() { return color; }

        public int getxPos() { return headXPos; }

        public int getYPos() { return headYPos; }

        public int getLength() { return length; }

        public List<BodySegment> getBody() { return body; }
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

        // Spawns a score item on a random unoccupied square
        public void spawnScore(String type, int magnitude) { 
            int xPos;
            int yPos;
            do {
                xPos = new Random().nextInt(width);
                yPos = new Random().nextInt(height);
            } while ( !field[yPos][xPos].getType().equals("empty") );

            field[yPos][xPos].setType(type);
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

        public Square() {
            this.type = "empty";
        }

        public String getType() {
            return type;
        }

        public void setType(String type) {
            this.type = type;
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
