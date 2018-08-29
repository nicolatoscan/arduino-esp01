#include <SoftwareSerial.h>

SoftwareSerial espSerial(7, 6);

void setup() {
  Serial.begin(9600);
  espSerial.begin(9600);

  //CONNECT TO WIFI AND START SERVER
  SendCommand("AT+RST", 500);

  SendCommand("AT+CWJAP=\"wifi\",\"password\"", 5000);
  SendCommand("AT+CWMODE=1", 1000);
  SendCommand("AT+CIFSR", 1000);
  SendCommand("AT+CIPMUX=1", 1000);
  SendCommand("AT+CIPSERVER=1,80", 1000);
}

void loop() {
  ReadIncomingCall();

}

void SendCommand(String s, int d) {
  espSerial.println(s);
  delay(d);
  ReadResponse(s);
}

void ReadResponse(String name) {
  while (espSerial.available()) {
    Serial.println(espSerial.readString());
  }
  Serial.println();
}

int connectionId;
void ReadIncomingCall() {
  if (espSerial.available())
  {
    if (espSerial.find("+IPD,"))  //CALLING
    {
      Serial.println(" ------ NEW INCOMING CALL ------ ");
      delay(10);

      connectionId = espSerial.read() - 48;
      espSerial.readStringUntil(' ');
      GETHandler();
      espSerial.readString();
      JSONResponse();
    }
  }
}

void GETHandler() {
  if (espSerial.read() != '/' || espSerial.read() != '?') {
    return;
  }

  Serial.println(" --- GET VALUES --- ");

  while (true)
  {
    String var = espSerial.readStringUntil('=');
    char value = espSerial.read();

    Serial.print(var);
    Serial.print(": ");
    Serial.println(value);

    if (var == "par1") {
      
    } else if (var == "par2") {
      
    }

    if (espSerial.read() != '&')
      return;
  }
}

void JSONResponse() {

  // -- REPLY
  String webpage = "Hello world";

  Serial.println("RESPONSE");
  webpage = "{\"result\": \"" + webpage + "\"}";

  String r = "HTTP/1.1 200 OK\r\n";
  r += "Access-Control-Allow-Origin: *\r\n";
  r += "Connection: keep-alive\r\n";
  r += "Content-Type: application/json\r\n";
  r += "\r\n" + webpage;
  EspJSONReply(r);

  //CLOSING
  String closeCommand = "AT+CIPCLOSE=";  // close the socket connection
  closeCommand += connectionId; // append connection id
  SendCommand(closeCommand, 10);
}

void EspJSONReply(String d)
{
  String cipSend = " AT+CIPSEND=";
  cipSend += connectionId;
  cipSend += ",";
  cipSend += d.length();
  SendCommand(cipSend, 10);
  SendCommand(d, 10);
}

