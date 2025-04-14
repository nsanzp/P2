#include <math.h>
#include <stdlib.h>
#include <stdio.h>

#include "pav_analysis.h"
#include "vad.h"

const float FRAME_TIME = 10.0F; /* in ms. */

/* 
 * As the output state is only ST_VOICE, ST_SILENCE, or ST_UNDEF,
 * only this labels are needed. You need to add all labels, in case
 * you want to print the internal state in string format
 */

const char *state_str[] = {
  "UNDEF", "S", "V", "INIT"
};

const char *state2str(VAD_STATE st) {
  return state_str[st];
}

/* Define a datatype with interesting features */
typedef struct {
  //float zcr;
  float p;
  //float am;
  float threshold0;
} Features;

/* 
 * TODO: Delete and use your own features!
 */

Features compute_features(const float *x, int N) { //modificar esto.
  /*
   * Input: x[i] : i=0 .... N-1 
   * Ouput: computed features
   */
  /* 
   * DELETE and include a call to your own functions
   *
   * For the moment, compute random value between 0 and 1 
   */
  Features feat;
  float N_init = 0.18 / (FRAME_TIME * 1e-3);//18 trames.
  feat.threshold0 = compute_power(x,N_init);
   feat.p = compute_power(x,N);//media de las potencias en decibelios
                                //(mejor resultado con potencia en dB).
  //feat.zcr = feat.p = feat.am = (float) rand()/RAND_MAX;
  return feat;
}

/* 
 * TODO: Init the values of vad_data
 */

VAD_DATA * vad_open(float rate) {
  VAD_DATA *vad_data = malloc(sizeof(VAD_DATA));
  vad_data->state = ST_INIT;
  vad_data->sampling_rate = rate;
  vad_data->frame_length = rate * FRAME_TIME * 1e-3;

  vad_data->p0=0;//inicialització a FSA.
  vad_data->k0=0;//nivel de referencia del ruido de fondo.
  vad_data->k1=0;//nivel de posibilidad de voz. (k0+alpha1)
  vad_data->k2=0;//nivel de confirmación de voz. (k1+alpha2)
  vad_data->alpha1=5;
  vad_data->alpha2=0;
  vad_data->lmin_sil=0.18;//segundos
  vad_data->lmin_voz=1.6;//segundos
  return vad_data;
}

VAD_STATE vad_close(VAD_DATA *vad_data) {
  /* 
   * TODO: decide what to do with the last undecided frames
   */
  VAD_STATE state = vad_data->state;

  free(vad_data);
  return state;
}

unsigned int vad_frame_size(VAD_DATA *vad_data) {
  return vad_data->frame_length;
}

/* 
 * TODO: Implement the Voice Activity Detection 
 * using a Finite State Automata
 */

VAD_STATE vad(VAD_DATA *vad_data, float *x, float alpha0) {

  /* 
   * TODO: You can change this, using your own features,
   * program finite state automaton, define conditions, etc.
   */

  Features f = compute_features(x, vad_data->frame_length); //calculamos features de la trama.
  vad_data->last_feature = f.p; /* save feature, in case you want to show */

  //k1 = 10 dB; por debajo del mínimo de potencia de sonidos fricativos sordos, en nuestro caso 10,37 dB
  //k2 duración máxima de espera. 0,312s

  switch (vad_data->state) { 
  case ST_INIT:
    vad_data->state = ST_SILENCE;
    vad_data->k0 = f.threshold0;
    vad_data->k1 = vad_data->k0 + vad_data->alpha1;
    vad_data->k2 = vad_data->k1 + vad_data->alpha2;
    break;

  case ST_SILENCE:
    if (f.p > vad_data->k1){
      vad_data->state = ST_UNDEF;
        if (f.p > vad_data->k2){
          vad_data->state = ST_VOICE;
        }
    }
    break;

  case ST_VOICE: 
  if (f.p < vad_data->k2){
    vad_data->state = ST_UNDEF;
      if (f.p < vad_data->k1){
        vad_data->state = ST_SILENCE;
      }
  }
    break;

  case ST_UNDEF:
    break;
  }

  if (vad_data->state == ST_SILENCE ||
      vad_data->state == ST_VOICE)
    return vad_data->state;
  else
    return ST_UNDEF;
}

void vad_show_state(const VAD_DATA *vad_data, FILE *out) {
  fprintf(out, "%d\t%f\n", vad_data->state, vad_data->last_feature);
}//fijar umbral en función de ruido de fondo.