// Sensor Setup - Adjust these values to calibrate your particular board.
bool DebugMode = false;            // Set it to false before connect to FC.
float ReferenceVoltage = 4.55;    // Voltage that powers the Arduino Board (usually from it's voltage regulator)
// Set the Voltage divider scale for each analog pin, according to the resistors used on each cell.
float DividerScale[] = {2.25, 3.29, 4.315, 5.462, 6.5, 7.6, 8.7, 9.8};




// Set the available analog pins for voltage monitoring (in the correct cell order)
// Arduino Pro Micro
const int CellsPins[] = {A0, A1, A2, A3, A6, A7, A8, A9};
const int CurrentSensorPin = A10;                         

// Do Not Touch variables bellow.
float CellSumarizedVoltage[16];
float CellVoltage[16];
int thisBatteryCellCount = 0;
int thisBatteryNumber = 0;
float thisBatterymAhUsed = 0;
float thisBatteryAmpsDraw = 0;
unsigned long prevBattetyPoolingMilis = 0;
unsigned long prevSendDataMilis = 0;
unsigned long prevSendVersionMilis = 0;

// Pool time for battery information
const long batteryPoolingInterval = 500;
const long sendDataInterval = 100;
const long cellCountLockTimeout = 5 * 1000; // 5 seconds
const long sendVersionInfoInterval = 10 * 1000; // 10 seconds

// 12-byte data package
typedef struct {
  uint8_t PackNumber;          // 1 byte (max value 255)
  uint8_t CellCount;           // 1 byte (max value 255)
  uint16_t PackVoltage;        // 2 bytes (2 decimal digits, max value 655.35)
  uint16_t HigherCellVoltage;  // 2 bytes (2 decimal digits, max value 655.35)
  uint16_t LowerCellVoltage;   // 2 bytes (2 decimal digits, max value 655.35)
  uint16_t TotalmAhUsed;       // 2 bytes (no decimal digits, max value 65535)
  uint16_t AmpsDraw;           // 2 bytes (2 decimal digits, max value 655.35)
} BatteryPackInformation;

void setup() {
  if(DebugMode)
    Serial.begin(9600); // This pipes to the serial monitor

  // 9600 bps is fine since each battery outputs only 20 bytes twice per second.
  Serial1.begin(9600); // This pipes to FC
}

void loop() {
  unsigned long currentMillis = millis();

  if (currentMillis - prevSendVersionMilis >= sendVersionInfoInterval) {
    prevSendVersionMilis = currentMillis;
    send_protocol_version_info();
  }
  
  if (currentMillis - prevBattetyPoolingMilis >= batteryPoolingInterval) {
    prevBattetyPoolingMilis = currentMillis;
    getCellCount();
    getAllCellsVoltage();
  }
  
  if (currentMillis - prevSendDataMilis >= sendDataInterval) {
    if(DebugMode) {
      printBatteryInformation();
      printAllCellsVoltage();
      printAllCellsRawVoltage();
    }
  
    outputBatteryInformation();
  }
}

int getCellCount() {
  if(cellCountLockTimeout > millis() || thisBatteryCellCount == 0) {
    int cellCount = 0;
    for (int i = 0; i < (sizeof(CellsPins)/sizeof(int)); i++) {
      if(getCellVoltage(i) >= 0.5)
        cellCount++;
      else
        break;
    }
    thisBatteryCellCount = cellCount;
    return cellCount;
  } else {
    return thisBatteryCellCount;
  }
}

void getAllCellsVoltage() {
  for (int i = 0; i < thisBatteryCellCount; i++) {
    getCellVoltage(i);
  }
}

float getCellVoltage(int cellNumber) {
  int sensorValue = analogRead(CellsPins[cellNumber]);
  float voltage = sensorValue * (ReferenceVoltage / 1023.00) * DividerScale[cellNumber];

  CellSumarizedVoltage[cellNumber] = voltage;
  
  if(cellNumber==0)
    CellVoltage[cellNumber] = voltage;
  else
    CellVoltage[cellNumber] = voltage - CellSumarizedVoltage[cellNumber-1];

  return CellVoltage[cellNumber];
}

float getHigherCellVoltage() {
  if(thisBatteryCellCount==0)
    return 0;

  float voltage = 0;
  for (int i = 0; i < thisBatteryCellCount; i++) {
    if(CellVoltage[i] > voltage)
      voltage = CellVoltage[i];
  }
  return voltage;
}

float getLowerCellVoltage() {
  if(thisBatteryCellCount==0)
    return 0;
    
  float voltage = CellVoltage[0];
  for (int i = 0; i < thisBatteryCellCount; i++) {
    if(CellVoltage[i] < voltage)
      voltage = CellVoltage[i];
  }
  return voltage;
}

float getAverageCellVoltage() {
  if(thisBatteryCellCount==0)
    return 0;

  return getBatteryPackVoltage() / thisBatteryCellCount;
}

float getBatteryPackVoltage() {
  if(thisBatteryCellCount==0)
    return 0;
    
  float voltage = 0;
  for (int i = 0; i < thisBatteryCellCount; i++) {
      voltage = voltage + CellVoltage[i];
  }
  return voltage;
}

// ********************** PRINT FUNCTIONS **********************

void printBatteryInformation() {
  Serial.println("-----");
  Serial.print("Battery number: ");
  Serial.println(thisBatteryNumber);

  Serial.print("Cell count: ");
  Serial.println(thisBatteryCellCount);
  
  Serial.print("Battery voltage: ");
  Serial.println(getBatteryPackVoltage());

  Serial.print("Higher cell: ");
  Serial.println(getHigherCellVoltage());

  Serial.print("Lower cell: ");
  Serial.println(getLowerCellVoltage());

  Serial.print("Average cell: ");
  Serial.println(getAverageCellVoltage());

  Serial.print("mAh used: ");
  Serial.println(thisBatterymAhUsed);

  Serial.print("Current Amps: ");
  Serial.println(thisBatteryAmpsDraw);

}

void printCellVoltage(int cellNumber) {

  Serial.print("Cell ");
  Serial.print(cellNumber);
  Serial.print(": ");
  Serial.print(getCellVoltage(cellNumber));
  Serial.println("V");

}

void printAllCellsVoltage() {
  for (int i = 0; i < thisBatteryCellCount; i++) {
    printCellVoltage(i);
  }
}

void printAllCellsRawVoltage() {
  for (int i = 0; i < thisBatteryCellCount; i++) {
  Serial.print("RAW Cell ");
  Serial.print(i);
  Serial.print(": ");
  Serial.print(CellSumarizedVoltage[i]);
  Serial.println("V");
  }
}

void outputBatteryInformation() {
  BatteryPackInformation thisPack;
  thisPack.PackNumber = thisBatteryNumber;
  thisPack.CellCount = thisBatteryCellCount;
  thisPack.PackVoltage = getBatteryPackVoltage() * 100;
  thisPack.HigherCellVoltage = getHigherCellVoltage() * 100;
  thisPack.LowerCellVoltage = getLowerCellVoltage() * 100;
  thisPack.TotalmAhUsed = thisBatterymAhUsed;
  thisPack.AmpsDraw = thisBatteryAmpsDraw;

  send_data_packet(thisPack);
}

void send_data_packet(BatteryPackInformation thisPack){
  // Send a Starting char and an Ending char with the pack between - Total 14 byte data packet
  Serial1.write('S');
  Serial1.write((uint8_t *)&thisPack, sizeof(thisPack));
  Serial1.write('E');
  return;
}

void send_protocol_version_info() {
  Serial1.write("VER");
  Serial1.write("0001"); // Version Number
  Serial1.write("E");
}

