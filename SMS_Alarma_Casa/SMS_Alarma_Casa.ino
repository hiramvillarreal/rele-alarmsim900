#include <SoftwareSerial.h>

//Libraries needed
#include "SIM900.h"
#include <SoftwareSerial.h>
#include "sms.h"


SMSGSM sms;
char my_number [13] = "YOURNUMBER"; // CHANGE THIS FOR YOU PERSONAL NUMBER
int pirPin1 = 4; // PIR motion sensor on pin D4
int Relay1 = 5; // Relay1 on D5
int Sirena = 6; // Relay for alarm siren D6
int LedState = 13; //LED armed-disarmed alarm status
boolean started = false; //Boolean for SIM online
boolean armado = true; // Boolean for alarm status armed-disarme
char pos; //  SMS sposition on SIM
char message[180]; // Message SMS characterlenght
char number[13]; // Telefone number from sender
char *ArmedON;
char *ArmedOFF;
char *Relay1On;
char *Relay1Off;
byte stat = 0;
//int numberint =0;
//int my_numberint = 0;


void setup()
{
  Serial.begin(9600);
  pinMode(LedState, OUTPUT);
  digitalWrite(LedState, HIGH);
  pinMode(Sirena, OUTPUT);
  digitalWrite(Sirena, HIGH);
  pinMode(Relay1, OUTPUT);
  digitalWrite(Relay1, HIGH);
  pinMode(pirPin1,  INPUT);
  digitalWrite(pirPin1, LOW);
  Serial.println("GSM Shield testing.");
  //Start configuration of shield with baudrate.
  if (gsm.begin(9600))
  {
    Serial.println("\nstatus=READY");
    started = true;
    armado = true;
  }
  else
    Serial.println("\nstatus=IDLE");
  if (started)
  {
    //Enable this two lines if you want to send an SMS.
    Serial.println("\nSMS Enviando Sistema se reinicio totalmente");
    if (sms.SendSMS(my_number, "SOY HIRAM CONTESTA PARA PROBAR: Sistema se reinicio ARMADO  Enviar:\n 4 -Para desarmar \n 2 -Para relevador1 ON\n 3 -Para relevador1 OFF"));
    //if NO SPACE ,you need delte SMS  from position 1 to position 20
    //please enable this four lines
    for (int i = 1; i <= 20; i++)
    {
      sms.DeleteSMS(i);
    }
  }
  delay(10000);
};

void loop()
{
  Serial.print("Sensor de movimiento1:  "); Serial.println(digitalRead(pirPin1));
  //Serial.println("Sensor de movimiento2:  "); Serial.println(digitalRead(pirPin2));
  Serial.print("Armado:  "); Serial.println(armado);
  if (armado == true)
  {
    if  (digitalRead(pirPin1) == HIGH)
    {
      delay(8000);
      if (digitalRead(pirPin1) == HIGH)
      {
        Serial.println("Alarma activada!!! ");
        digitalWrite(Sirena, LOW);
        if (sms.SendSMS(my_number, "Sirena activada!!! \n Enviar: \n 4 -Para desarmar"))
          Serial.println("\nEnviando SMS alarma activada.");
        for (int i = 1; i <= 20; i++)
        {
          sms.DeleteSMS(i);
        }
        delay(5000);
        // loop timer for Time for siren on:
        for (int i = 0; i <= 40; i++) {
          unREAD();
        }
        Serial.println("Alarma silenciada... ");
        digitalWrite(Sirena, HIGH);
      }

      else
        Serial.println("No Movimiento constante...");
    }
  }
  unREAD();
}

//Funtion read new SMSs
void unREAD()
{
  pos = sms.IsSMSPresent(SMS_UNREAD);
  stat = sms.GetAuthorizedSMS((int)pos, number, message, 180, 1, 10);
  if ((int)pos > 0 && (int)pos <= 20)
  {
    Serial.print ("SMS entrante de: ");
    Serial.println(number);
    int numberint = int(number);
    int my_numberint = int(my_number);
    //if (stat == GETSMS_AUTH_SMS) {
    if (numberint == numberint) {
      message[0] = '\0';
      sms.GetSMS((int)pos, number, message, 180);
      Serial.print("Mensaje entro: ");
      Serial.println(message);
      ArmedON = strstr(message, "1");
      Relay1On = strstr(message, "2");
      Relay1Off = strstr(message, "3");
      ArmedOFF = strstr(message, "4");
      if (ArmedON) {
        digitalWrite(LedState, HIGH);
        armado = true;
        sendstatSMS();
      }
      if (ArmedOFF) {
        digitalWrite(LedState, LOW);
        armado = false;
        digitalWrite(Sirena, HIGH);
        delay(5000);
        sendstatSMS();
      }
      if (Relay1On) {
        digitalWrite(Relay1, LOW);
        sendstatSMS();
      }
      if (Relay1Off) {
        digitalWrite(Relay1, HIGH);
        sendstatSMS();
      }
    }
    else
    {
      sms.DeleteSMS((int)pos);
    }
    delay(1000);
  }
  else
  {
    Serial.println("NO NEW SMS,WAITTING");
  }

  delay(1000);
}




void sendstatSMS()
{
  if (armado == true && !Relay1)
  {
    if (started)
    {
      if (sms.SendSMS(my_number, "Sistema armado. \n Relevador1 OFF. \n Enviar:\n 4 -Para desarmar  \n 2 -Para relevador1 ON"))
        Serial.println("\nEnviando Sistema armado");
      for (int i = 1; i <= 20; i++)
      {
        sms.DeleteSMS(i);
      }
      delay(5000);
    }
    Serial.println("Sistema armado, Relevador1 OFF");
  }

  if (armado == true && Relay1)
  {
    if (started)
    {
      if (sms.SendSMS(my_number, "Sistema armado. \n Relevador1 ON. \n Enviar:\n 4 -Para desarmar \n 3 -Para relevador1 OFF"))
        Serial.println("\nEnviando Sistema armado");
      for (int i = 1; i <= 20; i++)
      {
        sms.DeleteSMS(i);
      }
      delay(5000);
    }
    Serial.println("Sistema armado, Relevador1 ON");
  }

  if (armado == false && !Relay1) {
    if (started)
    {
      //Enable this two lines if you want to send an SMS.
      if (sms.SendSMS(my_number, "Sistema desarmado. \n Relevador1 OFF \n Enviar:\n 1 -Para armar\n 2 -Para relevador1 ON"))
        Serial.println("\nEnviando SMS");
      for (int i = 1; i <= 20; i++)
      {
        sms.DeleteSMS(i);
      }
      delay(5000);
    }
    Serial.println("Sistema desarmado, Relevador1 OFF");
  }

  if (armado == false && Relay1) {
    if (started)
    {
      //Enable this two lines if you want to send an SMS.
      if (sms.SendSMS(my_number, "Sistema desarmado. \n Relevador1 ON \n Enviar:\n 1 -Para armar \n 3 -Para relevador1 OFF"))
        Serial.println("\nEnviando SMS");
      for (int i = 1; i <= 20; i++)
      {
        sms.DeleteSMS(i);
      }
      delay(5000);
    }
    Serial.println("Sistema desarmado, Relevador1 ON");
  }
}




