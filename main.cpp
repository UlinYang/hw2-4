#include "mbed.h"
#include "Ramp.hpp"
#include "uLCD_4DGL.h"
 

using namespace std::chrono;
Timer timer;
PwmOut PWM(LED1);
AnalogIn AO(A0);
DigitalOut IntrptOut(D3);
InterruptIn IntrptIn(D5);
uLCD_4DGL uLCD(D1, D0, D2);
EventQueue queue(32 * EVENTS_EVENT_SIZE);
Thread thread, ISR;
 
bool CurState = false;
float ValTmp;
float ThresholdRatio = 2 / 3.3;
int SampleCount = 0, SampleMulCount = 0, SampleMulCountBuf = 0;
float SampleSum = 0, SampleAvg = 0;;
 
 
 
void RiseHandler(){  
    //printf("Current Number of Rise: %d\n", CurNumRise);
    uLCD.text_width(4); 
    uLCD.text_height(4);
    uLCD.color(GREEN);
    uLCD.locate(0, 1);
    uLCD.printf("%2d", SampleMulCount*50);
}
void RiseISR(){
    queue.call(RiseHandler);
}
void FallHandler(){
    //printf("Current Number of Fall: %d\n", CurNumFall);
    uLCD.text_width(4); 
    uLCD.text_height(4);
    uLCD.color(RED);
    uLCD.locate(0, 1);
    uLCD.printf("%2d", SampleMulCount*50);
}
void FallISR(){
    queue.call(FallHandler);
}
 
 
void Sensor(){
    while(1){
        if(SampleCount++<50){
            SampleSum += AO.read();
        }
        else{
            SampleAvg = SampleSum / 50;
            SampleSum = AO.read();
            ++SampleMulCount;
            SampleCount = 0;
 
            if(SampleAvg>=ThresholdRatio && CurState==false){
                SampleMulCountBuf = SampleMulCount;
                IntrptOut.write(1);
                CurState = true;
            }
            else if(SampleAvg<ThresholdRatio && CurState==true){
                SampleMulCountBuf = SampleMulCount;
                IntrptOut.write(0);
                CurState = false;
            }
        }
 
        //printf("ADC Value: %f\n", AO.read());
        ThisThread::sleep_for(1ms);
    }
}
 
int main()
{   
    Ramp R(0.25, 0.5, 0.75);
    int SampleNum =  1000 / ((1) * (50));
    int count = 0;
    float *DutyCycles_ms = new float [SampleNum];
    float *DutyCycles_us = new float [SampleNum];
    PWM.period_ms(5);
 
    for(int i=0; i<SampleNum; i++){
        DutyCycles_ms[i] = 5 * R.Function((float)i/(float)SampleNum);
        DutyCycles_us[i] = 1000 * 5 * R.Function((float)i/(float)SampleNum);
        //printf("Duty Cycle Sample%d: R(%f) -> %f\n\r", i, (float)i/(float)SampleNum, R.Function((float)i/(float)SampleNum));
    }
 
    thread.start(Sensor); // 存取
    ISR.start(callback(&queue, &EventQueue::dispatch_forever)); // 執行 HANDALER
    IntrptIn.rise(RiseISR);
    IntrptIn.fall(FallISR);
    while (1){//INTRUP 不能占用太多 tread 所以要再丟給另一個THREAD 
        PWM.pulsewidth_us(DutyCycles_us[count]);// 設定 dutycycles 一個週期高電壓的時長
        count = (count==SampleNum) ? 0 : count+1;// ? IF ELSE
        ThisThread::sleep_for(50ms);
    }
}