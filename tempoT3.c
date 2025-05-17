// tarefa 3: os processos testam todos os outros e atualizam o vetor de acordo com o estado

#include <stdio.h>
#include <stdlib.h>
#include "smpl.h"

#define test 1      
#define fault 2     
#define recovery 3  

#define UKNOWN -1
#define CORRETO 0
#define FALHO 1

typedef struct {
    int id; 
} TipoProcesso;

TipoProcesso *processo;

// simulação de eventos
int setup(int numProcessos, int vetorEstados[]) {
    for (int i = 0; i < numProcessos; i++) {
        schedule(test, 30.0, i);
        vetorEstados[i] = UKNOWN;
    }
    
    schedule(fault, 31.0, 1);
    schedule(fault, 31.0, 2);
    schedule(fault, 31.0, 3);

    schedule(recovery, 61.0, 1);
    schedule(recovery, 61.0, 3);

    return 0;
}

const char* nomeEstado(int estado) {
    switch (estado) {
        case UKNOWN:
            return "UKNOWN";
        case CORRETO:
            return "CORRETO";
        case FALHO:
            return "FALHO";
        default:
            return "ESTADO_INVÁLIDO";
    }
}

int main(int argc, char *argv[]) {
    static int numProcessos;
    static int token;
    static int event, r, i;
    static int MaxTempoSimulac = 150;

    static char fa_name[5];

    if (argc != 2) {
        puts("Uso correto: tempo <número de processos>");
        exit(1);
    }

    numProcessos = atoi(argv[1]);

    smpl(0, "Um Exemplo de Simulação");
    reset();      
    stream(1);    

    processo = (TipoProcesso *) malloc(sizeof(TipoProcesso) * numProcessos);

    for (i = 0; i < numProcessos; i++) {
        memset(fa_name, '\0', 5);       
        sprintf(fa_name, "%d", i);      
        processo[i].id = facility(fa_name, 1);
    }

    int vetorEstados[numProcessos];
    setup(numProcessos, vetorEstados);

    while (time() < MaxTempoSimulac) {
        cause(&event, &token);

        switch (event) {
            case test:
                // se o próprio processo tá falho, não testa
                if (status(processo[token].id) != 0) break; 
                
                vetorEstados[token] = CORRETO;
                printf("\nRodada de testes do processo: %d\n", token);

                // testa todos os próximos processos até achar um funcionando
                for (int i = 1; i < numProcessos; i++) {
                    int testTarget = (token + i) % numProcessos;

                    if (status(processo[testTarget].id) == 0) {
                        printf("O processo %d testou o processo %d CORRETO no tempo %4.1f\n", token, testTarget, time());
                        vetorEstados[testTarget] = CORRETO;
                    } else {
                        printf("O processo %d testou o processo %d FALHO no tempo %4.1f\n", token, testTarget, time());
                        vetorEstados[testTarget] = FALHO;
                    }
                }
            
                printf("\nTodos os processos foram testados! Imprimindo resultados:\n");
                for (int j = 0; j < numProcessos; j++) {
                    printf("Processo %d: %s\n", j, nomeEstado(vetorEstados[j]));
                }

                schedule(test, 30.0, token);
                break;

            case fault:            
                r = request(processo[token].id, token, 0);
                printf("\nSou o processo %d e falhei no tempo %4.1f\n", token, time());
                break;

            case recovery:
                release(processo[token].id, token);
                printf("\nSou o processo %d e acabo de recuperar no tempo %4.1f\n", token, time());
            
                schedule(test, 1.0, token);
                break;
            default:
                printf("Erro!");
                break;
        }
    } 

    return 0;
}
