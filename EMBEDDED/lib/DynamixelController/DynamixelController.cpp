#include "DynamixelController.hpp"

uint8_t rcvdPktUsb[MAX_LENGTH_OF_MESSAGE];
uint8_t posInArrayUsb = 0;
//boolean newData = false;
void readVelocities();
Uart1Event event1;
Uart2Event event2;
Uart3Event event3;

IntervalTimer txTimer1;
IntervalTimer txTimer2;
IntervalTimer txTimer3;

volatile uint8_t rcvdPkt1[MAX_LENGTH_OF_MESSAGE];
volatile uint8_t rcvdPkt2[MAX_LENGTH_OF_MESSAGE];
volatile uint8_t rcvdPkt3[MAX_LENGTH_OF_MESSAGE];

QueueArray <DynamixelMessage*> queue1(50);
QueueArray <DynamixelMessage*> queue2(50);
QueueArray <DynamixelMessage*> queue3(50);

volatile uint16_t posInArray1 = 0;
volatile uint16_t posInArray2 = 0;
volatile uint16_t posInArray3 = 0;

volatile uint16_t numOfBytesToRead1 = 4;
volatile uint16_t numOfBytesToRead2 = 4;
volatile uint16_t numOfBytesToRead3 = 4;

Vector<uint8_t> messageVector1;           //Vectors to hold the messages before putting them into the queues.
Vector<uint8_t> messageVector2;           //Should behave like the regular C++ standard-class vector-type.
Vector<uint8_t> messageVector3;

volatile bool sync1 = true;
volatile bool sync2 = true;
volatile bool sync3 = true;

volatile bool busy1=false;
volatile bool busy2=false;
volatile bool busy3=false;

volatile uint8_t resendCounter1=0;
volatile uint8_t resendCounter2=0;
volatile uint8_t resendCounter3=0;

volatile uint8_t idMap[256];
volatile bool usbMode=false;
volatile bool scanMode=false;

void scanPort() {
    scanMode = true;
    for (int i=0;i<254;i++){
        idMap[i]=0;
    }
    uint8_t search[16];
    for(int i=0;i<16;i++)
    {
        search[i]=0;
    }

    for(int k=0;k<8;k++){
        for (int j=32*k;j<32*(k+1);j++){
            search[0]=255;
            search[1]=255;
            search[2]=j;
            search[3]=4;
            search[4]=2;
            search[5]=3;
            search[6]=1;
            DynamixelMessage* ScanMessage1 = new DynamixelMessage(search); //messages are being generated and put
            DynamixelMessage* ScanMessage2 = new DynamixelMessage(search); //
            DynamixelMessage* ScanMessage3 = new DynamixelMessage(search); //

            queue1.push(ScanMessage1);                                                   //messages are being pushed
            queue2.push(ScanMessage2);                                                   //into their corresponding queues
            queue3.push(ScanMessage3);

        }

        send1();
        send2();
        send3();
        while(busy1 && busy2 && busy3){
            delay(10);                                                                 //Waiting for ports to finish writing all messages
        }
        scanMode=false;
    }
}


void initializeServos() {
    uint8_t setReturnDelayTime_1[8] = {255, 255, 1, 4, 3, 5, 1, 241};
    uint8_t setReturnDelayTime_2[8] = {255, 255, 2, 4, 3, 5, 1, 240};
    uint8_t setTorqueEnable_1[8] = {255, 255, 1, 4, 3, 24, 1, 222};
    uint8_t setTorqueEnable_2[8] = {255, 255, 2, 4, 3, 24, 1, 221};
    uint8_t setTorqueLimit_1[9] = {255, 255, 1, 5, 3, 34, 32, 3, 177};
    uint8_t setTorqueLimit_2[9] = {255, 255, 2, 5, 3, 34, 32, 3, 176};
    uint8_t setCcwAngleLimit_1[9] = {255, 255, 1, 5, 3, 8, 0, 0, 238};
    uint8_t setCcwAngleLimit_2[9] = {255, 255, 2, 5, 3, 8, 0, 0, 237};
    uint8_t setCwAngleLimit_1[9] = {255, 255, 1, 5, 3, 6, 0, 0, 240};
    uint8_t setCwAngleLimit_2[9] = {255, 255, 2, 5, 3, 6, 0, 0, 239};
    //uint8_t setLED[8] = {255, 255, 2, 4, 3, 25, 0, 219};    //for debug purposes

    writeToUART(setReturnDelayTime_1);
    writeToUART(setReturnDelayTime_2);
    writeToUART(setTorqueEnable_1);
    writeToUART(setTorqueEnable_2);
    writeToUART(setTorqueLimit_1);
    writeToUART(setTorqueLimit_2);
    writeToUART(setCcwAngleLimit_1);
    writeToUART(setCcwAngleLimit_2);
    writeToUART(setCwAngleLimit_1);
    writeToUART(setCwAngleLimit_2);
}

