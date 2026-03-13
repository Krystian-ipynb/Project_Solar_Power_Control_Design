const int panelPin = A0;
const int shuntPin = A5;

const int loadMosfet = 10;
const int chargeMosfet = 5;
const int dischargeMosfet = 3;

const float Rshunt = 0.1;
const float Rload = 300.0;

const float PloadTarget = 0.12;

const float ADC_REF = 5.0;
const int ADC_RES = 1023;

const float dividerRatio = 4.0;

void setup() {

  pinMode(loadMosfet, OUTPUT);
  pinMode(chargeMosfet, OUTPUT);
  pinMode(dischargeMosfet, OUTPUT);

  Serial.begin(9600);
}

void loop() {

  float vPanelADC = analogRead(panelPin);
  float vAfterADC = analogRead(shuntPin);

  float Vpanel = (vPanelADC / ADC_RES) * ADC_REF * dividerRatio;
  float Vafter = (vAfterADC / ADC_RES) * ADC_REF * dividerRatio;

  float Vshunt = Vpanel - Vafter;
  float Ipanel = Vshunt / Rshunt;

  float Ppanel = Vpanel * Ipanel;

  float dutyLoad = 0;
  float dutyCharge = 0;
  float dutyDischarge = 0;

  bool batterySupplying = false;

  if (Ppanel >= PloadTarget) {

    float Vload = sqrt(PloadTarget * Rload);
    dutyLoad = Vload / Vpanel;

    if (dutyLoad > 1.0) dutyLoad = 1.0;

    dutyCharge = 1.0 - dutyLoad;
    dutyDischarge = 0;

  } 
  else {

    dutyLoad = 1.0;

    dutyDischarge = (PloadTarget - Ppanel) / PloadTarget;

    if (dutyDischarge > 1.0) dutyDischarge = 1.0;

    dutyCharge = 0;

    batterySupplying = true;
  }

  analogWrite(loadMosfet, dutyLoad * 255);
  analogWrite(chargeMosfet, dutyCharge * 255);
  analogWrite(dischargeMosfet, dutyDischarge * 255);

  float Pload = (Vpanel * dutyLoad) * (Vpanel * dutyLoad) / Rload;
  float Pcharge = Ppanel - Pload;

  Serial.print("Panel Voltage: ");
  Serial.print(Vpanel);
  Serial.print(" V  |  ");

  Serial.print("Panel Current: ");
  Serial.print(Ipanel);
  Serial.print(" A  |  ");

  Serial.print("Panel Power: ");
  Serial.print(Ppanel);
  Serial.print(" W  |  ");

  Serial.print("Load Power: ");
  Serial.print(Pload);
  Serial.print(" W  |  ");

  Serial.print("Battery Charge Power: ");
  Serial.print(Pcharge);
  Serial.print(" W  |  ");

  Serial.print("Battery Supplying: ");

  if (batterySupplying) {
    Serial.println("YES");
  } else {
    Serial.println("NO");
  }

  delay(200);
}