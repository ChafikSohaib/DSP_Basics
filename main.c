#include "dsk6713_aic23.h"
#include "math.h"
#include "FiltreBasseBand_18500_22000_400Coef.inc"
#include "TablesPulsesAcoustiques.inc"

Uint32 input_sample();
void output_sample(int out_data);
void comm_poll();

#define    RC     				7
#define    SEUIL_Detection     	100

Uint32 fs = DSK6713_AIC23_FREQ_48KHZ;		// Frequence d'echantillonnage Fs = 48000


const short Coeff[8]= {-24636,-25997,-27246,-28378,-29389,-30274,-31029,-31651};
Uint16 TabTrace[8];

short Xn,Yn;
short XnBuf[N+1],YnBuf[N];


short i, j,f,fmax,Fx,Np;

Uint16 DetectionCanalLibre = 0, DetectionPulse = 0;

short IntgC;
short Qn[8], Qn_1[8], Qn_2[8];

Uint32 Module[8], ModuleMaximum, ModuleMaximumSuivant;

int tmp1, tmp2;													// Pour les calculs intermédiaires
Uint32 t; 														// Indice de boucle dépassant +32768



main() {

	comm_poll();
	for (f=0;f<8;f++)
		TabTrace[f]=0;

	DetectionCanalLibre = 0;
	while(!DetectionCanalLibre){									// Attente détection d'un canal libre

		for (Np=0;Np<10;Np++){
			IntgC = 0;

			for (i=0;i<N;i++){
				XnBuf[i] = 0;
				YnBuf[i] = 0;
			}

			DetectionPulse = 0;
			t = 0;
			while (!DetectionPulse && t<48000){								// Attente détection d'un pulse pendant une seconde
				Xn = input_sample();
				t++;
				XnBuf[0] = Xn;
				Yn = 0;

				for (i=N-1;i>=0;i--){

					Yn = Yn + (int ) ((h[i]*XnBuf[i]) >> k) ;
					XnBuf[i+1]=XnBuf[i];

				}

				IntgC = IntgC + (short)(( abs(Yn) - IntgC ) >> 7);
				YnBuf[j]= (short) IntgC;

				j = (j + 1) % N;

				if ( IntgC > SEUIL_Detection )
					DetectionPulse = 1;

			}
			if (t < 48000) {

				for (f=0;f<8;f++) {
					Qn_1 [f] = 0 , Qn_2 [f] = 0;
				}

				for (i=0;i<96;i++) {

					Xn = input_sample();
					for (f=0;f<8;f++){

						tmp1 = (int) (Coeff [f]*Qn_1 [f]);							// Q8 * Q14 = Q22
						tmp1 = tmp1 + (int)(Xn << 7) ; 								// Q22 + (Q15 << 7) = Q22
						tmp1 = tmp1 - (int)(Qn_2 [f]<< 14) ; 						// Q22 + (Qn_2[f] << 14) = Q22

						Qn[f] = (short)(tmp1 >> 14);								// Q22 >> 14 = Q8
						Qn_2 [f] = Qn_1 [f];
						Qn_1 [f] = Qn [f];

					}

				}

				ModuleMaximum = 0;
				ModuleMaximumSuivant = 0;

				for (f=0;f<8;f++){

					tmp1 = (int)(Qn_1 [f] * Qn_1 [f]);									// Q8 * Q8 = Q16
					tmp1 = (int)(Qn_2 [f] * Qn_2 [f])  + tmp1;							// (Q8 * Q8) + Q16 = Q16
					tmp2 = (int) ((Qn_2 [f] * Coeff [f] ) >> 6) ;						// (Q8 * Q14) >> 6 = Q16
					tmp2 = (int)((tmp2 * Qn_1 [f]) >> 8 );								// (Q16 * Q8) >> 8 = Q16

					Module [f] = (Uint32)(tmp1 + tmp2);									// Q16

					if (Module [f] > ModuleMaximum) {

						ModuleMaximum = Module [f];
						fmax = f;

					}

					if (Module [f] != ModuleMaximum && Module [f] > ModuleMaximumSuivant) {

						ModuleMaximumSuivant = Module [f];

					}

				}

				if (ModuleMaximum > ModuleMaximumSuivant * 16) {

						TabTrace[fmax] = 1;

				}

			}		// fin if t < 48000




		}	// fin boucle Np

		for (f=0;f<8;f++){
			if ( TabTrace[f] == 0 ) {
				DetectionCanalLibre = 1;
				Fx = f;
			}
		}
	}			// fin boucle while1

	while (1) {

		for (t=0;t<96;t++) {

			output_sample(TablePulse[Fx][t]);

		}

		for (t=0;t<(48000-96);t++) {

			output_sample(0);

		}

	}			// fin boucle while1

}				// fin main
