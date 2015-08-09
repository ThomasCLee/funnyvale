package com.hackarobot.www.hackabotnanocontroller;

import android.bluetooth.BluetoothAdapter;
import android.bluetooth.BluetoothSocket;
import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.graphics.Color;
import android.os.Bundle;
import android.os.Handler;
import android.support.v7.app.ActionBarActivity;
import android.view.Menu;
import android.view.MenuItem;
import android.bluetooth.BluetoothAdapter;
import android.bluetooth.BluetoothDevice;
import android.view.View;
import android.widget.ArrayAdapter;
import android.widget.Button;
import android.widget.ListView;
import android.widget.TextView;
import android.widget.Toast;
import android.view.View.OnClickListener;
//import android.content.IntentFilter;

import java.io.BufferedInputStream;
import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStream;
import java.io.InputStreamReader;
import java.io.OutputStream;
import java.io.StringWriter;
import java.util.Set;
import java.util.UUID;

public class MainActivity extends ActionBarActivity {

    // example: http://examples.javacodegeeks.com/android/core/bluetooth/bluetoothadapter/android-bluetooth-example/
    // another one: http://knowledge-cess.com/arduino-android-interfacing-example-code-and-troubleshooting/

    private BluetoothAdapter btAdapter = null;
    private Button leftButton;
    private Button rightButton;
    private Button forwardButton;
    private Button backwardButton;
    private Button connectButton;
    private Button scanButton;
    private TextView textStatus;
    private final static int REQUEST_ENABLE_BT = 1;
    private ListView myListView;
    private ArrayAdapter<String> myArrayAdapter;
    // UUID is used as a key to connect to the device
    private static final UUID MY_UUID = UUID.fromString("00001101-0000-1000-8000-00805F9B34FB");
    // address is the MAC address of the other device
    private String deviceAddress=null;
    private static String deviceName = "HC-06";
    private BluetoothDevice btDevice;
    private BluetoothSocket btSocket;
    private OutputStream outStream;
    private InputStream inStream;
    private Thread btThread=null;
    private Runnable runnable=null;
    private Handler handler;
    private String handlerMsg;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        handler = new Handler();

        // get the buttons and text views
        leftButton = (Button)findViewById(R.id.buttonLeft);
        rightButton= (Button)findViewById(R.id.buttonRight);
        forwardButton = (Button)findViewById(R.id.buttonForward);
        backwardButton = (Button)findViewById(R.id.buttonBackward);
        connectButton = (Button)findViewById(R.id.buttonConnect);
        scanButton   = (Button)findViewById(R.id.buttonScan);
        textStatus = (TextView)findViewById(R.id.textStatus);
        // get the bluetooth adapter
        btAdapter = BluetoothAdapter.getDefaultAdapter();

        // listView
        myListView = (ListView)findViewById(R.id.listView);

        // create the arrayAdapter that contains the BTDevices, and set it to the ListView
        myArrayAdapter = new ArrayAdapter<String>(this,android.R.layout.simple_list_item_1);
        myListView.setAdapter(myArrayAdapter);

        // by default, disable the buttons
        leftButton.setEnabled(false);
        rightButton.setEnabled(false);
        forwardButton.setEnabled(false);
        backwardButton.setEnabled(false);
        connectButton.setEnabled(false);

