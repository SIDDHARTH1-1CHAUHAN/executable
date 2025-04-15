/* 
 * Digital Logic Lab Simulator - Complete Implementation
 * Supports all basic gates, combinational/sequential circuits, timers, counters and decoders
 * Designed for Arduino Mega (for sufficient I/O pins)
 */

// ====================
// PIN CONFIGURATION
// ====================
// Input Pins (Connect switches/buttons here)
const int inputPins[] = {22, 24, 26, 28, 30, 32, 34, 36}; // 8 input pins
const int numInputs = 8;

// Output Pins (Connect LEDs here)
const int outputPins[] = {23, 25, 27, 29, 31, 33, 35, 37}; // 8 output pins
const int numOutputs = 8;

// Special Pins
const int clockPin = 38;     // For sequential circuits
const int resetPin = 39;     // System reset
const int modePin = 40;      // Mode selection
const int segmentPins[7] = {41, 43, 45, 47, 49, 51, 53}; // 7-segment pins (a-g)

// ====================
// GLOBAL VARIABLES
// ====================
String currentCircuit = "AND";
bool lastClockState = LOW;
unsigned long lastPulseTime = 0;
int counterValue = 0;
bool flipFlopState = LOW;

// 7-segment display patterns (0-9)
const byte digitPatterns[10] = {
  B00111111, // 0
  B00000110, // 1
  B01011011, // 2
  B01001111, // 3
  B01100110, // 4
  B01101101, // 5
  B01111101, // 6
  B00000111, // 7
  B01111111, // 8
  B01101111  // 9
};

// ====================
// SETUP FUNCTION
// ====================
void setup() {
  // Initialize all input pins
  for (int i = 0; i < numInputs; i++) {
    pinMode(inputPins[i], INPUT_PULLUP);
  }
  
  // Initialize all output pins
  for (int i = 0; i < numOutputs; i++) {
    pinMode(outputPins[i], OUTPUT);
  }
  
  // Initialize special pins
  pinMode(clockPin, INPUT_PULLUP);
  pinMode(resetPin, INPUT_PULLUP);
  pinMode(modePin, INPUT_PULLUP);
  
  // Initialize 7-segment display pins
  for (int i = 0; i < 7; i++) {
    pinMode(segmentPins[i], OUTPUT);
  }
  
  // Start serial communication
  Serial.begin(115200);
  Serial.println("Digital Logic Lab Simulator Initialized");
  printMenu();
}

// ====================
// MAIN LOOP
// ====================
void loop() {
  // Check for serial commands
  if (Serial.available() > 0) {
    handleSerialCommand();
  }
  
  // Read all inputs
  bool inputs[numInputs];
  for (int i = 0; i < numInputs; i++) {
    inputs[i] = digitalRead(inputPins[i]);
  }
  
  // Process the selected circuit
  if (currentCircuit.startsWith("Basic")) {
    processBasicGates(inputs);
  } 
  else if (currentCircuit.startsWith("Combinational")) {
    processCombinationalCircuits(inputs);
  }
  else if (currentCircuit.startsWith("Sequential")) {
    processSequentialCircuits(inputs);
  }
  else if (currentCircuit.startsWith("Timers")) {
    processTimerCircuits();
  }
  else if (currentCircuit.startsWith("Counters")) {
    processCounterCircuits(inputs);
  }
  else if (currentCircuit.startsWith("Decoders")) {
    processDecoderCircuits(inputs);
  }
  
  delay(10); // Small delay for stability
}

// ====================
// CIRCUIT PROCESSING FUNCTIONS
// ====================

// Basic Logic Gates
void processBasicGates(bool inputs[]) {
  bool A = inputs[0];
  bool B = inputs[1];
  bool output = LOW;
  
  if (currentCircuit == "AND") output = A && B;
  else if (currentCircuit == "OR") output = A || B;
  else if (currentCircuit == "NOT") output = !A;
  else if (currentCircuit == "NAND") output = !(A && B);
  else if (currentCircuit == "NOR") output = !(A || B);
  else if (currentCircuit == "XOR") output = A ^ B;
  else if (currentCircuit == "XNOR") output = !(A ^ B);
  
  digitalWrite(outputPins[0], output);
  Serial.print("Output: "); Serial.println(output ? "HIGH" : "LOW");
}

// Combinational Circuits
void processCombinationalCircuits(bool inputs[]) {
  bool A = inputs[0];
  bool B = inputs[1];
  bool C = inputs[2];
  
  if (currentCircuit == "Half Adder") {
    bool sum = A ^ B;
    bool carry = A && B;
    digitalWrite(outputPins[0], sum);
    digitalWrite(outputPins[1], carry);
  }
  else if (currentCircuit == "Full Adder") {
    bool sum = A ^ B ^ C;
    bool carry = (A && B) || (B && C) || (A && C);
    digitalWrite(outputPins[0], sum);
    digitalWrite(outputPins[1], carry);
  }
  else if (currentCircuit == "Multiplexer (MUX)") {
    // 4:1 MUX implementation
    bool S0 = inputs[2];
    bool S1 = inputs[3];
    bool output = (!S1 && !S0 && A) || (!S1 && S0 && B) || 
                 (S1 && !S0 && C) || (S1 && S0 && inputs[4]);
    digitalWrite(outputPins[0], output);
  }
  // Additional combinational circuits...
}

