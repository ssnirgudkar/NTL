/*
 * Macro Definitions
 */
#define SPEC_TRG         A0
#define SPEC_ST          A1
#define SPEC_CLK         A2
#define SPEC_VIDEO       A3
//#define WHITE_LED        A4
#define WHITE_LED        9
#define LASER_404        A5

#define SPEC_CHANNELS    288 // New Spec Channel
uint16_t data[SPEC_CHANNELS];
uint16_t data_LEDON[SPEC_CHANNELS];

char inputKey = '\0';
int8_t ledState = 0;

void setup(){

  //Set desired pins to OUTPUT
  pinMode(SPEC_CLK, OUTPUT);
  pinMode(SPEC_ST, OUTPUT);
  pinMode(LASER_404, OUTPUT);
  pinMode(WHITE_LED, OUTPUT);

  digitalWrite(SPEC_CLK, HIGH); // Set SPEC_CLK High
  digitalWrite(SPEC_ST, LOW); // Set SPEC_ST Low

  Serial.begin(115200); // Baud Rate set to 115200
  
}

/*
 * This functions reads spectrometer data from SPEC_VIDEO
 * Look at the Timing Chart in the Datasheet for more info
 */
void readSpectrometer(){

  int delayTime = 1; // delay time

  // Start clock cycle and set start pulse to signal start
  digitalWrite(SPEC_CLK, LOW);
  delayMicroseconds(delayTime);
  digitalWrite(SPEC_CLK, HIGH);
  delayMicroseconds(delayTime);
  digitalWrite(SPEC_CLK, LOW);
  digitalWrite(SPEC_ST, HIGH);
  delayMicroseconds(delayTime);

  //Sample for a period of time
  for(int i = 0; i < 15; i++){

      digitalWrite(SPEC_CLK, HIGH);
      delayMicroseconds(delayTime);
      digitalWrite(SPEC_CLK, LOW);
      delayMicroseconds(delayTime); 
 
  }

  //Set SPEC_ST to low
  digitalWrite(SPEC_ST, LOW);

  //Sample for a period of time
  for(int i = 0; i < 85; i++){

      digitalWrite(SPEC_CLK, HIGH);
      delayMicroseconds(delayTime);
      digitalWrite(SPEC_CLK, LOW);
      delayMicroseconds(delayTime); 
      
  }

  //One more clock pulse before the actual read
  digitalWrite(SPEC_CLK, HIGH);
  delayMicroseconds(delayTime);
  digitalWrite(SPEC_CLK, LOW);
  delayMicroseconds(delayTime);

  //Read from SPEC_VIDEO
  for(int i = 0; i < SPEC_CHANNELS; i++){

      if (inputKey != 's')
      {
         data[i] = analogRead(SPEC_VIDEO);
         data_LEDON[i] = data[i];
      }
      else
      {
         data_LEDON[i] = analogRead(SPEC_VIDEO);
      }
      
      digitalWrite(SPEC_CLK, HIGH);
      delayMicroseconds(delayTime);
      digitalWrite(SPEC_CLK, LOW);
      delayMicroseconds(delayTime);
  }

  //Set SPEC_ST to high
  digitalWrite(SPEC_ST, HIGH);

  //Sample for a small amount of time
  for(int i = 0; i < 7; i++){
    
      digitalWrite(SPEC_CLK, HIGH);
      delayMicroseconds(delayTime);
      digitalWrite(SPEC_CLK, LOW);
      delayMicroseconds(delayTime);
  }

  digitalWrite(SPEC_CLK, HIGH);
  delayMicroseconds(delayTime);

  for(int i = 0; i < SPEC_CHANNELS; i++){
     data[i] =  data_LEDON[i] - data[i]; 
  }
  
}
/*
 * The function below prints out data to the terminal or 
 * processing plot
 */
void printData(){
  
  for (int i = 0; i < SPEC_CHANNELS; i++){
    Serial.print(data[i]);
    Serial.print(',');
  }
  Serial.print("\n");
}



void processKeys() {

  char incomingByte = '\0';

  // send data only when you receive data:
  if (Serial.available() > 0) {
    // read the incoming byte:
    incomingByte = Serial.read();

    // say what you got:
    Serial.print("I received: ");
    Serial.println(incomingByte, DEC);
  }

  if (incomingByte == 's') {
    ledState = ! ledState;
    if (inputKey != 's') { // toggle input key
      inputKey = 's';
    } else {
      inputKey = 'x';
    }
  }
  if (ledState) {
      //digitalWrite(WHITE_LED, HIGH);
      analogWrite(WHITE_LED, 127); // any value between 0-255
  }
  else {
      //digitalWrite(WHITE_LED, LOW);
      analogWrite(WHITE_LED, 0);
  }
}

void loop(){
  processKeys();
  readSpectrometer();
  printData();
  delay(10);  
}
