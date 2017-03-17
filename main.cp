//2sec window / 0.25sec moving
#include "mbed.h" 
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "fft4g.h"
#include "uLCD_4DGL.h" 
Serial pc(USBTX, USBRX); 
#define FS 64  										//sampling freq
#define LEN 128										//Length == FFT points
#define M_PI 3.1415926535
 
Timer time_comp; 
 
uLCD_4DGL uLCD(p9,p10,p11);
AnalogIn analog1(p20);
double power[LEN];
double frequency[LEN];
double freq;
float PSDA1, PSDA2, PSDAR;
int fft_counter, intPSDA1, intPSDA2;
 
int main(void){
    pc.baud(115200);
    int n,m,m1,m2,m3;
    n = LEN * 2;

    uLCD.baudrate(115200);
    uLCD.background_color(WHITE);
    uLCD.cls();
    uLCD.textbackground_color(WHITE);
    
    double a[600];
    double a_copy[600];
    int ip[600];
    double w[600];
    double a_buff[32];
    fft_counter = 0;
             
    wait(5);             
    for(m=0; m<600; m++){       							//0.015625*256 = 4
        a[m*2] = analog1.read();        						//a[0] - a[2LEN-2]
        a[m*2 + 1] = 0.0;								//a[1] - a[2LEN-1]
        wait(0.015625);
    }
    
    for(m=0; m<LEN; m++){
        a_copy[m*2] = a[m*2];
        a_copy[m*2 + 1] = a[m*2 + 1];
    }

    while(1){             
    //FFT
        cdft(n, -1, a_copy, ip, w);
        for (int i=0; i<LEN; i++){
            if (i < LEN/2) freq = (double)i * FS / LEN;				//freq = i * 64/128 = 0.5i
            else freq = (double) -(LEN - i - 1) * FS / LEN;         
            frequency[i] = freq; 
            power[i] = (a_copy[i*2]*a_copy[i*2] + a_copy[i*2+1]*a_copy[i*2+1]);    //i = 256 , freq = 64
        }
            

    //drawing PSD
    //Output:0 - 64Hz
    //32Hz, (k = 0~63)*(0.5Hz)
        uLCD.cls();
        for(int k=0; k<LEN; k++){
            uLCD.line(k, 128, k, 128 - power[k]*10, 0x000000);     			//  [Hz]
        }

        uLCD.locate(0,0);
        uLCD.color(BLACK);
        uLCD.printf("%d\n",fft_counter);

        fft_counter++;


        for(m1=0; m1<16; m1++){
            a_buff[m1] = analog1.read();
            wait(0.015625);
        }
        for(m2=0; m2<112; m2++){            						//a[0] <- a[16*2], a[1*2] <- a[17*2], ... , a[111*2] <- a[127*2]
            a[m2*2] = a[(m2+16)*2];
        } 
        for(m3=0; m3<16; m3++){             						//a[112*2] <- a_buff[0], a[112*2] <- a_buff[1], ... a[112*2] <- a_buff[15] 
            a[(m3+112)*2] = a_buff[m3];    
        }
        for(m=0; m<LEN; m++){
            a_copy[m*2] = a[m*2];
            a_copy[m*2 + 1] = a[m*2 + 1];
        } 
    }
}