void initializeUART() {

    event1.txEventHandler = txEvent1;             //defines the function to trigger for sent bytes on Port 1 of the Teensy
    event1.rxEventHandler = rxEvent1;             //defines the function to trigger for received bytes on Port 1 of the Teensy
    event1.rxBufferSizeTrigger = 1;               //defines how many bytes have to enter the input buffer until the interrupt triggers the interrupt function
    event1.begin(1000000);                        //defines the baudrate for the corresponding port
    event1.clear();                               //clears

    event2.txEventHandler = txEvent2;
    event2.rxEventHandler = rxEvent2;
    event2.rxBufferSizeTrigger = 1;
    event2.begin(1000000);
    event2.clear();

    event3.txEventHandler = txEvent3;
    event3.rxEventHandler = rxEvent3;
    event3.rxBufferSizeTrigger=1;
    event3.begin(1000000);
    event3.clear();

    delay(4000);
    scanPort();
    delay(2000);
    //Serial.println("UART Initialization complete!");
}

void writeToUART(uint8_t* servoPckt) {
    DynamixelMessage* USBMessage = new DynamixelMessage(servoPckt);
    if (idMap[servoPckt[2]] == 1 || scanMode) {
        pushToQueue1(USBMessage);
    }
    else if (idMap[servoPckt[2]] == 2 || scanMode) {
        pushToQueue2(USBMessage);
    }
    else if (idMap[servoPckt[2]] == 3 || scanMode) {
        pushToQueue3(USBMessage);
    }
    else if (idMap[servoPckt[2]] == 0) {
        delete USBMessage;
    }
}


void convertToReadableVelocities(uint8_t* servoPckt) {
    int lowByte = 0, highByte = 0;
    static int velLeft = 0, velRight = 0;
    static boolean velLeftIsEmpty;
    static boolean velRightIsEmpty;

    if (servoPckt[0] == 1 && velLeftIsEmpty == false) {
        lowByte = static_cast<int>(servoPckt[1] & 255);
        highByte = static_cast<int>(servoPckt[2] << 8);
        velLeft = lowByte + highByte;
        velLeftIsEmpty = true;
        if (velLeft >= 1024) {
            velLeft -= 1024;
            velLeft *= (-1);
        } else {
            velLeft = velLeft;
        }
    }
    if (servoPckt[0] == 2 && velRightIsEmpty == false) {
        lowByte = static_cast<int>(servoPckt[1] & 255);
        highByte = static_cast<int>(servoPckt[2] << 8);
        velRight = lowByte + highByte;
        velRightIsEmpty = true;
        if (velRight > 1024) {
            velRight -= 1024;
        } else {
            velRight *= (-1);
        }
    }
    if (velLeftIsEmpty && velRightIsEmpty == true) {
        int velArray[2] = {0};
        velArray[0] = velLeft;
        velArray[1] = velRight;
        velLeft = 0, velRight = 0;
        velLeftIsEmpty = false;
        velRightIsEmpty = false;
        convertServoDataToJson(velArray);
    }
}

void servoReadPcktConstructor() {
    uint8_t servoPckt[8] {FF, FF, 0, _READ_LENGHT, _READ_SERVO_DATA,
        SERVO_REGISTER_PRESENT_SPEED, _NUM_OF_BYTES_TO_READ, 0};
    for (uint8_t id = 1; id < 3; id++) {
        uint8_t checkSum = 0;
        servoPckt[2] = id;
        for (size_t i = 2; i < sizeof servoPckt-1; i++) {
            checkSum += servoPckt[i];
        }
        checkSum = ~(checkSum) & 255;
    servoPckt[sizeof servoPckt-1] = checkSum;

    writeToUART(servoPckt);
    }
}

