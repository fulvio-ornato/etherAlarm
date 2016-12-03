long tempo=1;
int secondi=0;
int scaduto=0;
int ritardo=1;
long timeStart,timeEnd; 
int btnProgramma = 2;  
int btnAvanza = 9;  
int flagEsci=1;
int buzzer = 12; 
int myPins[] = {3,4,6,7,8,10};

void setup() {
  pinMode(btnProgramma, INPUT);    // button as input
  digitalWrite(btnProgramma, HIGH); // turns on pull-up resistor after input
  pinMode(btnAvanza, INPUT);    // button as input
  digitalWrite(btnAvanza, HIGH); // turns on pull-up resistor after input
    
  Serial.begin(57600);
  //set as output the pin 3,4,6,7,8,10
  for (int i=0; i<=5; i++) {
      pinMode(myPins[i], OUTPUT);
  }

  timeStart = millis();
  timeEnd = timeStart+tempo*1000*60;
  Serial.print("timeStart ");
  Serial.println(timeStart);
  Serial.print("timeEnd ");
  Serial.println(timeEnd);
  
}

void cyclon() {
  int i=3;
  int verso=0;
  while (1) {
    Serial.print("i= ");
    Serial.println(i);
    if (verso == 0) {
      if (i>3) {digitalWrite(i-1, LOW);};
      if (i==3) {digitalWrite(8, LOW);};
    }
    if (verso == 1) {
      if (i<8) {digitalWrite(i+1, LOW);};
      if (i==8) {digitalWrite(3, LOW);};
    }
    digitalWrite(i, HIGH);
    if (verso==0) {i++;} else {i--;};
    if (i>8 and verso==0) {i=8;verso=1;};
    if (i<3 and verso==1) {i=3;verso=0;delay(2000);};
    delay(100);
  } 
 
}

void beep() {
  tone(buzzer,2000,50);
}

// the loop function runs over and over again forever
void loop() {
  settaTimer();
  tempo = (timeEnd - millis())/60000;
  secondi = ((timeEnd - millis())/1000)%60;
  if (secondi==0) {
    beep();
  }
  Serial.print("tempo ");
  Serial.println(tempo);
  Serial.print("secondi ");
  Serial.println(secondi);
  Serial.print("tempo bit 0=");
  Serial.println(bitRead(tempo,0));
  Serial.print("tempo bit 1=");
  Serial.println(bitRead(tempo,1));

  //if the time didn't end turn on the LED bit for bit
  if (timeEnd > millis()) {
    for (int i=0; i<=5; i++) {
      digitalWrite(myPins[i], bitRead(tempo,i));
    }
    if (tempo==0) {
      Serial.println("ultimo minuto");
      digitalWrite(myPins[0], bitRead(secondi,0));
    }
    delay(1000);   
  } else {
  //if the time ended blink all the led and active the buzzer
  //after one minute blink decrease  
    scaduto++;
    if (scaduto < 60) {
      for (int i=0; i<=5; i++) {
        digitalWrite(myPins[i], LOW);   
      }
      delay(200*ritardo);              
      for (int i=0; i<=5; i++) {
        digitalWrite(myPins[i], HIGH); 
        beep();
      }
      delay(500/ritardo);      
    } else {
      //cyclon();
      ritardo=10;
      scaduto=0;
    }
  }
}

void settaTimer() {
  
  //Serial.println("sto in verifica");
  tempo=1;
  if (digitalRead(btnProgramma) == LOW) {
    delay(100);
      if (digitalRead(btnProgramma) == LOW) {
        for (int j=1; j<3; j++) {beep();}

        Serial.println("stai programmando");
        //beep();
        delay(1000);
        while(1) {
         for (int i=0; i<=5; i++) {
           digitalWrite(myPins[i], bitRead(tempo,i));
         } 
         if (digitalRead(btnProgramma) == LOW) {
            delay(100);
            if (digitalRead(btnProgramma) == LOW) {   
              timeStart = millis();
              timeEnd = timeStart+tempo*1000*60;     
              return;
            }
          }
         if (digitalRead(btnAvanza) == LOW) {
            delay(100);
            if (digitalRead(btnAvanza) == LOW) {
              tempo++;
              tempo=tempo%64;          
            }
          }
        }
      }
  }
}



