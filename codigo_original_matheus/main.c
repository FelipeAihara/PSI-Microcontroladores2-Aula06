/*
 * Exemplo de RACE CONDITION no Zephyr RTOS — recurso compartilhado: SERIAL
 *
 * Duas threads de mesma prioridade escrevem mensagens na UART do console,
 * caractere por caractere (uart_poll_out). Uma mensagem só faz sentido se
 * for transmitida INTEIRA, sem interrupção — ou seja, a mensagem completa
 * é a seção crítica, não cada caractere.
 *
 * Um k_yield() após cada caractere força a troca de contexto, tornando a
 * race condition determinística: as mensagens saem embaralhadas, ex.:
 *
 *     TThhrreeaadd  AB::  lleeiittuurraa ...
 *
 * Resultado esperado (correto): cada linha sai inteira, de uma thread só.
 */

#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/drivers/uart.h>
#include <zephyr/sys/printk.h>

#define STACK_SIZE   1024
#define PRIORIDADE_A   5         
#define PRIORIDADE_B   5         
#define MENSAGENS    5
#define TAM_BUFFER   64

/* Recurso compartilhado: a UART do console (definida no devicetree) */
static const struct device *const uart = DEVICE_DT_GET(DT_CHOSEN(zephyr_console));

K_THREAD_STACK_DEFINE(stack_a, STACK_SIZE);
K_THREAD_STACK_DEFINE(stack_b, STACK_SIZE);

static struct k_thread thread_a;
static struct k_thread thread_b;

/* Envia uma string pela serial, um caractere de cada vez */
static void serial_escreve(const char *msg)
{
	while (*msg != '\0') {
		uart_poll_out(uart, *msg++);
		k_yield();     /* troca de contexto forçada no meio da mensagem */
	}
}

static void tarefa(void *p1, void *p2, void *p3)
{
	const char *nome = (const char *)p1;
	char buffer[TAM_BUFFER];

	ARG_UNUSED(p2);
	ARG_UNUSED(p3);

	for (int i = 0; i < MENSAGENS; i++) {
		snprintk(buffer, sizeof(buffer),
			 "%s: leitura do sensor numero %d\r\n", nome, i);

		/* ===== INÍCIO DA SEÇÃO CRÍTICA ===== */
		serial_escreve(buffer);
		/* ===== FIM DA SEÇÃO CRÍTICA ===== */

		k_msleep(100);  /* simula uma tarefa periódica */
	}
}

int main(void)
{
	if (!device_is_ready(uart)) {
		printk("UART do console nao esta pronta\n");
		return 0;
	}

	printk("\n=== Race condition na serial ===\n");
	printk("Cada linha deveria sair inteira, de uma unica thread:\n\n");

	k_tid_t tid_a = k_thread_create(&thread_a, stack_a,
					K_THREAD_STACK_SIZEOF(stack_a),
					tarefa, "Thread A", NULL, NULL,
					PRIORIDADE_A, 0, K_NO_WAIT);

	k_tid_t tid_b = k_thread_create(&thread_b, stack_b,
					K_THREAD_STACK_SIZEOF(stack_b),
					tarefa, "Thread B", NULL, NULL,
					PRIORIDADE_B, 0, K_NO_WAIT);

	k_thread_join(tid_a, K_FOREVER);
	k_thread_join(tid_b, K_FOREVER);

	printk("\n=== Fim ===\n");
	return 0;
}
