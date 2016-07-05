
using UnityEngine;
using System.Collections;

using System;
using System.Net;
using System.Net.Sockets;
using System.Text;

using System.Threading;
using System.Collections.Generic;

public class UdpListenUnity : MonoBehaviour {

	public int listenPort;

	private Thread t;
	private UdpClient listener;
	private bool msgFromThread = false;
	private string msgName;
	private object msgPayload;

	void Start () {
		t = new Thread( new ThreadStart(ListenThread) );
		t.IsBackground = true;
		t.Start();
	}

	void ListenThread() {
		listener = new UdpClient(listenPort);
		IPEndPoint groupEP = new IPEndPoint(IPAddress.Any, listenPort);
		string dat;
		byte[] receive_byte_array;
		Debug.Log("Listener: Waiting for broadcasts...\n");
		while (true) {
			receive_byte_array = listener.Receive(ref groupEP);
			dat = Encoding.ASCII.GetString(receive_byte_array, 0, receive_byte_array.Length);
//			Debug.Log ("Listener: Received a broadcast from " +
//				groupEP.ToString() + "\nMessage: " + received_data);
			// route message
			if (dat.StartsWith("REGISTER")) {
				// register a kiosk
				string name = dat.Substring(9);
				Kiosk k = new Kiosk(name, groupEP.Address, groupEP.Port);
				msgName = "AddNewKiosk";
				msgPayload = k;
				msgFromThread = true;
			}

		}
	}

	void Update() {
		if (msgFromThread) {
			gameObject.SendMessage(msgName, msgPayload);
			msgFromThread = false;
		}
	}

	void OnApplicationQuit() {
		if (t.IsAlive) t.Abort();
		if (listener!=null) listener.Close();
	}
}
