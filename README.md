# SMPL

| Função                              | Descrição                                              |
| ----------------------------------- | ------------------------------------------------------ |
| `smpl()`                            | Inicializa a simulação.                                |
| `reset()`                           | Reseta o estado do sistema.                            |
| `start()`                           | Inicia o relógio de simulação.                         |
| `schedule(evento, tempo, entidade)` | Agenda um evento no futuro.                            |
| `cause(&evento, &token)`            | Retira o próximo evento da lista de eventos agendados. |
| `facility(nome, n)`                 | Cria um recurso (como um servidor) com `n` instâncias. |
| `request(facility, token, tempo)`   | Solicita uso de um recurso.                            |
| `release(facility, token)`          | Libera o recurso.                                      |
| `time()`                            | Retorna o tempo atual da simulação.                    |
