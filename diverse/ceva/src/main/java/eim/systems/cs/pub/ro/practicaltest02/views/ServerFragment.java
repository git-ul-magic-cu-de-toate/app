package eim.systems.cs.pub.ro.practicaltest02.views;

import android.app.Fragment;
import android.os.Bundle;
import android.util.Log;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.Button;

import org.apache.http.client.ClientProtocolException;
import org.apache.http.client.HttpClient;
import org.apache.http.client.ResponseHandler;
import org.apache.http.client.methods.HttpGet;
import org.apache.http.impl.client.BasicResponseHandler;
import org.apache.http.impl.client.DefaultHttpClient;
import org.jsoup.Jsoup;
import org.jsoup.nodes.Document;
import org.jsoup.nodes.Element;

import java.io.BufferedReader;
import java.io.IOException;
import java.io.PrintWriter;
import java.net.ServerSocket;
import java.net.Socket;

import eim.systems.cs.pub.ro.practicaltest02.R;
import eim.systems.cs.pub.ro.practicaltest02.general.Constants;
import eim.systems.cs.pub.ro.practicaltest02.general.Utilities;
import eim.systems.cs.pub.ro.practicaltest02.model.CityWeatherForecast;

public class ServerFragment extends Fragment {

    private Button startServerButton, stopServerButton;

    private class ButtonClickListener implements View.OnClickListener {

        @Override
        public void onClick(View v) {
            int id = v.getId();

            switch (id) {
                case Constants.SERVER_START:
                    serverThread = new ServerThread();
                    Log.v(Constants.TAG, "Starting server...");
                    serverThread.startServer();
                    break;
                case Constants.SERVER_STOP:
                    if (serverThread != null) {
                        Log.v(Constants.TAG, "Stopping server...");
                        serverThread.stopServer();
                    }
                    break;
            }
        }
    }

    private class CommunicationThread extends Thread {

        private Socket socket;

        public CommunicationThread(Socket socket) {
            this.socket = socket;
        }

        @Override
        public void run() {
            try {
                Log.v(Constants.TAG, "Server - Connection opened with " + socket.getInetAddress() + ":" + socket.getLocalPort());
                BufferedReader bufferedReader = Utilities.getReader(socket);
                PrintWriter printWriter = Utilities.getWriter(socket);
                String url = bufferedReader.readLine();
                URLContentTask urlContentTask = new URLContentTask();

                String result;
                if (url.contains(Constants.BAD))
                    result = Constants.RESTRICTED;
                else
                    result = urlContentTask.getResponse(url);
                Log.v(Constants.TAG, "Server - result = " + result);
                printWriter.println(result);

                socket.close();
                Log.v(Constants.TAG, "Connection closed");
            } catch (IOException ioException) {
                Log.e(Constants.TAG, "An exception has occurred: " + ioException.getMessage());
                if (Constants.DEBUG) {
                    ioException.printStackTrace();
                }
            }
        }
    }

    private ServerThread serverThread;
    private class ServerThread extends Thread {

        private boolean isRunning;

        private ServerSocket serverSocket;

        public void startServer() {
            isRunning = true;
            start();
            Log.v(Constants.TAG, "startServer() method invoked");
        }

        public void stopServer() {
            isRunning = false;
            try {
                serverSocket.close();
            } catch(IOException ioException) {
                Log.e(Constants.TAG, "An exception has occurred: " + ioException.getMessage());
                if (Constants.DEBUG) {
                    ioException.printStackTrace();
                }
            }
            Log.v(Constants.TAG, "stopServer() method invoked");
        }

        @Override
        public void run() {
            try {
                serverSocket = new ServerSocket(Constants.SERVER_PORT);
                Log.d(Constants.TAG, "Server Socket created");
                while (isRunning) {
                    Socket socket = serverSocket.accept();
                    if (socket != null) {
                        CommunicationThread communicationThread = new CommunicationThread(socket);
                        communicationThread.start();
                    }
                }
            } catch (IOException ioException) {
                Log.e(Constants.TAG, "An exception has occurred at ServerSocket: "+ioException.getMessage());
                if (Constants.DEBUG) {
                    ioException.printStackTrace();
                }
            }
        }
    }

    @Override
    public View onCreateView(LayoutInflater inflater, ViewGroup parent, Bundle state) {
        return inflater.inflate(R.layout.fragment_server, parent, false);
    }

    @Override
    public void onActivityCreated(Bundle state) {
        super.onActivityCreated(state);

        startServerButton = (Button)getActivity().findViewById(R.id.startServer);
        stopServerButton= (Button)getActivity().findViewById(R.id.stopServer);

        ButtonClickListener buttonClickListener = new ButtonClickListener();

        startServerButton.setOnClickListener(buttonClickListener);
        stopServerButton.setOnClickListener(buttonClickListener);
    }

    @Override
    public void onDestroy() {
        super.onDestroy();
        if (serverThread != null) {
            serverThread.stopServer();
        }
    }

    private class URLContentTask {

        private String getResponse(String... urls) {
            String response = null;

            CityWeatherForecast cityWeatherForecast = new CityWeatherForecast();
            HttpClient httpClient = new DefaultHttpClient();
            HttpGet httpXkcdGet = new HttpGet(Constants.HTTP + urls[0]);
            ResponseHandler<String> responseHandlerGet = new BasicResponseHandler();
            String pageSourceCode = null;

            Log.v(Constants.TAG, "Server - resolving pageSourceCode " + Constants.HTTP + urls[0]);
            try {
                pageSourceCode = httpClient.execute(httpXkcdGet, responseHandlerGet);
                Log.v(Constants.TAG, "Server - pageSourceCode obtained");
            } catch (ClientProtocolException clientProtocolException) {
                Log.e(Constants.TAG, clientProtocolException.getMessage());
                if (Constants.DEBUG) {
                    clientProtocolException.printStackTrace();
                }
            } catch (IOException ioException) {
                Log.e(Constants.TAG, ioException.getMessage());
                if (Constants.DEBUG) {
                    ioException.printStackTrace();
                }
            }
            if (pageSourceCode != null) {
                Document document = Jsoup.parse(pageSourceCode);
                Element htmlTag = document.child(0);
                response = htmlTag.html();
            }

            return response;
        }
    }
}
