
#include <Adafruit_DotStar.h>
#include <Fishino.h>
#include <SPI.h>

#define NUMPIXELS 144 // Number of LEDs in strip (numero di led della striscia)

// Here's how to control the LEDs from any two pins:
// pin di controllo della striscia
#define DATAPIN   6
#define CLOCKPIN   7

// oggetto per poter controllare la striscia
Adafruit_DotStar strip = Adafruit_DotStar(
  NUMPIXELS, DATAPIN, CLOCKPIN, DOTSTAR_BRG);
// The last parameter is optional -- this is the color data order of the
// DotStar strip, which has changed over time in different production runs.
// Your code just uses R,G,B colors, the library then reassigns as needed.
// Default is DOTSTAR_BRG, so change this if you have an earlier strip.

// Hardware SPI is a little faster, but must be wired to specific pins
// (Arduino Uno = pin 11 for data, 13 for clock, other boards are different).
//Adafruit_DotStar strip = Adafruit_DotStar(NUMPIXELS, DOTSTAR_BRG);
  
//////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////
// CONFIGURATION DATA		-- ADAPT TO YOUR NETWORK !!!
// DATI DI CONFIGURAZIONE	-- ADATTARE ALLA PROPRIA RETE WiFi !!!

#ifndef __MY_NETWORK_H

// here pur SSID of your network
// inserire qui lo SSID della rete WiFi
#define MY_SSID	"FanClock"

// here put PASSWORD of your network. Use "" if none
// inserire qui la PASSWORD della rete WiFi -- Usare "" se la rete non ￨ protetta
#define MY_PASS	"Admin01!"

// comment this line if you want a dynamic IP through DHCP
// obtained IP will be printed on serial port monitor
// commentare la linea seguente per avere un IP dinamico tramite DHCP
// l'IP ottenuto verrà visualizzato sul monitor seriale
//#define IPADDR		169, 254,   0, 40
//#define GATEWAY		192, 168,   1,   1
//#define NETMASK		255, 255, 0,   0

#endif

// local port to listen on
// porta UDP sulla quale Fishino attende i pacchetti
unsigned int localPort = 2390;
//                    END OF CONFIGURATION DATA                      //
//                       FINE CONFIGURAZIONE                         //
///////////////////////////////////////////////////////////////////////

// define ip address if required
// NOTE : if your network is not of type 255.255.255.0 or your gateway is not xx.xx.xx.1
// you should set also both netmask and gateway
#ifdef IPADDR
	IPAddress ip(IPADDR);
	#ifdef GATEWAY
		IPAddress gw(GATEWAY);
	#else
		IPAddress gw(ip[0], ip[1], ip[2], 1);
	#endif
	#ifdef NETMASK
		IPAddress nm(NETMASK);
	#else
		IPAddress nm(255, 255, 255, 0);
	#endif
#endif

// buffer to hold incoming packet
// buffer per contenere i pacchetti ricevuti
char packetBuffer[255];

// a string to send back
// una stringa di conferma da spedire al mittente
char  ReplyBuffer[] = "acknowledged";

// the UDP client/server
// il client/server UDP
FishinoUDP Udp;

void printWifiStatus()
{
	// print the SSID of the network you're attached to:
	// stampa lo SSID della rete:
	Serial.print("SSID: ");
	Serial.println(Fishino.SSID());

	// print your WiFi shield's IP address:
	// stampa l'indirizzo IP della rete:
	IPAddress ip = Fishino.localIP();
	Serial << F("IP Address: ");
	Serial.println(ip);

	// print the received signal strength:
	// stampa la potenza del segnale di rete:
	long rssi = Fishino.RSSI();
	Serial << F("signal strength (RSSI):");
	Serial.print(rssi);
	Serial << F(" dBm\n");
}

