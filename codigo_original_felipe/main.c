/*
 * Demonstração de deadlock: duas threads, dois mutex, ordem de aquisição invertida.
 *
 *   thread1: pega A -> (espera) -> tenta B
 *   thread2: pega B -> (espera) -> tenta A
 *
 * A main continua imprimindo um "heartbeat" para mostrar que o sistema
 * NÃO travou por inteiro: só as duas threads envolvidas ficam bloqueadas.
 */

#include <zephyr/kernel.h>
#include <zephyr/sys/printk.h>

#define STACK_SIZE  1024
#define PRIORIDADE_A  5
#define PRIORIDADE_B 5

/* Janela entre o primeiro e o segundo lock (facilita o deadlock). */
#define JANELA_MS   10

K_MUTEX_DEFINE(mutex_a);
K_MUTEX_DEFINE(mutex_b);

void thread1(void *p1, void *p2, void *p3)
{
	ARG_UNUSED(p1);
	ARG_UNUSED(p2);
	ARG_UNUSED(p3);

	while (1) {
        printk("[%u] thread1: heartbeat (sistema vivo)\n", k_uptime_get_32());
		printk("[%u] T1: tentando pegar A\n", k_uptime_get_32());
		k_mutex_lock(&mutex_a, K_FOREVER);
		printk("[%u] T1: pegou A\n", k_uptime_get_32());

		k_msleep(JANELA_MS);

		printk("[%u] T1: tentando pegar B\n", k_uptime_get_32());
		k_mutex_lock(&mutex_b, K_FOREVER);
		printk("[%u] T1: pegou B (tem A e B)\n", k_uptime_get_32());

		/* Seção crítica usando os dois recursos */

		k_mutex_unlock(&mutex_b);
		k_mutex_unlock(&mutex_a);
		printk("[%u] T1: soltou A e B\n", k_uptime_get_32());

		k_msleep(100);
	}
}

void thread2(void *p1, void *p2, void *p3)
{
	ARG_UNUSED(p1);
	ARG_UNUSED(p2);
	ARG_UNUSED(p3);

	while (1) {
        printk("[%u] thread2: heartbeat (sistema vivo)\n", k_uptime_get_32());
		printk("[%u] T2: tentando pegar B\n", k_uptime_get_32());
		k_mutex_lock(&mutex_b, K_FOREVER);
		printk("[%u] T2: pegou B\n", k_uptime_get_32());

		k_msleep(JANELA_MS);

		printk("[%u] T2: tentando pegar A\n", k_uptime_get_32());
		k_mutex_lock(&mutex_a, K_FOREVER);
		printk("[%u] T2: pegou A (tem A e B)\n", k_uptime_get_32());

		/* Seção crítica usando os dois recursos */

		k_mutex_unlock(&mutex_a);
		k_mutex_unlock(&mutex_b);
		printk("[%u] T2: soltou A e B\n", k_uptime_get_32());

		k_msleep(100);
	}
}

K_THREAD_DEFINE(t1_id, STACK_SIZE, thread1, NULL, NULL, NULL, PRIORIDADE_A, 0, 0);
K_THREAD_DEFINE(t2_id, STACK_SIZE, thread2, NULL, NULL, NULL, PRIORIDADE_B, 0, 0);

int main(void)
{
	printk("=== Demo de deadlock ===\n");

	while (1) {
		k_msleep(1000);
		printk("[%u] main: heartbeat (sistema vivo)\n", k_uptime_get_32());
	}

	return 0;
}