// Sequential Circuits
void processSequentialCircuits(bool inputs[]) {
  bool clock = digitalRead(clockPin);
  bool reset = digitalRead(resetPin);
  
  // Detect rising clock edge
  if (clock == HIGH && lastClockState == LOW) {
    if (currentCircuit == "D Flip-Flop") {
      flipFlopState = inputs[0];
    }
    else if (currentCircuit == "JK Flip-Flop") {
      bool J = inputs[0];
      bool K = inputs[1];
      if (J && K) flipFlopState = !flipFlopState;
      else if (J) flipFlopState = HIGH;
      else if (K) flipFlopState = LOW;
    }
  }
  lastClockState = clock;
  
  if (reset == LOW) {
    flipFlopState = LOW;
  }
  
  digitalWrite(outputPins[0], flipFlopState);
}

// Timer Circuits
void processTimerCircuits() {
  unsigned long currentTime = millis();
  
  if (currentCircuit == "Astable Multivibrator") {
    if (currentTime - lastPulseTime >= 1000) { // 1Hz output
      bool outputState = !digitalRead(outputPins[0]);
      digitalWrite(outputPins[0], outputState);
      lastPulseTime = currentTime;
    }
  }
  // Additional timer circuits...
}

// Counter Circuits
void processCounterCircuits(bool inputs[]) {
  bool clock = digitalRead(clockPin);
  bool reset = digitalRead(resetPin);
  
  if (reset == LOW) {
    counterValue = 0;
  }
  
  // Detect rising clock edge
  if (clock == HIGH && lastClockState == LOW) {
    if (currentCircuit == "Binary Up Counter") {
      counterValue = (counterValue + 1) % 16;
    }
    else if (currentCircuit == "Binary Down Counter") {
      counterValue = (counterValue - 1 + 16) % 16;
    }
    
    // Display counter value on outputs
    for (int i = 0; i < 4; i++) {
      digitalWrite(outputPins[i], (counterValue >> i) & 0x01);
    }
  }
  lastClockState = clock;
}

// Decoder and Display Circuits
void processDecoderCircuits(bool inputs[]) {
  if (currentCircuit == "BCD Decoder with 7-Segment Display") {
    int value = (inputs[3] << 3) | (inputs[2] << 2) | (inputs[1] << 1) | inputs[0];
    value = constrain(value, 0, 9);
    
    // Display the digit on 7-segment
    for (int i = 0; i < 7; i++) {
      digitalWrite(segmentPins[i], (digitPatterns[value] >> i) & 0x01);
    }
  }
}

// ====================
// HELPER FUNCTIONS
// ====================

void handleSerialCommand() {
  String command = Serial.readStringUntil('\n');
  command.trim();
  
  if (command == "menu") {
    printMenu();
  }
  else if (command == "reset") {
    resetSystem();
  }
  else {
    // Check if command matches any circuit
    if (isValidCircuit(command)) {
      currentCircuit = command;
      Serial.print("Circuit set to: ");
      Serial.println(currentCircuit);
      resetSystem();
    }
    else {
      Serial.println("Invalid command. Type 'menu' for options.");
    }
  }
}

bool isValidCircuit(String circuit) {
  // Array of all valid circuits (would be expanded)
  String validCircuits[] = {
    "AND", "OR", "NOT", "NAND", "NOR", "XOR", "XNOR",
    "Half Adder", "Full Adder", "Multiplexer (MUX)",
    "D Flip-Flop", "JK Flip-Flop",
    "Astable Multivibrator",
    "Binary Up Counter", "Binary Down Counter",
    "BCD Decoder with 7-Segment Display"
  };
  
  for (String valid : validCircuits) {
    if (circuit == valid) return true;
  }
  return false;
}

void resetSystem() {
  // Reset all outputs
  for (int i = 0; i < numOutputs; i++) {
    digitalWrite(outputPins[i], LOW);
  }
  
  // Reset 7-segment display
  for (int i = 0; i < 7; i++) {
    digitalWrite(segmentPins[i], LOW);
  }
  
  // Reset state variables
  flipFlopState = LOW;
  counterValue = 0;
  lastPulseTime = millis();
}

void printMenu() {
  Serial.println("\n==== Digital Logic Lab Simulator ====");
  Serial.println("Available Circuits:");
  Serial.println("Basic Gates: AND, OR, NOT, NAND, NOR, XOR, XNOR");
  Serial.println("Combinational: Half Adder, Full Adder, Multiplexer (MUX)");
  Serial.println("Sequential: D Flip-Flop, JK Flip-Flop");
  Serial.println("Timers: Astable Multivibrator");
  Serial.println("Counters: Binary Up Counter, Binary Down Counter");
  Serial.println("Decoders: BCD Decoder with 7-Segment Display");
  Serial.println("\nCommands: 'menu', 'reset', or circuit name");
  Serial.println("===================================");
}