// setup code
// codice di inizializzazione
void setup()
{ 
  strip.begin(); // Initialize pins for output
  strip.show();
	// Initialize serial and wait for port to open
	// Inizializza la porta seriale e ne attende l'apertura
	Serial.begin(115200);

	// reset and test WiFi module
	// resetta e testa il modulo WiFi
	while(!Fishino.reset())
	Serial << F("Fishino RESET FAILED, RETRYING...\n");
	Serial << F("Fishino WiFi RESET OK\n");

	// go into station mode
	// imposta la modalità stazione
	Fishino.setMode(STATION_MODE);

	// try forever to connect to AP
	// tenta la connessione finchè non riesce
	Serial << F("Connecting to AP...");
	while(!Fishino.begin(MY_SSID, MY_PASS))
	{
		Serial << ".";
		delay(2000);
	}
	Serial << "OK\n";
	
	
	// setup IP or start DHCP client
	// imposta l'IP statico oppure avvia il client DHCP
#ifdef IPADDR
	Fishino.config(ip, gw, nm);
#else
	Fishino.staStartDHCP();
#endif

	// wait till connection is established
	Serial << F("Waiting for IP...");
	while(Fishino.status() != STATION_GOT_IP)
	{
		Serial << ".";
		delay(500);
	}
	Serial << "OK\n";

	
	// print connection status on serial port
	// stampa lo stato della connessione sulla porta seriale
	printWifiStatus();

	// starts listening on local port
	// inizia l'ascolto dei pacchetti UDP alla porta specificata
	Serial << F("Starting connection to server...\n");
	Udp.begin(localPort);
}

//variabili per controllare se i dati sono già stati letti
bool cLedN = false;
bool cRed = false;
bool cGreen = false;
bool cBlue = false;
//variabili per contenere i diversi dati
int nLed = 0;
int color[3] = {0,0,0};

void loop()
{
  //azzero le variabili di controllo 
  cLedN = false;
  cRed = false;
  cGreen = false;
  cBlue = false;
  //accendo i led con gli ultimi dati che ho ricevuto
  for(int i = 0; i < nLed; i++){
    strip.setPixelColor(i,color[1],color[0],color[2]);
  }
  strip.show();
  
	// if there's data available, read a packet
	// se ci sono dati in arrivo, li stampa
	int packetSize = Udp.parsePacket();
	if (packetSize)
	{
		Serial << F("Received packet of size ");
		Serial << packetSize << "\n";
		Serial << F("From ");
		IPAddress remoteIp = Udp.remoteIP();
		Serial << remoteIp;
		Serial << F(", port ");
		Serial << Udp.remotePort() << "\n";

		// read the packet into packetBufffer
		// legge il pacchetto nel buffer
		int len = Udp.read(packetBuffer, 255);
		if (len > 0)
			packetBuffer[len] = 0;

		Serial << F("Contents:\n");
		Serial.println(packetBuffer);
    //leggo il contenuto del pacchetto
    for(int i = 0; i < 255; i++){
       String s = "";
       if(packetBuffer[i] == 'n' && !cLedN){
          //leggo il numero di led da accendere
          s = String(packetBuffer[i+1])+String(packetBuffer[i+2])+String(packetBuffer[i+3]);
          Serial.println(s.toInt());
          nLed = s.toInt();
          cLedN = true;
       }
       else if(packetBuffer[i] == 'r' && !cRed){
          //leggo il valore del rosso
          s = String(packetBuffer[i+1])+String(packetBuffer[i+2])+String(packetBuffer[i+3]);
          color[0] = s.toInt();
          Serial.println(color[0]);
          cRed = true;
       }
       else if(packetBuffer[i] == 'g' && !cGreen){
          //leggo il valore del verde
          s = String(packetBuffer[i+1])+String(packetBuffer[i+2])+String(packetBuffer[i+3]);
          color[1] = s.toInt();
          Serial.println(color[1]);
          cGreen = true;
       }
       else if(packetBuffer[i] == 'b' && !cBlue){
          //leggo il valore del blu
          s = String(packetBuffer[i+1])+String(packetBuffer[i+2])+String(packetBuffer[i+3]);
          color[2] = s.toInt();
          Serial.println(color[2]);
          cBlue = true;
       }
    }  
    // send a reply, to the IP address and port that sent us the packet we received
    // invia una risposta all'indirizzo IP e alla porta da cui proviene la richiesta
    Udp.beginPacket(Udp.remoteIP(), Udp.remotePort());
    Udp.write(ReplyBuffer);
    Udp.endPacket();        
  }
}
