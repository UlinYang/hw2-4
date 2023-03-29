#ifndef _RAMP_ 
 #define _RAMP_ 
 
 class Ramp{ 
     private: 
         float RaiseEnd; 
         float HighEnd; 
         float FallEnd; 
         float RaiseSlope; 
         float FallSlope; 
     public: 
         Ramp(){ 
             RaiseEnd = 0.25; 
             HighEnd = 0.5; 
             FallEnd = 0.75; 
             RaiseSlope = (float)(1/RaiseEnd); 
             FallSlope = (float)((-1)/(FallEnd-HighEnd)); 
         } 
         Ramp(float _RaiseEnd, float _HighEnd, float _FallEnd){ 
             RaiseEnd = _RaiseEnd; 
             HighEnd = _HighEnd; 
             FallEnd = _FallEnd; 
             RaiseSlope = (float)(1/RaiseEnd); 
             FallSlope = (float)((-1)/(FallEnd-HighEnd)); 
         } 
          
         float Function(float T){ 
             if(T<=RaiseEnd) 
                 return T * RaiseSlope; 
             else if(T<=HighEnd) 
                 return 1; 
             else if(T<=FallEnd) 
                 return 1 + (T-HighEnd) * FallSlope; 
             else 
                 return 0; 
         } 
 
         ~Ramp(){} 
 
 }; 
 
 
 #endif