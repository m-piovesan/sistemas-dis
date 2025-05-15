#include <stdio.h>
#include <stdlib.h>
#include "smpl.h"

// Definição dos tipos de eventos
#define test 1      
#define fault 2     
#define recovery 3  

typedef struct {
    int id; 
} TipoProcesso;

TipoProcesso *processo;

int main(int argc, char *argv[]) {
    static int numProcessos; // Número de processos
    static int token; // Identifica qual processo está executando o evento atual
    static int event, r, i; // Variáveis auxiliares
    static int MaxTempoSimulac = 150; // Tempo máximo da simulação

    static char fa_name[5]; // Nome usado para identificar as facilities (como string)

    // Validação de entrada
    if (argc != 2) {
        puts("Uso correto: tempo <número de processos>");
        exit(1);
    }

    // Converte argumento da linha de comando para inteiro
    numProcessos = atoi(argv[1]);

    // Inicialização da SMPL
    smpl(0, "Um Exemplo de Simulação");
    reset();       // Reinicia o tempo da simulação
    stream(1);     // Define a semente para geração de números aleatórios (não usado aqui, mas boa prática)

    // Alocação e criação dos processos
    processo = (TipoProcesso *) malloc(sizeof(TipoProcesso) * numProcessos);

    for (i = 0; i < numProcessos; i++) {
        memset(fa_name, '\0', 5);        // Limpa o nome da facility
        sprintf(fa_name, "%d", i);       // Define o nome da facility como o índice (ex: "0", "1", ...)
        processo[i].id = facility(fa_name, 1); // Cria uma facility com uma instância (1)
    }

    // Escalonamento inicial de eventos:
    // Cada processo fará seu primeiro teste no tempo 30.0
    for (i = 0; i < numProcessos; i++) {
        schedule(test, 30.0, i); // Evento 'test' para cada processo
    }

    // Simulamos uma falha no processo 1 no tempo 31.0
    schedule(fault, 31.0, 1);

    // E sua recuperação no tempo 61.0
    schedule(recovery, 61.0, 1);

    puts("===============================================================");
    puts("           Sistemas Distribuídos Prof. Elias");
    puts("          LOG do Trabalho Prático 0, Tarefa 0");
    puts("      Digitar, compilar e executar o programa tempo.c");
    printf("   Este programa foi executado para: N=%d processos.\n", numProcessos);
    printf("           Tempo Total de Simulação = %d\n", MaxTempoSimulac);
    puts("===============================================================\n");

    while (time() < MaxTempoSimulac) {
        cause(&event, &token); // Obtém o próximo evento agendado

        switch (event) {
            case test:
                // Verifica se o processo está ativo (0 = ativo, != 0 = falho)
                if (status(processo[token].id) != 0) break;

                printf("Processo %d: estou testando no tempo %4.1f\n", token, time());

                // Agenda o próximo teste desse processo daqui a 30 unidades de tempo
                schedule(test, 30.0, token);
                break;

            case fault:
                // Marca a facility do processo como ocupada → representa que o processo está falho
                r = request(processo[token].id, token, 0);
                printf("Socooorro!!! Sou o processo %d e estou falhando no tempo %4.1f\n", token, time());
                break;

            case recovery:
                // Libera a facility do processo → processo volta a ficar ativo
                release(processo[token].id, token);
                printf("Viva!!! Sou o processo %d e acabo de recuperar no tempo %4.1f\n", token, time());

                // Após recuperação, agenda novo teste rapidamente (em 1 unidade de tempo)
                schedule(test, 1.0, token);
                break;
        }
    } 

    return 0;
}
