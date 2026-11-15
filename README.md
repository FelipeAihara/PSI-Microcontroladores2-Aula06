# PSI-Microcontroladores2-Aula06

Felipe Aihara - NUSP: 17889351, Matheus Mendes - NUSP: 17841153 e Cleber Fernandes - NUSP: 17884589

- OBS.: cada pasta desse repositório contém tanto o código quanto os testes realizados!

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

- Antes, estava ocorrendo um caso clássico de deadlock (a thread A ficava esperando a thread B soltar o mutex B, enquanto a thread B esperava a thread A soltar o mutex A). Por isso, as threads A e B paravam de enviar mensagens na serial (morriam). Para corrigir esse erro, adotei a solução de Dijkstra ao problema do jantar dos filósofos, em que se opta por adquirir o garfo (nesse caso, o mutex) de menor número primeiro. 

### Avaliação do codigo_matheus (race condition - serial; revisão feita por Felipe)

- O código original estava misturando as mensagens da thread A e da B (evidência está em codigo_matheus_original/teste1.txt. Algo interessante, entretanto, é que isso só ocorria no caso em que PRIORIDADE_A = PRIORIDADE_B. Isso não ocorria nos casos em que a prioridade da thread A era diferente da thread B, mesmo sem mutex. Para corrigir esse erro, eu só coloquei um mutex na seção crítica do código, em que se usava a UART.

### Avaliação do codigo_cleber (race condition - contador; revisão feita por Matheus)

- Assim como no caso "race condition - serial", o erro só aparecia quando as prioridades das threads eram iguais. Nesse caso, o erro era de que o contador assumia um valor diferente do esperado (2361 x 4000, resultado está em codigo_original_cleber/teste1.txt). Para resolver esse problema, implementei um mutex antes de chamar a função increment_shared_counter().
