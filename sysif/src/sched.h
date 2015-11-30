#ifndef SCHED_H
#define SCHED_H

#include <stdint.h>
#include "util.h"

// Macros ----------------------------------------------------------------------
#define NB_SAVED_REGISTERS 13
#define SIZE_STACK_PROCESS 10000 // En Octet = 10Ko

// Types et structures ---------------------------------------------------------
typedef int (func_t) (void);

struct pcb_s {
    uint32_t registres[NB_SAVED_REGISTERS];
    func_t * lr_user;
    func_t * lr_svc;
    uint32_t * sp_start;
    uint32_t * sp;
    uint32_t cpsr;
    struct pcb_s * pcb_next;
    int state;
    int priority;
    int exit_code;
    func_t * entry;
};

// Variables globales ----------------------------------------------------------
struct pcb_s * current_process;

// Gestion des processus -------------------------------------------------------
/**
 *  Initialise les schedulers
 */
void sched_init(SCHEDULING_POLICY schedPolicy);
/**
 *  Crée un nouveau processus en allouant de la mémoire pour ce dernier
 */
struct pcb_s * create_process(func_t* entry, PROC_PRIORITY priority);
/**
 *  Réalise l'election d'un nouveau processus
 */
void elect(void);
/**
 *  Démarre le processus courant
 */
void start_current_process(void);
// --------------------------------------------------
// ---------------- SIMPLE QUEUE --------------------
// --------------------------------------------------
/**
 *  Initialise le scheduler de type file simple
 */
void queue_sched_init(void);
/**
 *  Ajoute un processus à la file simple
 */
void queue_sched_add(struct pcb_s * newProcess);
/**
 *  Réalise l'élection d'un nouveau processus depuis la file simple
 */
struct pcb_s * queue_sched_elect(void);
/**
 *  Réalise l'élection d'un nouveau processus depuis les files de priorités
 */
void queue_sched_clean(void);
/**
 *  Teste si la fin des processus est atteinte
 */
void queue_sched_termination_test(void);
// --------------------------------------------------
// ---------------- PRIORITY QUEUES -----------------
// --------------------------------------------------
/**
 *  Initialise le scheduler de type files de priorité
 */
void priority_queue_sched_init(void);
/**
 *  Ajoute un processus aux files de priorité
 */
void priority_queue_sched_add(struct pcb_s * newProcess);
/**
 *  Nettoye les processus terminés des files de priorité
 */
 struct pcb_s * priority_queue_sched_elect(void);
/**
 *  Nettoye les processus terminés de la file simple
 */
void priority_queue_sched_clean(void);
/**
 *  Teste si la fin des processus est atteinte
 */
void priority_queue_sched_termination_test(void);

// Appel système : yieldto -----------------------------------------------------
/**
 *  Appel système pour passer a un autre process si on connait l'adresse de son PCB
 */
void sys_yieldto(struct pcb_s* dest);
/**
 * Appel noyau pour passer a un autre process si on connait l'adresse de son PCB 
 */
void do_sys_yieldto(struct pcb_s * context);

// Appel système : yield -------------------------------------------------------
/**
 *  Appel système pour passer a un autre process (rendre la main)
 */
void sys_yield(void);
/**
 *  Appel noyau pour passer a un autre process (rendre la main)
 */
void do_sys_yield(struct pcb_s * context);

// Appel système : exit --------------------------------------------------------
/**
 *  Appel système pour terminer un processus
 */
void sys_exit(int status);
/**
 *  Appel noyau pour terminer un processus
 */
void do_sys_exit(struct pcb_s * context);

// Handler des timer système ---------------------------------------------------
/**
 *
 */
void irq_handler(void);

#endif //SCHED_H
