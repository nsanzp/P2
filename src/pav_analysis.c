#include <math.h>
#include "pav_analysis.h"

float compute_power(const float *x, unsigned int N) {
    int i;
    float pot_lin=0;
    float pot_db;

    for(i=0; i<N; i++){
        pot_lin+=x[i]*x[i];
    }

    pot_lin/=N;
    pot_db = 10*log10(pot_lin);
    return pot_db;
}

float compute_am(const float *x, unsigned int N) {
    int i;
    float amp=0;

    for(i=0; i<N; i++){
        amp+=fabs(x[i]);
    }
    return amp/=N;
}

float compute_zcr(const float *x, unsigned int N, float fm) {
    int i, zcr=0;

    for(i=1; i<N; i++){
        if(sgn(x[i])!=sgn(x[i-1])){
            zcr+=1;
        }
    }

    zcr=zcr*(fm/2/(N-1));
    return zcr;
}

int sgn(float x){
    if(x<0){
        return -1;
    } else if(x==0){
        return 0;
    } else {
        return 1;
    }
}