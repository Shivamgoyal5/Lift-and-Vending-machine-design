// Pin Definitions
const int buttonPins[5] = {2, 3, 4, 5, 6};  // Button pins for floors 1-5
const int ledPins[5] = {A0, A1, A2, A3, A4}; // Floor LEDs for floors 1-5
const int upLedPin = 1;  // Upward direction LED
const int downLedPin = A5;  // Downward direction LED
const int segmentPins[7] = {7, 8, 9, 10, 11, 12, 13}; // 7-segment display (A-G)
const int dpPin = 0; // DP pin

// Priority queue for button presses (FIFO)
int floorQueue[5] = {0, 0, 0, 0, 0}; // Store up to 5 floor requests
int queueSize = 0; // Current size of the queue

// Floor number segment mappings (for common anode)
const byte floorNumbers[5][7] = {
  {HIGH, LOW, LOW, HIGH, HIGH, HIGH, HIGH},   // Floor 1
  {LOW, LOW, HIGH, LOW, LOW, HIGH, LOW},      // Floor 2
  {LOW, LOW, LOW, LOW, HIGH, HIGH, LOW},      // Floor 3
  {HIGH, LOW, LOW, HIGH, HIGH, LOW, LOW},     // Floor 4
  {LOW, HIGH, LOW, LOW, HIGH, LOW, LOW}       // Floor 5
};

// Current elevator state
int currentFloor = 1; // Start at floor 1
unsigned long inputStartTime; // Start time for button input period
bool inputInProgress = false; // To check if input is in progress

void setup() {
  // Initialize button pins as INPUT
  for (int i = 0; i < 5; i++) {
    pinMode(buttonPins[i], INPUT_PULLUP);
  }

  // Initialize floor LEDs as OUTPUT
  for (int i = 0; i < 5; i++) {
    pinMode(ledPins[i], OUTPUT);
  }

  // Initialize direction LEDs as OUTPUT
  pinMode(upLedPin, OUTPUT);
  pinMode(downLedPin, OUTPUT);

  // Initialize segment pins for 7-segment display as OUTPUT
  for (int i = 0; i < 7; i++) {
    pinMode(segmentPins[i], OUTPUT);
  }
  pinMode(dpPin, OUTPUT); // DP pin as OUTPUT

  // Start by clearing the 7-segment display
  clearDisplay();
  // Display the starting floor number
  displayFloor(currentFloor);
  lightFloorLed(currentFloor);
}

void loop() {
  // Start a new input period if not already in progress
  if (!inputInProgress) {
    startInputPeriod();
  }

  // Check button presses during input period
  if (inputInProgress) {
    checkButtons();

    // If the input period has ended
    if (millis() - inputStartTime >= 6000) { // 6 seconds input time
      inputInProgress = false; // End the input period
      if (queueSize > 0) {
        processQueue();  // Process the floors in the queue
      }
    }
  }
}

// Function to start the input period
void startInputPeriod() {
  inputStartTime = millis(); // Record the current time
  inputInProgress = true; // Mark input period as in progress
}

// Function to check button presses and add to the queue
void checkButtons() {
  for (int i = 0; i < 5; i++) {
    if (digitalRead(buttonPins[i]) == LOW) {  // Button is pressed
      if (!isInQueue(i + 1) && queueSize < 5) {  // Add the floor if not already in queue
        floorQueue[queueSize] = i + 1;
        queueSize++;
      }
    }
  }
}

// Check if a floor is already in the queue
bool isInQueue(int floor) {
  for (int i = 0; i < queueSize; i++) {
    if (floorQueue[i] == floor) {
      return true;
    }
  }
  return false;
}

// Function to process the queue
void processQueue() {
  for (int i = 0; i < queueSize; i++) {
    moveToFloor(floorQueue[i]);  // Process each floor in the queue
  }
  queueSize = 0; // Clear the queue after processing
}

// Function to move the elevator to the target floor
void moveToFloor(int targetFloor) {
  if (targetFloor == currentFloor) return;  // If already on the target floor, do nothing

  if (targetFloor > currentFloor) {
    digitalWrite(upLedPin, HIGH);  // Turn on Up LED
    digitalWrite(downLedPin, LOW); // Turn off Down LED
    for (int i = currentFloor + 1; i <= targetFloor; i++) {
      delay(1000);  // Simulate moving time (1 second per floor)
      displayFloor(i);  // Update 7-segment display
      lightFloorLed(i);  // Update LED for the corresponding floor
    }
  } else {
    digitalWrite(downLedPin, HIGH); // Turn on Down LED
    digitalWrite(upLedPin, LOW);    // Turn off Up LED
    for (int i = currentFloor - 1; i >= targetFloor; i--) {
      delay(1000);  // Simulate moving time (1 second per floor)
      displayFloor(i);  // Update 7-segment display
      lightFloorLed(i);  // Update LED for the corresponding floor
    }
  }

  currentFloor = targetFloor;  // Update the current floor
  digitalWrite(upLedPin, LOW);   // Turn off Up LED
  digitalWrite(downLedPin, LOW); // Turn off Down LED
  
  delay(3000); // Wait for 3 seconds at the destination floor
}

// Function to display the floor number on the 7-segment display
void displayFloor(int floor) {
  clearDisplay();  // Turn off all segments first
  for (int i = 0; i < 7; i++) {
    digitalWrite(segmentPins[i], floorNumbers[floor - 1][i]);  // Display the corresponding number
  }
  digitalWrite(dpPin, LOW); // Turn off DP (you can enable it if needed)
}

// Function to turn off all segments
void clearDisplay() {
  for (int i = 0; i < 7; i++) {
    digitalWrite(segmentPins[i], HIGH);  // Turn off all segments (HIGH for common anode)
  }
  digitalWrite(dpPin, HIGH); // Turn off DP
}

// Function to light up the LED for the corresponding floor
void lightFloorLed(int floor) {
  for (int i = 0; i < 5; i++) {
    if (i + 1 == floor) {
      digitalWrite(ledPins[i], HIGH); // Light up the current floor LED
    } else {
      digitalWrite(ledPins[i], LOW); // Turn off the others
    }
  }
}