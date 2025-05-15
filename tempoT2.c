// tarefa 2: Cada processo correto executa testes até achar outro processo correto
// nesse layout: executar com 4 no terminal para verificar os 3 processos falhos juntos

#include <stdio.h>
#include <stdlib.h>
#include "smpl.h"

#define test 1      
#define fault 2     
#define recovery 3  

typedef struct {
    int id; 
} TipoProcesso;

TipoProcesso *processo;

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

    for (i = 0; i < numProcessos; i++) {
        schedule(test, 30.0, i);
    }

    schedule(fault, 31.0, 1);
    schedule(fault, 31.0, 2);
    schedule(fault, 31.0, 3);

    schedule(recovery, 61.0, 1);
    schedule(recovery, 61.0, 3);

    while (time() < MaxTempoSimulac) {
        cause(&event, &token);

        switch (event) {
            case test:
                // se o próprio processo tá falho, não testa
                if (status(processo[token].id) != 0) break; 

                // testa todos os próximos processos até achar um funcionando
                for (int i = 1; i < numProcessos; i++) {
                    int testTarget = (token + i) % numProcessos;
                    
                    if (status(processo[testTarget].id) == 0) {
                        printf("O processo %d testou o processo %d CORRETO no tempo %4.1f\n", token, testTarget, time());
                        break;
                    } else {
                        printf("O processo %d testou o processo %d FALHO no tempo %4.1f\n", token, testTarget, time());
                    }

                    if (i == numProcessos - 1) {
                        printf("Essa não! Todos os outros processos estavam falhos, vou esperar!\n");
                    }
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
