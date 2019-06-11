// Import required libraries
#include <WiFi.h>
#include "FirebaseESP32.h"

//Wifi Setup Username and password
const char* ssid = "iPhone";
const char* password = "Nepal123";
#define FIREBASE_HOST "ece544-6703a.firebaseio.com"
#define FIREBASE_AUTH "8waGIVTipXcctVkG41P5S7vkDEWpwQvnNgeA9gTx"

//Instance of the firebase
FirebaseData firebaseData;

//Initialize the incoming data from uart initially
int incoming = 0;

//Function declaration for sending to uart
void send_to_uart();


//Setup Wifi and setup firebase
void setup(void)
{ 
// Start Serial
Serial.begin(115200);

// Connect to WiFi
WiFi.begin(ssid, password);
while (WiFi.status() != WL_CONNECTED) {
delay(500);
Serial.print(".");
}
Serial.println("");
Serial.println("WiFi connected");

// Print the IP address
Serial.println(WiFi.localIP());
Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);

}

//Keep sending and receiving data if available
void loop() {  
  send_to_uart();
  receive_from_to_uart();
}
  


//Function to receive control signal from the Nexsys A7 Board
void receive_from_to_uart()
{
  if(Serial.available()>0){
    incoming = Serial.read();
    if(incoming == 0)
    {
      Serial.flush();
      Firebase.setInt(firebaseData,"MOTOR_SELECT",incoming);
    }    
  }
}

//Function to Read Signal from the Firebase and send to Nexsys A7 board
void send_to_uart()
{

  //Check if the data is available
  if (Firebase.getInt(firebaseData, "MOTOR_SELECT")) 
  {

    if (firebaseData.dataType() == "int") {
      int data = firebaseData.intData();
      Serial.write(data);
    }

  }

  //If there is an error, then display the error
  else {
    Serial.println(firebaseData.errorReason());
  }
  
}