void servoWritePcktConstructor(Vector<int>& velArray) {
    uint8_t servoPckt[9] {FF, FF, 0, _WRITE_LENGHT, _WRITE_SERVO_DATA,
        SERVO_REGISTER_MOVING_SPEED, 0, 0, 0};
    for (uint8_t id = 1; id < 3; id++) {
        int val = 0;
        uint8_t checkSum = 0;
        servoPckt[2] = id;
        val = velArray.at(id-1);
        if (id == 1) {
            if (val < 0) {
                val *= -1;
                val += 1024;
            } else {
                val = val;
            }
        }
            if (id == 2) {
                if (val < 0) {
                    val *= -1;
                } else {
                    val += 1024;
                }
            }
        servoPckt[6] = val & 255;
        servoPckt[7] = val >> 8;
            for (size_t i = 2; i < sizeof servoPckt-1; i++) {
                checkSum += servoPckt[i];
            }
        checkSum = ~(checkSum) & 255;
        servoPckt[8] = checkSum;

        writeToUART(servoPckt);
    }
}

void parseJsonString(String s) {
    Vector<int> velVec;
    DynamicJsonBuffer jsonBuffer;
    JsonObject& root = jsonBuffer.parseObject(s);
    velVec.push_back(root["velLeft"]);
    velVec.push_back(root["velRight"]);
    servoWritePcktConstructor(velVec);
}

void convertServoDataToJson(int* dataArray) {
    DynamicJsonBuffer jsonBuffer;
    JsonArray& velArray = jsonBuffer.createArray();
    velArray.add(dataArray[0]);
    velArray.add(dataArray[1]);
    writeToUSB(velArray);    
}

void writeToUSB(JsonArray& velArray) {
    int len = velArray.measureLength();
    Serial.print('[');
    Serial.print(len);
    Serial.print(']');
    velArray.printTo(Serial);
    Serial.print('\n');
}

void readVelocities() {
    int n = 0;
    int array[2];
    char buf[20];
    Vector<int> velVec;
    DynamicJsonBuffer jsonBuffer;
    n = Serial.readBytesUntil(']', buf, sizeof buf);
    buf[n] = ']';
    JsonArray& velArray = jsonBuffer.parseArray(buf);
    velArray.copyTo(array);
    int velLeft = velArray[0];
    int velRight = velArray[1];
    velVec.push_back(velLeft);
    velVec.push_back(velRight);
    Serial.flush();
    servoWritePcktConstructor(velVec);
}

void requestHandler() {
    char requestByte = '0';
    while(Serial.available() > 0) {
        //Serial.readBytes(requestByte, 1);
        requestByte = Serial.read();
        switch(requestByte) {
            case sensorReadByte:
                readSensorData();
                break;
            case servoReadByte:
                servoReadPcktConstructor();
                break;
            case servoWriteByte:
                //readFromUSB();
                readVelocities();
                break;
        }
    }
}

void rxSerialEventUsb(){
 posInArrayUsb=0;
 if (!Serial.available()){
   return;
 }
 while(Serial.available()){

   if(posInArrayUsb==0){
     rcvdPktUsb[posInArrayUsb]=Serial.read();
     posInArrayUsb++;
   }
   if(posInArrayUsb>=3){
     if(rcvdPktUsb[0] == 255 && rcvdPktUsb[1] == 255){
       while(posInArrayUsb<rcvdPktUsb[3]+4){
         rcvdPktUsb[posInArrayUsb]=Serial.read();
         posInArrayUsb++;
       }
         if ((posInArrayUsb)==(rcvdPktUsb[3]+4)){
         uint8_t testchksum=0;
         for(int p=2;p<=rcvdPktUsb[3]+2;p++){                 //checksum test for message from USB
             testchksum=testchksum+rcvdPktUsb[p];
         }
         testchksum=~(testchksum)&255;
         if(testchksum!=rcvdPktUsb[posInArrayUsb-1]){
         }else if(testchksum==rcvdPktUsb[posInArrayUsb-1]){//Message seems to look good.Creation of Dynamixel Object and putting into Queue
           //Serial.println("Checksum correct, creating message object");
           DynamixelMessage* USBMessage=new DynamixelMessage(rcvdPktUsb);
           if(!USBMessage){
               return;
           }
           for(int i=0;i<=posInArrayUsb;i++)
           {
            // Serial.print(i);
            // Serial.print(": ");writeToUART
            // Serial.println(rcvdPktUsb[i]);
           }

           if (idMap[rcvdPktUsb[2]] == 1 || scanMode){
             pushToQueue1(USBMessage);
         }else if(idMap[rcvdPktUsb[2]] == 2 || scanMode){
             pushToQueue2(USBMessage);
         }else if(idMap[rcvdPktUsb[2]] == 3 || scanMode){
             pushToQueue3(USBMessage);
         }else if(idMap[rcvdPktUsb[2]] == 0){
             delete USBMessage;
           }
           posInArrayUsb=0;
           for(int i=0;i<MAX_LENGTH_OF_MESSAGE;i++)
           {
             rcvdPktUsb[i]=0;
           }
         }
       }
     }
   }else{
     if(posInArrayUsb<3){
       rcvdPktUsb[posInArrayUsb]=Serial.read();
       posInArrayUsb++;
       if(rcvdPktUsb[0] == 255 && rcvdPktUsb[1] == 255){//Found Packet prefix, trying to retreive ID+Length
         rcvdPktUsb[posInArrayUsb]=Serial.read();
         posInArrayUsb++;
         rcvdPktUsb[posInArrayUsb]=Serial.read();
         posInArrayUsb++;
       }
     }
   }
 }
}

