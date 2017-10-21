// Sensor Setup - Adjust these values to calibrate your particular board.
bool DebugMode = true;            // Set it to false before connect to FC.
float ReferenceVoltage = 4.55;    // Voltage that powers the Arduino Board (usually from it's voltage regulator)
// Set the Voltage divider scale for each analog pin, according to the resistors used on each cell.
float DividerScale[] = {2.25, 3.29, 4.315, 5.462, 6.5, 7.6, 8.7, 9.8};




// Set the available analog pins for voltage monitoring (in the correct cell order)
int CellsPins[] = {A0, A1, A2, A3, A6, A7, A8, A9}; // Arduino Pro Micro
int CurrentSensorPin = A10; // Arduino Pro Micro

// Do Not Touch variables bellow.
float CellSumarizedVoltage[16];
float CellVoltage[16];
int BatteryCellCount = 0;
int BatteryNumber = 0;
float TotalCurrent = 0;
float ActualCurrent = 0;

void setup() {
  if(DebugMode)
    Serial.begin(9600); //This pipes to the serial monitor

}

void loop() {
  getCellCount();
  getAllCellsVoltage();
  
  if(DebugMode)
    printBatteryInformation();
  
  delay(1000);
}

int getCellCount() {
  int cellCount = 0;
  for (int i = 0; i < (sizeof(CellsPins)/sizeof(int)); i++) {
    if(getCellVoltage(i) >= 0.1)
      cellCount++;
    else
      break;
  }
  BatteryCellCount = cellCount;
  return cellCount;
}

void getAllCellsVoltage() {
  for (int i = 0; i < BatteryCellCount; i++) {
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
  if(BatteryCellCount==0)
    return 0;

  float voltage = 0;
  for (int i = 0; i < BatteryCellCount; i++) {
    if(CellVoltage[i] > voltage)
      voltage = CellVoltage[i];
  }
  return voltage;
}

float getLowerCellVoltage() {
  if(BatteryCellCount==0)
    return 0;
    
  float voltage = CellVoltage[0];
  for (int i = 0; i < BatteryCellCount; i++) {
    if(CellVoltage[i] < voltage)
      voltage = CellVoltage[i];
  }
  return voltage;
}

float getAverageCellVoltage() {
  if(BatteryCellCount==0)
    return 0;

  return getBatteryPackVoltage() / BatteryCellCount;
}

float getBatteryPackVoltage() {
  if(BatteryCellCount==0)
    return 0;
    
  float voltage = 0;
  for (int i = 0; i < BatteryCellCount; i++) {
      voltage = voltage + CellVoltage[i];
  }
  return voltage;
}

// ********************** PRINT FUNCTIONS **********************

void printBatteryInformation() {
  Serial.println("-----");
  Serial.print("Battery number: ");
  Serial.println(BatteryNumber);

  Serial.print("Cell count: ");
  Serial.println(BatteryCellCount);
  
  Serial.print("Battery voltage: ");
  Serial.println(getBatteryPackVoltage());

  Serial.print("Higher cell: ");
  Serial.println(getHigherCellVoltage());

  Serial.print("Lower cell: ");
  Serial.println(getLowerCellVoltage());

  Serial.print("Average cell: ");
  Serial.println(getAverageCellVoltage());

  Serial.print("mAh used: ");
  Serial.println(TotalCurrent);

  Serial.print("Current Amps: ");
  Serial.println(ActualCurrent);

}

void printCellVoltage(int cellNumber) {

  Serial.print("Cell ");
  Serial.print(cellNumber);
  Serial.print(": ");
  Serial.print(getCellVoltage(cellNumber));
  Serial.println("V");

}

void printAllCellsVoltage() {
  for (int i = 0; i < BatteryCellCount; i++) {
    printCellVoltage(i);
  }
}

