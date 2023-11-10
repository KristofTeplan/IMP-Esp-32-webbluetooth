#include <ArduinoBLE.h>
#include <math.h>

BLEService metronomService("5035fbd0-7d40-11ee-b962-0242ac120002");  // Define a custom metronome service
BLEIntCharacteristic bpmCharacteristic("8835439a-7d42-11ee-b962-0242ac120002", BLERead | BLEWrite); // Define a BPM characteristic
BLEIntCharacteristic volumeCharacteristic("912554cc-7d42-11ee-b962-0242ac120002", BLERead | BLEWrite); // Define a Volume characteristic
BLEIntCharacteristic beepLengthCharacteristic("95fd1818-7d42-11ee-b962-0242ac120002", BLERead | BLEWrite); // Define a Beep Length characteristic

int speakerPin = 12;
int ledPin = 2;
bool state = false;
unsigned long lastBeepTime = 0;



bool beepOn = false;
int volume = 2;
int bpm = 60;
int beepLength = 600;


hw_timer_t *timerTurnOFF = NULL;
hw_timer_t *timerTurnON = NULL;
hw_timer_t *timerLED = NULL;

bool ledON = false;

//interupt handler to turn speaker on
void IRAM_ATTR beep_ON_interrupt() {
  if(volume == 8)
  {
    analogWrite(speakerPin, 255); //Special case, using formula 2^8 = 256 but PWM has range 0-255
  }
  else if(volume == 0)
  {
    analogWrite(speakerPin, 0); //Special case, using formula 2^0 = 1 but we want complete silence
  }
  else
  {
    analogWrite(speakerPin, pow(2, volume)); //Humans can hear volume in exponencial differences
  }

  digitalWrite(ledPin, HIGH); //Visualisation timing
  timerRestart(timerTurnOFF); //
  timerAlarmEnable(timerTurnOFF);
}

void IRAM_ATTR beep_OFF_interrupt() {
  analogWrite(speakerPin, 0);
  digitalWrite(ledPin, LOW);

  timerAlarmEnable(timerTurnON);
}

void setup() {
  Serial.begin(115200); //Monitor speed

  // Pins input/output setup
  pinMode(speakerPin, OUTPUT);
  pinMode(ledPin, OUTPUT);

  // Timer that will turn sound off after desired time for one beep
  timerTurnOFF = timerBegin(0, 80, true);
  timerAttachInterrupt(timerTurnOFF, &beep_OFF_interrupt, true);
  timerAlarmWrite(timerTurnOFF, 1000*beepLength, true);
  // Timer that will turn sound on after time= one_minute/bpm - beepLengt
  timerTurnON = timerBegin(1, 80, true);
  timerAttachInterrupt(timerTurnON, &beep_ON_interrupt, true);
  timerAlarmWrite(timerTurnON, 60*1000000/bpm - beepLength, true);

  // Starting timerTurnOFF. This timer will then start second timer in its interrupt handler. Second timer will restart this timer and so on in a loop
  timerAlarmEnable(timerTurnOFF);
  
  // Start the BLE communication
  if (!BLE.begin()) {
    Serial.println("Starting BLE failed!");
    while (1); // Waiting for restart
  }

  // Set the local name and advertise the metronome service
  BLE.setLocalName("ESP32-BT metronome");
  BLE.setAdvertisedService(metronomService);

  // Add the characteristics to the metronome service
  metronomService.addCharacteristic(bpmCharacteristic);
  metronomService.addCharacteristic(volumeCharacteristic);
  metronomService.addCharacteristic(beepLengthCharacteristic);

  // Add the service
  BLE.addService(metronomService);

  // Start advertising the service
  BLE.advertise();
  Serial.println("Bluetooth device advertising...");
}

void loop() {
  // Listen for BLE peripheral events
  BLEDevice central = BLE.central();

  if (central) 
  {
    Serial.print("Connected to central: ");
    Serial.println(central.address());

    while (central.connected()) 
    {
      //////////////////////////////////////////////////
      // Check if the characteristics were written to //
      //////////////////////////////////////////////////

      //User changed volume via bluetooth
      if(bpmCharacteristic.written())
      {
        // Bpm has to be bigger than zero bc later we use it as a divisor(deliteľ)
        if(bpmCharacteristic.value() > 0)
        {
          bpm = bpmCharacteristic.value();
          timerAlarmWrite(timerTurnON, 60*1000000/bpm - beepLength, true); //Rewrites timer max count value
          printf("BPM set to: %d\n",bpm);
        }
        else
        {
          printf("BPM KEPT at value: %d\n", bpm);
          printf("Value: %d is not valid!\nValue has to be whole positive number.\n", bpm);
        }
      }

      //User changed volume via bluetooth
      if (volumeCharacteristic.written())
      {
        if(volumeCharacteristic.value() >= 0 && volumeCharacteristic.value() <= 8)
        {
          volume = volumeCharacteristic.value();
          printf("Volume set to: %d\n",volume);
        }
        else
        {
          printf("Volume KEPT at value: %d\n", volume);
          printf("Value: %d is not valid!\nValue has to be whole number in range <0,8>\n", volume);
        }
      }
      
      //User changed beepLength via bluetooth
      if(beepLengthCharacteristic.written()) 
      {
        if(beepLengthCharacteristic.value() >= 1 && (beepLengthCharacteristic.value() < (60*1000/bpm)))
        {
          digitalWrite(speakerPin, LOW);
          beepLength = beepLengthCharacteristic.value();
          timerAlarmWrite(timerTurnOFF, 1000*beepLength, true); //Rewrites timer max count value
          printf("BeepLength set to: %d\n",beepLength);
        }
        else
        {
          printf("BeepLength KEPT at value: %d\n", beepLength);
          printf("Value: %d is not valid!\nValue has to be bigger than 0 and be less then one period of metronome(to have both sound on and sound off)\n", beepLength);
        }
      }
    }

    Serial.print("Disconnected from central: ");
    Serial.println(central.address());
  }
}