void noMessageReceival( UartEvent*          event,
                        IntervalTimer*      timer,
                        volatile uint8_t*   resendCounter,
                        void(*sendFunctionPointer)(void),
                        Vector<uint8_t>*    messagevector){
    timer->end();
    if((*resendCounter)<2){
      event->write(messagevector->data(),messagevector->size());
      (*resendCounter)+=1;
    }else{
      (*resendCounter)=0;
      sendFunctionPointer();
    }
}



void noMessageReceival1(){
  noMessageReceival(&event1, &txTimer1, &resendCounter1,send1, &messageVector1);
}

void noMessageReceival2(){
    noMessageReceival(&event2, &txTimer2, &resendCounter2,send2, &messageVector2);
}

void noMessageReceival3(){
    noMessageReceival(&event3, &txTimer3, &resendCounter3,send3, &messageVector3);
}


void txEvent( IntervalTimer* timer,                                               //Timer interrupt that triggers a function if a message does not
              void (*noMessageReceivalFunctionPointer)(void)){                    //reply after the time specified in begin(). The timer uses Âµseconds as a unit.
  timer->priority(255);
  timer->begin(noMessageReceivalFunctionPointer,300);
}

void txEvent1(){
  txEvent(&txTimer1, noMessageReceival1);
}

void txEvent2(){
  txEvent(&txTimer2, noMessageReceival2);
}

void txEvent3(){
  txEvent(&txTimer3, noMessageReceival3);
}

void copyArray(volatile uint8_t* array) {
    uint8_t testArray[3]{0};
    testArray[0] = static_cast<uint8_t>(array[2]);
    testArray[1] = static_cast<uint8_t>(array[5]);
    testArray[2] = static_cast<uint8_t>(array[6]);
    if (testArray[0] != 0 && testArray[0] < 3) {
        convertToReadableVelocities(testArray);
    }
}

void rxEvent( UartEvent*          event,
              IntervalTimer*      timer,
              volatile bool*      sync,
              volatile uint16_t*  posInArray,
              volatile uint16_t*  numOfBytesToRead,
              volatile uint8_t*   rcvdPkt,
              uint8_t             uartInt,
              volatile uint8_t*   resendCounter,
              void (*rxResyncFunctionPointer)(void),
              void(*sendFunctionPointer)(void)){
                  Vector<int> statusPckt;
  if ((*sync) == true){
    if ((*posInArray) >= 4){
      (*numOfBytesToRead) = rcvdPkt[3] +4;
    } else {
      (*numOfBytesToRead) = 4;
    }
    while (event->available() && (*posInArray) < (*numOfBytesToRead)){
      rcvdPkt[*posInArray] = event->read();
      (*posInArray)+=1;
    }

    if ((*posInArray) > 4 && (*posInArray) >= rcvdPkt[3] + 4){//received a complete packet from Dynamixel
      timer->end();
      *resendCounter=0;

      uint8_t testchksum=0;
      for(int p=2;p<=rcvdPkt[3]+2;p++){
          testchksum=testchksum+rcvdPkt[p];
      }
      testchksum=~(testchksum)&255;
      if(testchksum!=rcvdPkt[*posInArray-1]){
        //Serial.println("Wrong checksum from Servo");
        event->setRxEventHandler(rxResyncFunctionPointer);
        (*posInArray) = 0;
        event->setRxBufferSizeTrigger(1);
      }
      //Serial.println("Correct checksum from Servo");
      if(scanMode){
        idMap[rcvdPkt[2]]=uartInt;
    }else if (usbMode){
        //Serial.println("Sending back this Message to USB :");
      }

      /*Serial.println(rcvdPkt[2]);
      Serial.println(rcvdPkt[5]);
      Serial.println(rcvdPkt[6]);*/
      //statusPckt.push_back(id);
      //readStatusPckt(rcvdPkt);
      //Serial.println("rxEvent");
      copyArray(rcvdPkt);
      //static int count;
      /*statusPckt.push_back(rcvdPkt[5]);
      statusPckt.push_back(rcvdPkt[6]);*/
      //count++;
      //Serial.println(count);
      //statusPckt.clear();
      event->setRxBufferSizeTrigger(4);
      (*posInArray) = 0;
      sendFunctionPointer();


    }else{
      if ((*posInArray) >= 4){
        (*numOfBytesToRead) = rcvdPkt[3] + 4;
      }
      event->setRxBufferSizeTrigger((*numOfBytesToRead) - (*posInArray));
      if ((event->getRxBufferSizeTrigger() > 100) | (event->getRxBufferSizeTrigger() <= 0)){
        (*sync) = false;
        event->setRxEventHandler(rxResyncFunctionPointer);
        (*posInArray) = 0;
        event->setRxBufferSizeTrigger(1);
      }
    }
  }
}


