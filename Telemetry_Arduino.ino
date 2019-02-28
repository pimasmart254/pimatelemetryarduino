//Script name: Pima Smart Arduino Telemtry
//Developer: Kennedy W
//Date: January 2019
//Metadata: Electricity Meter
//Attributions: ModbusMaster Library 


/////////////////////////////////TELEMETRY1//////////////////////////////////////////////
#include <WiFiEspClient.h>
#include <WiFiEsp.h>
#include <WiFiEspUdp.h>
#include <PubSubClient.h>

#define WIFI_AP "LAN"
#define WIFI_PASSWORD "unifi123."
#define TOKEN "tkQGoZ5EIF0azlkCxLQe"

char thingsboardServer[] = "47.90.134.53";

// Initialize the Ethernet client object
WiFiEspClient espClient;
PubSubClient client(espClient);

int status = WL_IDLE_STATUS;
unsigned long lastSend;

//////////////////////////////////////////////////////////////////////////////////////

/////////////////////////////PAYLOAD1////////////////////////////////
#include <ModbusMaster.h>


// instantiate ModbusMaster object
ModbusMaster node;
float AV_LN, AV_LL, AV_C, AV_PF, F, kWh, TkW, TkVA, TkVAr;
///////////////////////////////END1///////////////////////////////////


void setup() {
 ///////////////////////////////////////////TELEMETRY2////////////////////////////////
  // initialize serial for debugging
  Serial.begin(9600);                           //Modded

  InitWiFi();
  client.setServer( thingsboardServer, 1883 );
  lastSend = 0;
 ////////////////////////////////////////////////////////////////////////////////////

   /////////////////////////////PAYLOAD2////////////////////////////////
  // use Serial (port 0); initialize Modbus communication baud rate
  Serial.begin(9600);

  // communicate through ModbusMaster node 1 over Serial (port 0)
  node.begin(1, Serial);
  ///////////////////////////////END2///////////////////////////////////
}

void loop() {
  /////////////////////////////////////////////////////////////////TELEMETRY3/////////////////////////////
  status = WiFi.status();
  if ( status != WL_CONNECTED) {
    while ( status != WL_CONNECTED) {
      Serial.print("Attempting to connect to WPA SSID: ");
      Serial.println(WIFI_AP);
      // Connect to WPA/WPA2 network
      status = WiFi.begin(WIFI_AP, WIFI_PASSWORD);
      delay(500);
    }
    Serial.println("Connected to AP");
  }

  if ( !client.connected() ) {
    reconnect();
  }

  if ( millis() - lastSend > 1000 ) { // Update and send only after 1 second
    ReadSendData();
    lastSend = millis();
  }

  client.loop();
 ///////////////////////////////////////////////////////////////////////////////////////////////////////
}

///////////////////////////////////////////////////////////PAYLOAD FUNCTION///////////////////////////////////////////

void ReadSendData()
{

  uint8_t j, result;
  unsigned long ii;                                             
  float flt, ff[60];
  

  // slave: read (60) 16-bit registers starting at register 1 to RX buffer
  //result = node.readInputRegisters(1, 60);                                   
  
  result = node.readInputRegisters(0, 59);                   
  
  // do something with data if read is successful

  if (result == node.ku8MBSuccess)
  {
  
  for (j = 0; j < 59; j+=2) {                                     

    ii = node.getResponseBuffer(j);                                
    ii = (ii<<16) | node.getResponseBuffer(j+1);                  
    
    
    flt = *(float *)&ii;                                           

    ff[j] = flt;                                                   
    //Serial.print(ff[j], 2);                                          
    //Serial.println(",");
    } 
   
  }
  
  
    //Initialization of carrier variables
   AV_LN = random(10, 20);
   AV_LL = random(30, 40);
   AV_C = random(30, 40);
   AV_PF = random(30, 40);
   F = random(30, 40);
   kWh = random(30, 40);
   TkW = random(30, 40);
   TkVA = random(30, 40);
   TkVAr = random(70, 80);

  
  //Assignment
    AV_LN = ff[6];
    AV_LL = ff[14];
    AV_C = ff[22];
    AV_PF = ff[42];
    F = ff[44];
    kWh = ff[46];
    TkW = ff[54];
    TkVA = ff[56];
    TkVAr = ff[58];
    
  

  String avg_volt_ln = String(AV_LN);
  String avg_volt_ll = String(AV_LL);
  String avg_current = String(AV_C);
  String avg_pf = String(AV_PF);
  String frequency = String(F);
  String energy = String(kWh);
  String t_active_pow = String(TkW);
  String t_apparent_pow = String(TkVA);
  String t_reactive_pow = String(TkVAr);
 
  // Prepare a JSON payload string
  String payload = "{";
  payload += "\"avg_volt_ln\":"; payload += avg_volt_ln; payload += ",";
  payload += "\"avg_volt_ll\":"; payload += avg_volt_ll; payload += ",";
  payload += "\"avg_current\":"; payload += avg_current; payload += ",";
  payload += "\"avg_pf\":"; payload += avg_pf; payload += ",";
  payload += "\"frequency\":"; payload += frequency; payload += ",";
  payload += "\"energy\":"; payload += energy; payload += ",";
  payload += "\"t_active_pow\":"; payload += t_active_pow; payload += ",";
  payload += "\"t_apparent_pow\":"; payload += t_apparent_pow; payload += ",";
  payload += "\"t_reactive_pow\":"; payload += t_reactive_pow;
  payload += "}";

  // Send payload
  char attributes[200];
  payload.toCharArray( attributes, 200 );
  client.publish( "v1/devices/me/telemetry", attributes );
  Serial.println( attributes );
  delay(10000);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////TELEMETRY AUX FUNCTION1//////////////////////////////////
void InitWiFi()
{
  // initialize serial for ESP module
  Serial1.begin(115200);
  
  // initialize ESP module
  WiFi.init(&Serial1); 
  // check for the presence of the shield
  if (WiFi.status() == WL_NO_SHIELD) {
    //Serial.println("WiFi shield not present");
    // don't continue
    while (true);
  }

  //Serial.println("Connecting to AP ...");
  // attempt to connect to WiFi network
  while ( status != WL_CONNECTED) {
    //Serial.print("Attempting to connect to WPA SSID: ");
    //Serial.println(WIFI_AP);
    // Connect to WPA/WPA2 network
    status = WiFi.begin(WIFI_AP, WIFI_PASSWORD);
    delay(500);
  }
  //Serial.println("Connected to AP");
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////TELEMETRY AUX FUNCTION2//////////////////////////////////

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    //Serial.print("Connecting to ThingsBoard node ...");
    // Attempt to connect (clientId, username, password)
    if ( client.connect("Arduino Uno Device", TOKEN, NULL) ) {
      //Serial.println( "[DONE]" );
    } else {
      //Serial.print( "[FAILED] [ rc = " );
      //Serial.print( client.state() );
      //Serial.println( " : retrying in 5 seconds]" );
      // Wait 5 seconds before retrying
      delay( 5000 );
    }
  }
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////

