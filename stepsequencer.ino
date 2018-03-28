int buttonPin[6] = { 24, 25, 26, 27, 39, 38 };
int ledPin[4] = { A9, A8, A7, A6 };

boolean lastButtonState[6] = { LOW, LOW, LOW, LOW, LOW, LOW };
boolean buttonState[6] = {LOW, LOW, LOW, LOW, LOW, LOW };

boolean stepState[3][4] = {
  {false, false, false, false },
  {false, false, false, false },
  {false, false, false, false }
};

int midiNote[3] = {60, 67, 72};

int tempo = 0;

int currentChannel = 0;

int currentStep = 0;
unsigned long lastStepTime = 0;



void setup() {

  //Serial.begin(9600);
  for (int i = 0; i < 4; i++) {
    pinMode(ledPin[i], OUTPUT);
  }
  for (int i = 0; i < 6; i++) {
    pinMode(buttonPin[i], INPUT);
  }
}

void loop() {
  //Serial.println(currentChannel);
  updateChannel();
  checkButtons();
  setLeds();
  sequence();
}


void updateChannel() {
  for (int i = 4; i < 6; i++) {
    lastButtonState[i] = buttonState[i];
    buttonState[i] = digitalRead(buttonPin[i]);

    if (buttonState[i] == HIGH && lastButtonState[i] == LOW) {
      if (i == 4) {
        currentChannel++;
        if (currentChannel > 2) {
          currentChannel = 0;
          //currentChannel = 2;  <- this would make it just stop once it reaches 2, keep repeating at te two
        } 
      }
      if (i == 5) {
        currentChannel--;
        if (currentChannel < 0) {
          currentChannel = 2;
        }
      }  
    }
  }
}


void checkButtons() {
  for (int i = 0; i < 4; i++) {
    lastButtonState[i] = buttonState[i];
    buttonState[i] = digitalRead(buttonPin[i]);

    if (buttonState[i] == HIGH && lastButtonState[i] == LOW) {
      if (stepState[currentChannel][i] == false) {
        stepState[currentChannel][i] = true;
      } else if (stepState[currentChannel][i] == true) {
        stepState[currentChannel][i] = false;
      }
    }
  }
}


void sequence() {

  tempo = map(analogRead(A13), 0, 1023, 10, 250);

  if (millis() > lastStepTime + tempo) {   //if its time to go to the next step...

    for (int i = 0; i < 3; i++) {
      if (stepState[i][currentStep] == HIGH) {
        usbMIDI.sendNoteOn(midiNote[i], 75, 1); //using curretnStep ebcuase it is soemthing that is alrady icrimenting, the note is now hardcoded
      }
      else {
        usbMIDI.sendNoteOff(midiNote[i], 0, 1);
      }
    }

    currentStep = currentStep + 1;         //increment to the next step
    if (currentStep > 3) {
      currentStep = 0;
    }

    lastStepTime = millis();               //set lastStepTime to the current time
  }
}


void setLeds() {
  for ( int i = 0; i < 4; i++) {
    if (i == currentStep) {
      analogWrite(ledPin[i], 255);
    }
    else if (stepState[currentChannel][i] == true ) {
      analogWrite(ledPin[i], 100);
    }
    else {
      analogWrite(ledPin[i], LOW);
    }
  }
}

