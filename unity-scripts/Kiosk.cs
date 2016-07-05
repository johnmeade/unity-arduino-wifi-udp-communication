using System.Net;
using System.Collections;

public class Kiosk {

	public string name;
	public IPAddress ip;
	public int port;

	public Kiosk (string name, IPAddress ip, int port) {
		this.name = name;
		this.port = port;
		this.ip = ip;
	}

}
