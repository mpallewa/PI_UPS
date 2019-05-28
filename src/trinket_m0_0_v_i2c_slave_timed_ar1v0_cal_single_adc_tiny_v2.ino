#include <Wire.h>

#define INTERVAL 1000
#define V_BAT_THRESHOLD  2621 // approx. 3.2v (5:1 divider)

// #define DEBUG
// #define TEST

#define IGNORE_THR 60 /* Pi is ignoring the command to shutdown and keeps polling via I2C */
#define FAIL_THR   30 /* Pi is not polling after the shutdown command was issued */

int controlPin = 1;
int ledPin = 13;
int sensorPinUSB = 3;
int sensorPinBAT = A4;

unsigned int usbPower, sensorValueBAT, failCount = 0,
                                             ignoreCount = 0, turnOff = LOW, turnOffStatus = LOW;
unsigned long readCount = 0L, prevReadCount = -1L;
unsigned char data[3];
unsigned long lastRun = 0L, runCount = 0L, t;

#ifdef TEST
unsigned long testCount = 0L;
#endif

void setup() {
  pinMode(ledPin, OUTPUT);
  pinMode(controlPin, OUTPUT);
  digitalWrite(ledPin, LOW);
  data[2] = 0;
  failCount = 0;
  ignoreCount = 0;
  readCount = 0L;
  prevReadCount = -1L;
  lastRun = 0L;
  runCount = 0L;
  turnOff = LOW;
  // analogReference(AR_INTERNAL1V0);
  ADC->INPUTCTRL.bit.GAIN = ADC_INPUTCTRL_GAIN_1X_Val;
  ADC->REFCTRL.bit.REFSEL = ADC_REFCTRL_REFSEL_INT1V_Val;
  analogReadResolution(12);
  digitalWrite(controlPin, !turnOff);
  turnOffStatus = turnOff;
  Wire.begin(0x20);             // join i2c bus with address 0x20
  Wire.onRequest(requestEvent);
  Wire.onReceive(receieveEvent);
  Serial.begin(9600);
  while (!Serial && millis() < 5000) {
    delay(100);
  }

}

// function that executes whenever data is requested by master
// this function is registered as an event, see setup()
void requestEvent()
{
  readCount++;
  Wire.write(data, 3);
}

void receieveEvent(int x) {

}

void loop() {
  if (((t = millis()) - lastRun) >= INTERVAL) {
    lastRun = t;
    noInterrupts();
    sensorValueBAT = analogRead(sensorPinBAT);
    usbPower = digitalRead(sensorPinUSB);

#ifdef TEST
    if (testCount >= 120) {
      sensorValueBAT = V_BAT_THRESHOLD - 1;
    }
#endif
    data[0] = sensorValueBAT & 0xff;
    data[1] = (sensorValueBAT >> 8) & 0xff;

    if (usbPower) {
      data[2] &= ~0x1;
      data[2] |=  0x2;
      ignoreCount = 0;
      if ((failCount > 0) && (turnOffStatus == LOW)) {
        failCount = FAIL_THR; // 1 Sec power cycle if not off
      } else {
        failCount = 0;
      }
      digitalWrite(ledPin, LOW);
#ifdef TEST
      testCount = 0L;
#endif
    } else {
      data[2] |= (sensorValueBAT < V_BAT_THRESHOLD) ? 0x1 : 0x0;
      data[2] &=  ~0x2;
#ifdef TEST
      testCount++;
#endif
    }
    if (readCount == prevReadCount) {
      if ((data[2] & 1) && (turnOffStatus == LOW)) {
        failCount++;
      }
    } else {
      prevReadCount = readCount;
      failCount = 0;

      if ((data[2] & 1) && (turnOffStatus == LOW)) {
        ignoreCount++;
      } else {
        ignoreCount = 0;
      }
    }
    interrupts();

    if ((ignoreCount >= IGNORE_THR) || (failCount >= FAIL_THR)) {
      turnOff = HIGH;
    } else {
      turnOff = LOW;
    }
#ifdef DEBUG
    Serial.println("");
    Serial.print("\nUSB POW = "); Serial.print(usbPower);
    Serial.print("BAT RAW = "); Serial.print(sensorValueBAT);
    Serial.print("  BAT = "); Serial.println((sensorValueBAT * 5.0) / 4096);
    Serial.print("SEC = "); Serial.print(runCount);
    Serial.print(" READ = "); Serial.print(readCount);
    Serial.print(" FC = "); Serial.print(failCount);
    Serial.print(" IC = "); Serial.print(ignoreCount);
    Serial.print(" OFF = "); Serial.print(turnOffStatus);
    Serial.print("  STATUS = "); Serial.println(data[2]);
#endif
#ifdef TEST
    Serial.print("TC = "); Serial.print(testCount);
#endif

    if (turnOff != turnOffStatus) {
      digitalWrite(controlPin, !turnOff);
      turnOffStatus = turnOff;
    }
    if (data[2] & 1) {
      digitalWrite(ledPin, runCount & 1);
    }
    runCount++;

  } else {
    delay(10);
  }
}
