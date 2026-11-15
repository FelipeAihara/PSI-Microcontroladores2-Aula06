/*
 * Demonstração de deadlock: duas threads, dois mutex, ordem de aquisição invertida.
 *
 *   thread_a: pega A -> (espera) -> tenta B
 *   thread_b: pega B -> (espera) -> tenta A
 *
 * A main continua imprimindo um "heartbeat" para mostrar que o sistema
 * NÃO travou por inteiro: só as duas threads envolvidas ficam bloqueadas.
 */

#include <zephyr/kernel.h>
#include <zephyr/sys/printk.h>

#define STACK_SIZE  1024
#define PRIORIDADE_A 5
#define PRIORIDADE_B 4

/* Janela entre o primeiro e o segundo lock (facilita o deadlock). */
#define JANELA_MS   10

K_MUTEX_DEFINE(mutex_1);
K_MUTEX_DEFINE(mutex_2);

void thread_a(void *p1, void *p2, void *p3)
{
	ARG_UNUSED(p1);
	ARG_UNUSED(p2);
	ARG_UNUSED(p3);

	while (1) {
        printk("[%u] thread_a: heartbeat (sistema vivo)\n", k_uptime_get_32());
		printk("[%u] T_A: tentando pegar A\n", k_uptime_get_32());
		k_mutex_lock(&mutex_1, K_FOREVER);
		printk("[%u] T_A: pegou A\n", k_uptime_get_32());

		k_msleep(JANELA_MS);

		printk("[%u] T_A: tentando pegar B\n", k_uptime_get_32());
		k_mutex_lock(&mutex_2, K_FOREVER);
		printk("[%u] T_A: pegou B (tem A e B)\n", k_uptime_get_32());

		/* Seção crítica usando os dois recursos */

		k_mutex_unlock(&mutex_2);
		k_mutex_unlock(&mutex_1);
		printk("[%u] T_A: soltou A e B\n", k_uptime_get_32());

		k_msleep(100);
	}
}

void thread_b(void *p1, void *p2, void *p3)
{
	ARG_UNUSED(p1);
	ARG_UNUSED(p2);
	ARG_UNUSED(p3);

	while (1) {
        printk("[%u] thread_b: heartbeat (sistema vivo)\n", k_uptime_get_32());
		printk("[%u] T_B: tentando pegar 1\n", k_uptime_get_32());
		k_mutex_lock(&mutex_1, K_FOREVER);
		printk("[%u] T_B: pegou B\n", k_uptime_get_32());

		k_msleep(JANELA_MS);

		printk("[%u] T_B: tentando pegar 2\n", k_uptime_get_32());
		k_mutex_lock(&mutex_2, K_FOREVER);
		printk("[%u] T_B: pegou A (tem A e B)\n", k_uptime_get_32());

		/* Seção crítica usando os dois recursos */

		k_mutex_unlock(&mutex_2);
		k_mutex_unlock(&mutex_1);
		printk("[%u] T_B: soltou A e B\n", k_uptime_get_32());

		k_msleep(100);
	}
}

K_THREAD_DEFINE(t_a_id, STACK_SIZE, thread_a, NULL, NULL, NULL, PRIORIDADE_A, 0, 0);
K_THREAD_DEFINE(t_b_id, STACK_SIZE, thread_b, NULL, NULL, NULL, PRIORIDADE_B, 0, 0);

int main(void)
{
	printk("=== Demo de deadlock ===\n");

	while (1) {
		k_msleep(1000);
		printk("[%u] main: heartbeat (sistema vivo)\n", k_uptime_get_32());
	}

	return 0;
}