        if (btAdapter == null) {

            textStatus.setText("Status: not supported");
            Toast.makeText(getApplicationContext(), "Your device does not support Bluetooth", Toast.LENGTH_LONG).show();
        } else {
            // Create a new thread to listen to incoming BT stream
            btThread = new Thread (new Runnable() {
                public void run() {
                    listen();
                }
            });

           /*runnable = new Runnable () {
                @Override
                public void run() {
                    // create a new thread which infinitely listen to the incoming data
                    listen();

                };
                btThread = new Thread(runnable);

            }*/
            turnOnBluetooth(this.findViewById(android.R.id.content));

         //   Toast.makeText(getApplicationContext(), "Your device supports Bluetooth", Toast.LENGTH_LONG).show();
            //  text.setText("Status: Disconnected");
            leftButton.setOnClickListener(new OnClickListener() {
                @Override
                public void onClick(View v) {
                    //TODO:
                    robotTurnLeft(v);

                }
            });
            rightButton.setOnClickListener(new OnClickListener() {
                @Override
                public void onClick(View v) {
                    //TODO:
                    robotTurnRight(v);

                }
            });
            forwardButton.setOnClickListener(new OnClickListener() {
                @Override
                public void onClick(View v) {
                    //TODO:
                    robotGoForward(v);

                }
            });
            backwardButton.setOnClickListener(new OnClickListener() {
                @Override
                public void onClick(View v) {
                    //TODO:
                    robotGoBackward(v);

                }
            });
            connectButton.setOnClickListener(new OnClickListener() {
                @Override
                public void onClick(View v) {
                    //TODO:
                    connectBluetoothDevice(v);
                }
            });
            scanButton.setOnClickListener(new OnClickListener() {
                @Override
                public void onClick(View v) {
                    //TODO:
                    scanBluetoothDevice(v);
                }
            });
        }

    }


    @Override
    public boolean onCreateOptionsMenu(Menu menu) {
        // Inflate the menu; this adds items to the action bar if it is present.
        getMenuInflater().inflate(R.menu.menu_main, menu);
        return true;
    }

    @Override
    public boolean onOptionsItemSelected(MenuItem item) {
        // Handle action bar item clicks here. The action bar will
        // automatically handle clicks on the Home/Up button, so long
        // as you specify a parent activity in AndroidManifest.xml.
        int id = item.getItemId();

        //noinspection SimplifiableIfStatement
        if (id == R.id.action_settings) {
            return true;
        }

        return super.onOptionsItemSelected(item);
    }

    @Override
    protected void onActivityResult(int requestCode, int resultCode, Intent data) {
        // enable the buttons if bluetooth is on
        if (requestCode == REQUEST_ENABLE_BT) {
            if (btAdapter.isEnabled()) {
                textStatus.setText("Status: Bluetooth is on");
                leftButton.setEnabled(true);
                rightButton.setEnabled(true);
                forwardButton.setEnabled(true);
                backwardButton.setEnabled(true);
                connectButton.setEnabled(true);
            } else {
                textStatus.setText("Status: Bluetooth is off");
                leftButton.setEnabled(false);
                rightButton.setEnabled(false);
                forwardButton.setEnabled(false);
                backwardButton.setEnabled(false);
                connectButton.setEnabled(false);
            }
        }
    }
    public void turnOnBluetooth(View view) {
        // request to enable bluetooth if it is not already enabled
        if (!btAdapter.isEnabled()) {
            Intent enableBtIntent = new Intent(BluetoothAdapter.ACTION_REQUEST_ENABLE);
            // TODO: change to ACTION_STATE_CHANGED later
            // refer to http://developer.android.com/guide/topics/connectivity/bluetooth.html
            //   Intent enableBtIntent = new Intent(BluetoothAdapter.ACTION_STATE_CHANGED);
            startActivityForResult(enableBtIntent, REQUEST_ENABLE_BT);
            Toast.makeText(getApplicationContext(), "Bluetooth turned on", Toast.LENGTH_LONG).show();

        } else {
            Toast.makeText(getApplicationContext(), "Bluetooth is already on", Toast.LENGTH_LONG).show();
            textStatus.setText("Status: Bluetooth is on");
            leftButton.setEnabled(true);
            rightButton.setEnabled(true);
            forwardButton.setEnabled(true);
            backwardButton.setEnabled(true);
            connectButton.setEnabled(true);
        }

    }
    public void turnOffBluetooth(View view) {
        // turning off bluetooth
        btAdapter.disable();
        textStatus.setText("Status: Disconnected");
        Toast.makeText(getApplicationContext(),"Bluetooth turned off",Toast.LENGTH_LONG).show();
    }
    public void robotTurnLeft(View view) {
        textStatus.setText("Status: left");
        sendData("L");
    }
    public void robotTurnRight(View view) {
        textStatus.setText("Status: right");
        sendData("R");
    }
    public void robotGoForward(View view) {
        textStatus.setText("Status: forward");
        sendData("F");
    }
    public void robotGoBackward(View view) {
        textStatus.setText("Status: backward");
        sendData("B");
    }
    private void sendData(String message) {
        byte[] msgBuffer=message.getBytes();
        try {
            outStream.write(msgBuffer);
        } catch (IOException e) {

        }
    }

    public void connectBluetoothDevice(View view) {
        Set<BluetoothDevice> pairedDevices = btAdapter.getBondedDevices();
        // if there are paired devices
        if (pairedDevices.size() > 0) {
            // loop through paired devices
            for (BluetoothDevice device : pairedDevices) {
                // add the name and address to an array adapter to show in a ListView
                //myArrayAdapter.add(device.getName() + "\n" + device.getAddress());
                if (device.getName().equals(deviceName)) {
                  deviceAddress=device.getAddress();
                }
            }
            Toast.makeText(getApplicationContext(),"Show paired devices", Toast.LENGTH_SHORT).show();

            // connect to a particular device : TODO, choose from the listVIew
            if (deviceAddress!=null) {
                Toast.makeText(getApplicationContext(),"Try to connect to " + deviceName + "("+deviceAddress+")", Toast.LENGTH_SHORT).show();
                BluetoothDevice btDevice = btAdapter.getRemoteDevice(deviceAddress);
                try {
                    btSocket = btDevice.createRfcommSocketToServiceRecord(MY_UUID);
                } catch (IOException e) {
                    Toast.makeText(getApplicationContext(),"Unable to connect to " + deviceName, Toast.LENGTH_SHORT).show();
                    // TODO:
                    //exit;
                }
                try {
                    // Establish the connection
                    btSocket.connect();
                    textStatus.setText("Connected to " + deviceName);

                    try {
                        outStream = btSocket.getOutputStream();
                        inStream = btSocket.getInputStream();
                        btThread.start();
                    } catch (IOException e) {
                    }

                } catch (IOException e) {
                    textStatus.setText("Unable to connect to" + deviceName);
                }
            }
        } else {
            Toast.makeText(getApplicationContext(),"Unable to find paired devices", Toast.LENGTH_SHORT).show();
        }
    }
    public void scanBluetoothDevice(View view) {
        if (btAdapter.isDiscovering()) {
            // the button is pressed when it discovers, so cancel the discovery
            btAdapter.cancelDiscovery();
        } else {
              myArrayAdapter.clear();
              btAdapter.startDiscovery();
             registerReceiver(bReceiver, new IntentFilter(BluetoothDevice.ACTION_FOUND));
        }

    }

    final BroadcastReceiver bReceiver = new BroadcastReceiver() {
        @Override
        public void onReceive(Context context, Intent intent) {
            String action = intent.getAction();
            // When discovery finds a device
            if (BluetoothDevice.ACTION_FOUND.equals(action)) {
                // get the BluetoothDevice object from the Intent
                BluetoothDevice device = intent.getParcelableExtra(BluetoothDevice.EXTRA_DEVICE);
                // add the name and the MAC address of the object to the arrayAdapter
                myArrayAdapter.add(device.getName() + "\n" + device.getAddress());
                myArrayAdapter.notifyDataSetChanged();

            }
        }
    };

    @Override
    protected void onDestroy() {
        super.onDestroy();
        unregisterReceiver(bReceiver);
    }

    // Run listen in different thread
    public void listen() {
        // keep listening to the InputStream util an exception occurs
        String btBuffer="";
        handlerMsg = "";
        while (true) {
            if (inStream !=null) {
                try {

                    // Read from the InputStream

                    /*BufferedReader r = new BufferedReader(new InputStreamReader(inStream));
                    StringBuilder total = new StringBuilder();
                    String line;
                    while((line = r.readLine()) !=null) {
                        total.append(line);
                    }
                    textStatus.setText(total);*/

                    byte buffer[];
                    buffer = new byte[1024];
                    //BufferedInputStream bin = new BufferedInputStream(inStream);

                    // Read is synchronous call which keeps on waiting until data is available
                    int bytes = inStream.read(buffer);



                    if (buffer !=null && bytes > 0) {
                        for (int i = 0; i < bytes; i++) {
                           btBuffer += (char) buffer[i];
                        }
                        if (btBuffer.contains("\n")) {
                            // print message up to newline character
                            int index = btBuffer.indexOf('\n');
                            handlerMsg = btBuffer.substring(0, index);
                            btBuffer= btBuffer.substring(index+1);

                            handler.post(new Runnable() {
                                @Override
                                public void run() {
                                    textStatus.setText(handlerMsg);
                                    textStatus.setWidth(500);
                                    textStatus.setHeight(200);
                                    textStatus.setBackgroundColor(Color.parseColor("#DDFFFF"));
                                }
                            });
                        }
                    }


                } catch (IOException e) {
                    //TODO
                    break;
                }
            }

        }

    }


}

