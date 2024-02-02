import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.net.Socket;
import java.util.Scanner;

public class ServerTester {
    public static void main(String[] args) {
        try (Socket socket = new Socket("127.0.0.1", 12345)){
            Thread receiveThread = new Thread(() -> {
                try {
                    InputStream inputStream = socket.getInputStream();
                    byte[] buffer = new byte[1024];
                    int bytesRead;
                    while ((bytesRead = inputStream.read(buffer)) != -1) {
                        String message = new String(buffer, 0, bytesRead);
                        System.out.println("Server: " + message);
                    }
                } catch (IOException e) {
                    e.printStackTrace();
                }
            });

            Thread sendThread = new Thread(() -> {
                try (
                    OutputStream outputStream = socket.getOutputStream();
                    Scanner scanner = new Scanner(System.in);
                ){
                    String message;
                    while (true) {
                        message = scanner.nextLine();
                        outputStream.write((message).getBytes());
                        outputStream.flush();
                    }
                } catch (IOException e) {
                    e.printStackTrace();
                }
            });

            receiveThread.start();
            sendThread.start();

        } catch (IOException e) {
            e.printStackTrace();
        }
    }
}