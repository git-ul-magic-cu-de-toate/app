package eim.systems.cs.pub.ro.practicaltest02.views;

import android.os.AsyncTask;
import android.os.Bundle;
import android.app.Fragment;
import android.util.Log;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.Button;
import android.widget.EditText;
import android.widget.TextView;

import java.io.BufferedReader;
import java.io.IOException;
import java.io.PrintWriter;
import java.net.Socket;

import eim.systems.cs.pub.ro.practicaltest02.R;
import eim.systems.cs.pub.ro.practicaltest02.general.Constants;
import eim.systems.cs.pub.ro.practicaltest02.general.Utilities;

public class ClientFragment extends Fragment {

    private EditText serverAddressEditText, serverPortEditText, cityEditText;
    private TextView serverMessageTextView;
    private Button displayMessageButton;

    private class ClientAsyncTask extends AsyncTask<String, String, Void> {

        @Override
        protected Void doInBackground(String... params) {
            Socket socket = null;
            try {
                String serverAddress = params[0];
                int serverPort = Integer.parseInt(params[1]);
                String url = params[2];
                socket = new Socket(serverAddress, serverPort);
                if (socket == null) {
                    return null;
                }
                Log.v(Constants.TAG, "Client - Connection opened with " + socket.getInetAddress() + ":" + socket.getLocalPort());
                Log.v(Constants.TAG, "Client - url = " + url);
                BufferedReader bufferedReader = Utilities.getReader(socket);
                PrintWriter printWriter = Utilities.getWriter(socket);
                String currentLine;
                printWriter.println(url);
                while ((currentLine = bufferedReader.readLine()) != null) {
                    publishProgress(currentLine);
                }
            } catch (IOException ioException) {
                Log.e(Constants.TAG, "An exception has occurred: " + ioException.getMessage());
                if (Constants.DEBUG) {
                    ioException.printStackTrace();
                }
            } finally {
                try {
                    if (socket != null) {
                        socket.close();
                    }
                    Log.v(Constants.TAG, "Connection closed");
                } catch (IOException ioException) {
                    Log.e(Constants.TAG, "An exception has occurred: " + ioException.getMessage());
                    if (Constants.DEBUG) {
                        ioException.printStackTrace();
                    }
                }
            }
            return null;
        }

        @Override
        protected void onPreExecute() {
            serverMessageTextView.setText("");
        }

        @Override
        protected void onProgressUpdate(String... progress) {
            serverMessageTextView.append(progress[0] + "\n");
        }

        @Override
        protected void onPostExecute(Void result) { }
    }

    @Override
    public View onCreateView(LayoutInflater inflater, ViewGroup parent, Bundle state) {
        return inflater.inflate(R.layout.fragment_client, parent, false);
    }

    @Override
    public void onActivityCreated(Bundle state) {
        super.onActivityCreated(state);

        serverAddressEditText = (EditText)getActivity().findViewById(R.id.server_address_edit_text);
        serverPortEditText = (EditText)getActivity().findViewById(R.id.server_port_edit_text);
        serverMessageTextView = (TextView)getActivity().findViewById(R.id.server_message_text_view);
        cityEditText = (EditText)getActivity().findViewById(R.id.city_edit_text);

        displayMessageButton = (Button)getActivity().findViewById(R.id.display_message_button);
        displayMessageButton.setOnClickListener(new Button.OnClickListener() {
            @Override
            public void onClick(View view) {
                ClientAsyncTask clientAsyncTask = new ClientAsyncTask();
                clientAsyncTask.execute(serverAddressEditText.getText().toString(),
                        serverPortEditText.getText().toString(),
                        cityEditText.getText().toString());
            }
        });

    }

}
