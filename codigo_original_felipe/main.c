#include <zephyr/kernel.h>
#include <zephyr/sys/printk.h>

/* ------------------------------------------------------------------ *
 * Demonstracao de Race Condition em Zephyr RTOS
 *
 * Duas threads de MESMA prioridade incrementam, cada uma, um contador
 * global NUM_INCREMENTS vezes. Se o acesso ao contador fosse atomico
 * e protegido, o valor final teria que ser exatamente
 * 2 * NUM_INCREMENTS. Sem sincronizacao, threads podem ser preemptadas
 * (pelo scheduler, por tick de sistema ou por uma ISR) no MEIO do
 * padrao ler-modificar-escrever, perdendo incrementos.
 *
 * ------------------------------------------------------------------ */

#define NUM_THREADS     2
#define NUM_INCREMENTS  2000
#define STACK_SIZE      1024
#define THREAD_PRIORITY 5

/*
 * Alarga artificialmente a janela entre "ler" e "escrever" o recurso
 * compartilhado. O bug (acesso nao atomico) ja existe sem isso - aqui
 * so aumentamos a probabilidade de uma troca de contexto acontecer
 * durante o acesso desprotegido, tornando a falha visivel de forma
 * confiavel em poucas execucoes, em vez de depender da sorte de um
 * tick de interrupcao cair exatamente entre duas instrucoes.
 */
#define CRITICAL_SECTION_DELAY_US 50

/* "volatile" aqui so evita que o compilador otimize o acesso embora;
 * ela NAO torna o acesso atomico nem substitui sincronizacao. */
static volatile uint32_t shared_counter;

static void increment_shared_counter(void)
{

	/* 1. LER o valor atual do recurso compartilhado */
	uint32_t temp = shared_counter;

	/* 2. "Processar" o valor - simula algum trabalho sobre o dado lido.
	 *    E exatamente aqui, entre o passo 1 e o passo 3, que outra
	 *    thread pode ser escalonada, ler o MESMO valor antigo e
	 *    tambem calcular temp + 1 a partir dele. */
	k_busy_wait(CRITICAL_SECTION_DELAY_US);
	temp = temp + 1;

	/* 3. ESCREVER o valor de volta no recurso compartilhado.
	 *    Se outra thread fez o mesmo entre os passos 1 e 3, uma das
	 *    duas escritas sera perdida: o incremento "desaparece". */
	shared_counter = temp;

}

static void counter_thread(void *p1, void *p2, void *p3)
{
	const char *name = (const char *)p1;

	ARG_UNUSED(p2);
	ARG_UNUSED(p3);

	for (int i = 0; i < NUM_INCREMENTS; i++) {
		increment_shared_counter();
	}

	printk("Thread %s terminou suas %d incrementacoes\n", name, NUM_INCREMENTS);
}

K_THREAD_DEFINE(thread_a, STACK_SIZE, counter_thread, "A", NULL, NULL,
		THREAD_PRIORITY, 0, 0);

K_THREAD_DEFINE(thread_b, STACK_SIZE, counter_thread, "B", NULL, NULL,
		THREAD_PRIORITY, 0, 0);

void main(void)
{
	uint32_t expected = NUM_THREADS * NUM_INCREMENTS;

	printk("\n=== Demonstracao de Race Condition (Zephyr RTOS) ===\n");
	printk("Threads:........... %d\n", NUM_THREADS);
	printk("Incrementos/thread: %d\n", NUM_INCREMENTS);
	printk("Valor esperado:.... %u\n\n", expected);

	/* Espera as duas threads terminarem antes de ler o resultado final */
	k_thread_join(thread_a, K_FOREVER);
	k_thread_join(thread_b, K_FOREVER);

	printk("\nValor final do contador: %u\n", shared_counter);
	printk("Valor esperado:          %u\n", expected);

	if (shared_counter == expected) {
		printk("RESULTADO: OK - nenhuma atualizacao perdida\n");
	} else {
		printk("RESULTADO: FALHA - %u atualizacao(oes) perdida(s) "
		       "por race condition\n",
		       expected - shared_counter);
	}
}
