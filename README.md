# PSI-Microcontroladores2-Aula06

Felipe Aihara - NUSP: 17889351 e Cleber Fernandes - NUSP: 17884589

### Cenários escolhidos e breve descrição

- O primeiro cenário escolhido foi um de race condition.  
- O segundo cenário escolhido foi um de deadlock. A thread A tenta adquirir o mutex A. Se consegue, simula uma realização de trabalho com ele. Depois,
tenta adquirir o mutex B. Se consegue, simula uma realização de trabalho com ele. Idem para a thread B, mas na ordem inversa (pega mutex B primeiro).

### Planejamento de testes (race condition)

| Caso de Teste | Pré-condição | Etapas de Teste | Pós-condição Esperada |
|----------------|---------------|------------------|------------------------|
| 1 | ... | ... | ... |
| 2 | ... | ... | ... |
| 3 | ... | ... | ... |

### Planejtamento de testes (deadlock)

| Caso de Teste | Pré-condição | Etapas de Teste | Pós-condição Esperada |
|----------------|---------------|------------------|------------------------|
| 1 | PRIORIDADE_A = PRIORIDADE_B | Rodar o código e ler a porta serial | Ler a porta serial por 10 segundos. Ver se todas threads (main, thread1 e thread2) estão vivas.  |
| 2 | PRIORIDADE_A > PRIORIDADE_B | Rodar o código e ler a porta serial | Ler a porta serial por 10 segundos. Ver se todas threads (main, thread1 e thread2) estão vivas.  |
| 3 | PRIORIDADE_B < PRIORIDADE_B | Rodar o código e ler a porta serial | Ler a porta serial por 10 segundos. Ver se todas threads (main, thread1 e thread2) estão vivas.  |


### Avaliação do código do Felipe (revisão feita por Cleber, incluindo evidências que os testes foram realizados)

- O que estava errado antes?
- Como você corrigiu o erro?
- OBS.: colocar as evidências nas pastas de cada código (código_original/ e codigo_corrigido)

### Avaliação do código do Cleber (revisão feita por Felipe, incluindo evidências que os testes foram realizados)

- O que estava errado antes?
- Como você corrigiu o erro?
- O que mudou com a correção?
- OBS.: colocar as evidências nas pastas de cada código (código_original/ e codigo_corrigido)
