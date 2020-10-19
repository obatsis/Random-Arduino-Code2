#include <DHT.h>
#include <Ethernet.h>
#include <SPI.h>

byte mac[] = { 0x00, 0xAA, 0xBB, 0xCC, 0xDE, 0x01 }; // RESERVED MAC ADDRESS
EthernetClient client;

#define DHTPIN 2 // SENSOR PIN
#define DHTTYPE DHT11 // SENSOR TYPE - THE ADAFRUIT LIBRARY OFFERS SUPPORT FOR MORE MODELS
DHT dht(DHTPIN, DHTTYPE);

long previousMillis = 0;
unsigned long currentMillis = 0;
long interval = 250000; // READING INTERVAL

int t = 0;	// TEMPERATURE VAR
int h = 0;	// HUMIDITY VAR
String data;

void setup() { 
	Serial.begin(115200);

	if (Ethernet.begin(mac) == 0) {
		Serial.println("Failed to configure Ethernet using DHCP"); 
	}
//den xreiazontai
	dht.begin(); 
	delay(10000); // GIVE THE SENSOR SOME TIME TO START

	h = (int) dht.readHumidity(); 
	t = (int) dht.readTemperature(); 
//ta apo pano
	data = "";
}

//sinartisi pou na kanei tin angnosi eisodou kai ton diaxorismo dedomenon tipou sting mallon

void loop(){
 //edo prepei na ginetai i anagnosi kai i epilogh ton metriseon
	currentMillis = millis();
	if(currentMillis - previousMillis > interval) { // READ ONLY ONCE PER INTERVAL    
		previousMillis = currentMillis;
		h = (int) dht.readHumidity();
		t = (int) dht.readTemperature();
	}

	data = "temp1=" + t + "&hum1=" + h;

	if (client.connect("www.*****.*************.com",80)) { // REPLACE WITH YOUR SERVER ADDRESS
		client.println("POST /add.php HTTP/1.1"); 
		client.println("Host: *****.*************.com"); // SERVER ADDRESS HERE TOO
		client.println("Content-Type: application/x-www-form-urlencoded"); 
		client.print("Content-Length: "); 
		client.println(data.length()); 
		client.println(); 
		client.print(data); 
	} 

	if (client.connected()) { 
		client.stop();	// DISCONNECT FROM THE SERVER
	}
//prepei na ginei ana ena deuterolepto
	delay(300000); // WAIT FIVE MINUTES BEFORE SENDING AGAIN 
}



