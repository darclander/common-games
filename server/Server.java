import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.net.ServerSocket;
import java.net.Socket;
import java.util.ArrayList;
import java.util.List;
import java.util.concurrent.ExecutorService;
import java.util.concurrent.Executors;

public class Server {

    private static final int PORT = 12345;
    private static List<Socket> clientList = new ArrayList<>();
    private static ExecutorService executorService = Executors.newCachedThreadPool();

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

                // Process the received data (you can replace this with your logic)
                String receivedData = new String(buffer, 0, bytesRead);
                System.out.println("Received from client " + clientSocket.getInetAddress() + ": " + receivedData);

                if (receivedData.equals("BROADCAST_TEST")) {
                    broadcast("Broadcast message from server");
                }

                if (receivedData.equals("NEW_PLAYER_JOINED")) {
                    //
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

    private static void broadcast(String message) {
        for (Socket clientSocket : clientList) {
            try {
                OutputStream outputStream = clientSocket.getOutputStream();
                outputStream.write(message.getBytes());
            } catch (IOException e) {
                e.printStackTrace();
            }
        }
        System.out.println("Broadcasted: '" + message + "' to " + clientList.size() + " clients.");
    }
}
