#include "MAX30102.h"
#include <Wire.h>

MAX30100 sensor;

boolean getInterruption = false;
unsigned long previousMillis = 0;
// constants won't change :
const long interval = 20; //this changes the sample rate time, 10 is fast, 20 is slow
        int thresh = 0;
        boolean Pulse, firstBeat, secondBeat;
        int BPM, IBI, P, T, amp;
        int Signal;
        int rate[10];
        unsigned long lastBeatTime, sampleCounter;
        int lastBPM;

void setup() {
  // put your setup code here, to run once:
  Wire.begin();
  Serial.begin(115200);
  while (!Serial);
  sensor.reset();
  sensor.init();
  attachInterrupt(digitalPinToInterrupt(2), interruption, FALLING);
  delay(1000);
}

void loop() {
  float temperature = 0;
  char buffer[35]; // you have to be aware of how long your data can be
                 // not forgetting unprintable and null term chars
  //sprintf(buffer,"var1:%i\tvar2:%i\tvar3:%i",var1,var2,var3);
  //Serial.println(buffer);
  static unsigned long upStartedTime, upStopedTime, IBI;
  static boolean upStarted = false;
  int IRData, HR, biggestIR, lowestIR;

  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= interval) {
    // save the last time you blinked the LED
    previousMillis = currentMillis;
    //sensor.GetTemp();
    sensor.readSensor();
    //Serial.println(meanDiff(sensor.IR));
    //sprintf(buffer,"%d %d",meanDiff(sensor.IR),meanDiff(sensor.RED));

    //This show the Heart rate numbers
    /*IRData = meanDiff(sensor.IR);
    getBPM();
    if(lastBPM != BPM){
      sprintf(buffer,"%d %d",0,BPM);
      Serial.println(buffer);
      lastBPM = BPM;
    }*/

    //This shows the graphic heart beats
    sprintf(buffer,"%d,",meanDiff(sensor.IR));
    Serial.println(buffer);
  }

  if (getInterruption) {
    getInterruption = false;
    int interruption = 0;
    interruption = sensor.GetStatus();
    //Serial.println("Interrupt!!!!");
    //Serial.println(interruption);

    if ( interruption & 0x80 ) {
      Serial.println("almost full!!!!!!!!\n");
      //sensor.readSensor();
    }
    if ( interruption & 0x40 ) {
      //Serial.println("temp ready");
      //Serial.println(sensor.ReadTemp());
    }
    if ( interruption & 0x20 ) {
      //Serial.println("heart rate ready\n");
      //sensor.readSensor();
    }
    if ( interruption & 0x10 ) {
      //Serial.println("spo2 ready\n");
      //sensor.readSensor();
    }
    if ( interruption & 0x01 ) {
      Serial.println("power ready");
      //MAX30100_Read();
    }
  }
}

void interruption()
{
  getInterruption = true;
}

long meanDiff(int M) {
#define LM_SIZE 15
  static int LM[LM_SIZE];      // LastMeasurements
  static byte index = 0;
  static long sum = 0;
  static byte count = 0;
  long avg = 0;

  // keep sum updated to improve speed.
  sum -= LM[index];
  LM[index] = M;
  sum += LM[index];
  index++;
  index = index % LM_SIZE;
  if (count < LM_SIZE) count++;

  avg = sum / count;
  return avg - M;
}

void getBPM(){

        Signal = meanDiff(sensor.IR);
        // We take a reading every 0.05 seconds
        sampleCounter = millis();                        // keep track of the time in mS with this variable
        int N = sampleCounter - lastBeatTime;       // monitor the time since the last beat to avoid noise

        // Find the peak and trough of the pulse wave
        if(Signal < thresh && N > (IBI/5)*3){       // avoid dichrotic noise by waiting 3/5 of last IBI
            if (Signal < T){                        // T is the trough
                T = Signal;                         // keep track of lowest point in pulse wave
            }
        }

        if(Signal > thresh && Signal > P){          // thresh condition helps avoid noise
            P = Signal;                             // P is the peak
        }                                           // keep track of highest point in pulse wave

        // Look for the heart beat
        if (N > 250){                               // avoid high frequency noise
            if ( (Signal > thresh) && (Pulse == false) && (N > (IBI/5)*3) ){
                Pulse = true;                       // set the Pulse flag when we think there is a pulse
                IBI = sampleCounter - lastBeatTime; // measure time between beats in mS
                lastBeatTime = sampleCounter;       // keep track of time for next pulse

                if(secondBeat){                     // if this is the second beat, if secondBeat == TRUE
                    secondBeat = false;             // clear secondBeat flag
                    for(int i=0; i<=9; i++){        // seed the running total to get a realisitic BPM at startup
                        rate[i] = IBI;
                    }
                }

                if(firstBeat){                      // if it’s the first time we found a beat, if firstBeat == TRUE
                    Serial.println("first beat");
                    firstBeat = false;              // clear firstBeat flag
                    secondBeat = true;              // set the second beat flag
                    return;                         // IBI value is unreliable so discard it
                }

                // Keep a running total of the last 10 IBI values
                int runningTotal = 0;               // clear the runningTotal variable
                for(int i=0; i<=8; i++){            // shift data in the rate array
                    rate[i] = rate[i+1];
                    runningTotal += rate[i];
                }

                rate[9] = IBI;   // add the latest IBI to the rate array
                runningTotal += rate[9]; // add the latest IBI to runningTotal
                runningTotal /= 10;                 // average the last 10 IBI values
                BPM = 60000/runningTotal;           // get the beats per minutes -> BPM
            }
        }

        if (Signal < thresh && Pulse == true){      // when the values are going down, the beat is over
            Pulse = false;                          // reset the Pulse flag so we can do it again
            amp = P - T;                            // get amplitude of the pulse wave
            thresh = amp/2 + T;                     // set thresh at 50% of the amplitude
            P = thresh;                             // reset these for next time
            T = thresh;
        }

        if (N > 2500){                              // if 2.5 seconds go by without a beat -> reset
            thresh = 250;                           // set thresh default
            P = 250;                                // set P default
            T = 250;                                // set T default
            lastBeatTime = sampleCounter;           // bring the lastBeatTime up to date
            firstBeat = true;                       // set these to avoid noise
            secondBeat = false;                     // when we get the heartbeat back
        }
}
