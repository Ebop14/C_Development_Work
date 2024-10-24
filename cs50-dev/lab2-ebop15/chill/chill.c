#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <math.h>
#include <stdbool.h>

// calculates the wind chill from parameters
float calculateChill(const float temp, const float velocity){
    float chill = 35.74 + (0.621 * temp) - (35.75 * pow(velocity, 0.16)) + 0.4265 * temp * pow(velocity, 0.16);
    return chill;    
}


// builds the table if no inputs
void noArgumentTable(void) {
    printf("Temp    Wind    Chill\n");
    printf("----    ----    ----\n");
    float temp = -10.0;
    while(temp < 40.0){
        float velocity = 5.0;
        while(velocity <= 15){
            float chill = calculateChill(temp, velocity);
            printf("%0.1f    %0.1f    %0.1f \n", temp, velocity, chill);
            velocity += 5;
        }
        temp += 10;
    }
}


// builds table if one argument
void oneArgumentTable(const float temp){
    printf("Temp    Wind    Chill\n");
    printf("----    ----    ----\n");
    float velocity = 5.0;
    while(velocity <= 15){
        float chill = calculateChill(temp, velocity);
        printf("%0.1f    %0.1f    %0.1f \n", temp, velocity, chill);
        velocity += 5;
    }
}


// main function
int
main(const int argc, const char *argv[]) {
    //Wind Chill = 35.74 + 0.6215T - 35.75(V^0.16) + 0.4275T(V^0.16)
    // T is temperature in degrees
    // V is wind velocity in mph
    if(argc == 1){
        noArgumentTable();        
        return 0;
    }
    // if there is one argument, temperature
    else if(argc == 2){
        float temp = atof(argv[1]);
        if(temp < 50 && temp > -99){
            oneArgumentTable(temp);
        }
        else{
            printf("Please specify a temperature between -99 and 50\n");
            return 2;
        }
    }
    // if there are two arguments, temperature and wind velocity
    else if(argc == 3){
        float temp = atof(argv[1]);
        float velocity = atof(argv[2]);
        if(temp > 50 || temp < -99){
            printf("Please specify a temperature between -99 and 50\n");
            return 2;
        }
        else if(velocity < 0.5 || velocity > 231){
            printf("Please specify a velocity between 0.5 and 231\n");
            return 3;
        }
        else{  
            float chill = calculateChill(temp, velocity);
            printf("Temp    Wind    Chill\n");
            printf("----    ----    ----\n");
            printf("%4.1f  %4.1f  %4.1f\n", temp, velocity, chill);
        }
    }
    // too many arguments
    else {
        printf("too many arguments \n");
        return 1;
    }
}