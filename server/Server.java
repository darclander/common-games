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


// Server class
public class Server {

    private static final int PORT = 12345;
    private static List<Socket> clientList = new ArrayList<>();
    private static ExecutorService executorService = Executors.newCachedThreadPool();
    private static List<Player> players = new ArrayList<>();
    private static int playerIDCounter = 0;

    public static void main(String[] args) {
        try {
            ServerSocket serverSocket = new ServerSocket(PORT);
            System.out.println("Server listening on port " + PORT);

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

    private static void handleClient(Socket clientSocket) {
        try {
            InputStream inputStream = clientSocket.getInputStream();
            OutputStream outputStream = clientSocket.getOutputStream();
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

                    case "BROADCAST_TEST": // Parameters: 0 = MESSAGE
                    // Send a message to all connected clients
                        System.out.println(params.get(0));
                        broadcast(params.get(0));
                        break;

                    case "NEW_PLAYER_JOINED": // Parameters: 0 = PLAYER_NAME
                        broadcast("NEW_PLAYER_JOINED;" + params.get(0));
                        break;

                    case "ADD_NEW_PLAYER": // Parameters: 0 = name, 1 = COLOR
                        Player newPlayer = new Player(playerIDCounter++, params.get(0), params.get(1), 0, 0); // 0 = pid, 1 = name, 2 = color, 3 = xPosition, 4 = yPosition
                        players.add(newPlayer);
                        outputStream.write(("NEW_PLAYER_RESPONSE;" + newPlayer.getPid() + ";" + newPlayer.getXPosition() + ";" + newPlayer.getYPosition()).getBytes()); // 0 = pid, 1 = xPosition, 2 = yPosition




                        broadcast("NEW_PLAYER;" + params.get(0) + ";" + params.get(1), clientSocket);

                        break;

                    case "PLAYER_MOVED": // Parameters: 0 = oldPos, 1 = newPos
                        // 
                        break;


                    case "TEST":
                        System.out.println(clientList); 
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
        private int xPosition;
        private int yPosition;

        public Player(int pid, String name, String color, int xPosition, int yPosition) {
            this.pid = pid;
            this.name = name;
            this.color = color;
            this.xPosition = xPosition;
            this.yPosition = yPosition;
        }

        public int getPid() {
            return pid;
        }

        public String getName() {
            return name;
        }

        public String getColor() {
            return color;
        }

        public int getXPosition() {
            return xPosition;
        }

        public int getYPosition() {
            return yPosition;
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