void rxEvent1(){
  rxEvent(&event1, &txTimer1, &sync1, &posInArray1, &numOfBytesToRead1, rcvdPkt1,1,&resendCounter1, rxResync1, send1);
}

void rxEvent2(){
  rxEvent(&event2, &txTimer2, &sync2, &posInArray2, &numOfBytesToRead2, rcvdPkt2,2,&resendCounter2, rxResync2, send2);
}

void rxEvent3(){
  rxEvent(&event3, &txTimer3, &sync3, &posInArray3, &numOfBytesToRead3, rcvdPkt3,3,&resendCounter3, rxResync3, send3);
}


void rxResync( UartEvent*          event,
              volatile bool*      sync,
              volatile uint16_t*   posInArray,
              volatile uint8_t*   rcvdPkt,
              void (*rxEventFunctionPointer)(void)){
  if (!event->available()) {
    return;
  }
  rcvdPkt[*posInArray] = ((uint8_t) event->read());

  (*posInArray)+=1;
  if ((*posInArray) >= 4){
    if (rcvdPkt[0] == 255 && rcvdPkt[1] == 255){
      (*sync) = true;
      event->setRxBufferSizeTrigger(rcvdPkt[3] + 4);

      event->setRxEventHandler(rxEventFunctionPointer);
    } else {

      rcvdPkt[0] = rcvdPkt[1];
      rcvdPkt[1] = rcvdPkt[2];
      rcvdPkt[2] = rcvdPkt[3];
      (*posInArray)-=1;
    }
  }

}


void rxResync1(){
  rxResync(&event1, &sync1, &posInArray1, rcvdPkt1, rxEvent1);
}

void rxResync2(){
  rxResync(&event2, &sync2, &posInArray2, rcvdPkt2, rxEvent2);
}

void rxResync3(){
  rxResync(&event3, &sync3, &posInArray3, rcvdPkt3, rxEvent3);
}


void send(UartEvent*                      event,
          QueueArray<DynamixelMessage*>*  queue,
          volatile uint8_t*               resendCounter,
          volatile bool*                  busy,
          Vector<uint8_t>*                messagevector){
  if (!queue->isEmpty()){
    *busy=true;
    DynamixelMessage* message=queue->pop();
    message->assemblePacketfromArray(messagevector);
    event->write(messagevector->data(),messagevector->size());
    delete message;
  }else{
    *busy=false;
  }
}

void send1(){
  send(&event1, &queue1, &resendCounter1,&busy1, &messageVector1);
}

void send2(){
  send(&event2, &queue2, &resendCounter2,&busy2, &messageVector2);
}

void send3(){
  send(&event3, &queue3, &resendCounter3,&busy3, &messageVector3);
}


void pushToQueue( QueueArray<DynamixelMessage*>*  queue,
                  DynamixelMessage*               messageToPush,
                  volatile bool*                  busy,
                  void (*sendFunctionPointer)(void)){
  queue->push(messageToPush);
  if(!(*busy)){
    sendFunctionPointer();
  }
}

void pushToQueue1(DynamixelMessage* messageToPush){
    pushToQueue(&queue1,messageToPush,&busy1,send1);
}

void pushToQueue2(DynamixelMessage* messageToPush){
    pushToQueue(&queue2,messageToPush,&busy2,send2);
}

void pushToQueue3(DynamixelMessage* messageToPush){
    pushToQueue(&queue3,messageToPush,&busy3,send3);
}
