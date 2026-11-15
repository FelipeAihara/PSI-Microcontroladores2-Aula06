# PSI-Microcontroladores2-Aula06

Felipe Aihara - NUSP: 17889351, Matheus Mendes - NUSP: 17841153 e Cleber Fernandes - NUSP: 17884589

### Cenários escolhidos e breve descrição

- O primeiro cenário escolhido foi o de um contador com race condition (codigo_cleber). Existem duas threads: ambas tentam incrementar o valor
de uma variável global. Entretanto, uma pode preemptar a outra, o que faz com que o valor final do contador possa diferir do valor esperado.
- O segundo cenário escolhido foi um em que a atomicidade foi violadada (codigo_matheus). Duas threads tentam usar o mesmo recurso compartilhado (a serial) simultaneamente, o que faz com que uma mensagem saia misturada com a outra.
- O terceiro cenário escolhido foi um de deadlock (codigo_felipe). A thread A tenta adquirir o mutex A e depois adquirir o mutex B. A thread B faz o contrário: primeiro tenta adquirir o mutex B e depois adquirir o mutex A.

### Planejamento de testes (race condition - contador)

| Caso de Teste | Pré-condição | Etapas de Teste | Pós-condição Esperada |
|----------------|---------------|------------------|------------------------|
| 1 | PRIORIDADE_A = PRIORIDADE_B | Rodar o código e ler a porta serial | Contador deveria assumir o valor de 2000 |
| 2 | PRIORIDADE_A > PRIORIDADE_B | Rodar o código e ler a porta serial | Contador deveria assumir o valor de 2000 |
| 3 | PRIORIDADE_A < PRIORIDADE_B | Rodar o código e ler a porta serial | Contador deveria assumir o valor de 2000 |

### Planejamento de testes (race condition - serial)

| Caso de Teste | Pré-condição | Etapas de Teste | Pós-condição Esperada |
|----------------|---------------|------------------|------------------------|
| 1 | PRIORIDADE_A = PRIORIDADE_B | Rodar o código e ler a porta serial | Cada linha deveria sair inteira, de uma thread só |
| 2 | PRIORIDADE_A > PRIORIDADE_B | Rodar o código e ler a porta serial | Cada linha deveria sair inteira, de uma thread só |
| 3 | PRIORIDADE_A < PRIORIDADE_B | Rodar o código e ler a porta serial | Cada linha deveria sair inteira, de uma thread só |


### Planejtamento de testes (deadlock)

| Caso de Teste | Pré-condição | Etapas de Teste | Pós-condição Esperada |
|----------------|---------------|------------------|------------------------|
| 1 | PRIORIDADE_A = PRIORIDADE_B | Rodar o código e ler a porta serial | Todas threads (main, thread_a e thread_b) deveriam continuar vivas, printando mensagens no serial.  |
| 2 | PRIORIDADE_A > PRIORIDADE_B | Rodar o código e ler a porta serial | Todas threads (main, thread_a e thread_b) deveriam continuar vivas, printando mensagens no serial.  |
| 3 | PRIORIDADE_A < PRIORIDADE_B | Rodar o código e ler a porta serial | Todas threads (main, thread_a e thread_b) deveriam continuar vivas, printando mensagens no serial.  |


### Avaliação do codigo_felipe (deadlock; revisão feita por Cleber)

- O que estava errado antes?
- Como você corrigiu o erro?
- OBS.: colocar as evidências nas pastas de cada código (código_original/ e codigo_corrigido)

### Avaliação do codigo_cleber (race condition - serial; revisão feita por Matheus)

- O que estava errado antes?
- Como você corrigiu o erro?
- O que mudou com a correção?
- OBS.: colocar as evidências nas pastas de cada código (código_original/ e codigo_corrigido)

### Avaliação do codigo_matheus (race condition - contador; revisão feita por Felipe)

- O que estava errado antes?
- Como você corrigiu o erro?
- O que mudou com a correção?
- OBS.: colocar as evidências nas pastas de cada código (código_original/ e codigo_corrigido)
