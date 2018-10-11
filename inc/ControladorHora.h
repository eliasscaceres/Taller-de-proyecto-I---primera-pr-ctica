#ifndef CONTROLADORHORA_H_ 
#define CONTROLADORHORA_H_
typedef unsigned int uint8;
void ActualizarHora(void);
void imprimirHora(void);
//void imprimirHoraParpadeando(state estadoact);
void cambiarValoresAlarma(uint8 aux,uint8 HS_MIN_SEG, uint8 posicion);
void cambiarTodosValoresReloj(void);
void cambiarValoresReloj(uint8 aux,uint8 HS_MIN_SEG, uint8 posicion);
void guardarTiempo(void);
void guardarTiempo2(void);
void recuperarTiempo(void);
void recuperarTiempo2(void);
void apagarAlarma(void);
#endif 
