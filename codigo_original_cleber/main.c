#include <zephyr/kernel.h>
#include <zephyr/sys/atomic.h>

/* ------------------------------------------------------------------ *
 * Sincronizacao baseada em semaforos para forcamento de Race Condition
 * 
 * Este codigo utiliza semaforos do Zephyr RTOS para coordenar estritamente
 * a ordem de execucao entre duas threads trabalhadoras e a thread principal,
 * criando uma janela controlada onde ocorre a perda de atualizacao (perda
 * de incremento) de forma deterministica, mesmo utilizando operacoes
 * atomicas parciais.
 * ------------------------------------------------------------------ */

#define ITERATIONS 1000
#define STACK_SIZE 1024
#define THREAD_PRIORITY 5

/* Variavel atomica global que armazena o contador de pacotes de telemetria */
static atomic_t telemetry_packets = ATOMIC_INIT(0);

/* Semáforos para controle de sincronização e alternancia de fases entre threads */
K_SEM_DEFINE(read_ready, 0, 2);
K_SEM_DEFINE(write_go, 0, 2);
K_SEM_DEFINE(write_done, 0, 2);
K_SEM_DEFINE(round_start, 0, 2);

/* Definicao estatica das pilhas e estruturas de controle das threads */
K_THREAD_STACK_DEFINE(worker_a_stack, STACK_SIZE);
K_THREAD_STACK_DEFINE(worker_b_stack, STACK_SIZE);
static struct k_thread worker_a_data;
static struct k_thread worker_b_data;

/* Funcao executada pelas threads trabalhadoras (Worker A e Worker B) */
static void telemetry_worker(void *arg1, void *arg2, void *arg3)
{
    ARG_UNUSED(arg1);
    ARG_UNUSED(arg2);
    ARG_UNUSED(arg3);

    for (int i = 0; i < ITERATIONS; ++i) {
        /* Aguarda o sinal da main para iniciar mais uma rodada de incremento */
        k_sem_take(&round_start, K_FOREVER);
        
        /* Le o valor atual do contador compartilhado */
        atomic_val_t packets = atomic_get(&telemetry_packets);

        /* Ambos os threads capturam o mesmo valor antes que qualquer um possa escrever. */
        k_sem_give(&read_ready);
        k_sem_take(&write_go, K_FOREVER);

        /*
         * O acesso atomico individual e seguro, mas esta gravacao nao e
         * atomica com a leitura precedente: a outra atualizacao e perdida.
         */
        atomic_set(&telemetry_packets, packets + 1);
        k_sem_give(&write_done);
    }
}

int main(void)
{
    /* Primeira Thread (worker_a) */
    k_thread_create(&worker_a_data, worker_a_stack, STACK_SIZE,
            telemetry_worker, NULL, NULL, NULL,
            THREAD_PRIORITY, 0, K_NO_WAIT);

    /* Segunda Thread (worker_b) */
    k_thread_create(&worker_b_data, worker_b_stack, STACK_SIZE,
            telemetry_worker, NULL, NULL, NULL,
            THREAD_PRIORITY, 0, K_NO_WAIT);

    /* Loop principal que controla e sincroniza as iteracoes das threads */
    for (int i = 0; i < ITERATIONS; ++i) {
        k_sem_give(&round_start);
        k_sem_give(&round_start);
        k_sem_take(&read_ready, K_FOREVER);
        k_sem_take(&read_ready, K_FOREVER);
        k_sem_give(&write_go);
        k_sem_give(&write_go);
        k_sem_take(&write_done, K_FOREVER);
        k_sem_take(&write_done, K_FOREVER);
    }

    /* Exibe os resultados finais da execucao e as metricas de perdas */
    printk("Pacotes registrados: %d\n",
            (int)atomic_get(&telemetry_packets));
    printk("Incrementos solicitados: %d\n", 2 * ITERATIONS);
    printk("Atualizacoes perdidas: %d\n",
            2 * ITERATIONS - (int)atomic_get(&telemetry_packets));

    return 0;
}