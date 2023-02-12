/**
 * @file proj2.h
 * @author Jakub Čoček (xcocek00), FIT
 * @brief Building H2O - header file
 * @date 2022-04-23
 * 
 */

//guard
#ifndef PROJ2_H
#define PROJ2_H

//includes
#include <stdio.h>
#include <stdlib.h>
#include <semaphore.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <stdbool.h>
#include <stdint.h>
#include <sys/mman.h>
#include <unistd.h>
#include <sys/wait.h>
#include <ctype.h>
#include <string.h>

//defines pro pojmenovane semafory (login kvuli testovani na merlinovi)
#define OQUEUE "/xcocek00_Oqueue"
#define HQUEUE "/xcocek00_Hqueue"
#define MUTEX "/xcocek00_mutex"
#define ZAPIS "/xcocek00_zapis"
#define BARRIER_MUTEX "/xcocek00_barrier_mutex"
#define BARRIER_TURNSTILE "/xcocek00_barrier_turnstile"
#define BARRIER_TURNSTILE2 "/xcocek00_barrier_turnstile2"
#define MOLECULE_DONE "/xcocek00_molecule_done"
#define HYDROGEN_CHECK "/xcocek00_hydorgen_check"

/**
 * @brief struktura pro zpracovani parametru
 */
typedef struct{
    int no; //pocet kysliku
    int nh; //pocet vodiku
    int ti; //Maximální čas [ms], po který atom kyslíku/vodíku po svém vytvoření čeká, než se zařadí do fronty na vytváření molekul
    int tb; //Maximální čas v [ms] nutný pro vytvoření jedné molekuly
}params_t;

/**
 * @brief struktura pro semafory
 */
typedef struct{
    sem_t *Oqueue;
    sem_t *Hqueue;
    sem_t *mutex;
    sem_t *zapis;
    sem_t *barrier_mutex;
    sem_t *barrier_turnstile;
    sem_t *barrier_turnstile2;
    sem_t *molecule_done;
    sem_t *hydrogen_check;
}semaphore_t;

/**
 * @brief funkce na zpracovani parametru
 * 
 * @param argc pocet parametru
 * @param argv pole paramtru
 * @param params struktura pro ulozeni pamarametru
 */
void parms_f(int argc, char *argv[], params_t *params);

/**
 * @brief inicializuje semafory
 * 
 * @param sem struktura se semafory
 */
void semaphore_init(semaphore_t *sem);

/**
 * @brief funkce pro unlink semaforu
 */
void semaphore_unlink();

/**
 * @brief funkce pro uzavreni semaforu
 * 
 * @param sem strukrura se semafoty
 */
void semaphore_close(semaphore_t *sem);

/**
 * @brief funkce na celkovy uklid
 * 
 * @param sem struktura semaforu
 */
void cleanup(semaphore_t *sem);

/**
 * @brief funkce na uvolneni sdilenych promennych
 * 
 * @param sem promenna pro semafory
 * @param oxygen_counter promenna pro pocitani kysliku ve fronte
 * @param hydrogen_counter promenna pro pocitani vodiku ve fronte
 * @param counter pocitani radku
 * @param noM pocitani molekul
 * @param idH pocitani id vodiku
 * @param idO pocitani id kysliku
 * @param barrier_counter pocitani processu v bariere
 * @param O_green_flag pusteni zbyvajicich kysliku
 * @param H_green_flag pusteni zbyvajicich vodiku
 * @param oxygen_pass_counter pocita kolik kysliku proslo mutexem
 * @param hydrogen_pass_counter pocita kolik vodiku proslo mutexem
 */
void shr_var_dtor(semaphore_t *sem, uint32_t *oxygen_counter, uint32_t *hydrogen_counter, uint32_t *counter, uint32_t *noM, uint32_t *idH, uint32_t *idO, int32_t *barrier_counter, bool *O_green_flag, bool *H_green_flag, uint32_t *oxygen_pass_counter, uint32_t *hydrogen_pass_counter);

/**
 * @brief funkce simulujici proces kyslik
 * 
 * @param sem promenna pro semafory
 * @param params struktura parametru 
 * @param idO pocitani id kysliku
 * @param fp soubor
 * @param counter pocitani radku
 * @param oxygen_counter pocita kolik kysliku proslo mutexem
 * @param hydrogen_counter pocita kolik vodiku proslo mutexem
 * @param noM pocitani molekul
 * @param barrier_counter pocitani processu v bariere
 * @param O_green_flag pusteni zbyvajicich kysliku
 * @param H_green_flag pusteni zbyvajicich vodiku
 * @param oxygen_pass_counter pocita kolik kysliku proslo mutexem
 * @param hydrogen_pass_counter pocita kolik vodiku proslo mutexem
 */
void process_oxygen(semaphore_t *sem, params_t *params, uint32_t *idO, FILE *fp, uint32_t *counter, uint32_t *oxygen_counter, uint32_t *hydrogen_counter, uint32_t *noM, int32_t *barrier_counter, bool *O_green_flag, bool *H_green_flag, uint32_t *oxygen_pass_counter, uint32_t *hydrogen_pass_counter);

/**
 * @brief funkce simulujici proces vodik
 * 
 * @param sem promenna pro semafory
 * @param params struktura parametru 
 * @param idH pocitani id vodiku
 * @param fp soubor
 * @param counter pocitani radku
 * @param oxygen_counter pocita kolik kysliku proslo mutexem
 * @param hydrogen_counter pocita kolik vodiku proslo mutexem
 * @param noM pocitani molekul
 * @param barrier_counter pocitani processu v bariere
 * @param O_green_flag pusteni zbyvajicich kysliku
 * @param H_green_flag pusteni zbyvajicich vodiku
 * @param oxygen_pass_counter pocita kolik kysliku proslo mutexem
 * @param hydrogen_pass_counter pocita kolik vodiku proslo mutexem 
 */
void process_hydrogen(semaphore_t *sem, params_t *params, uint32_t *idH, FILE *fp, uint32_t *counter, uint32_t *hydrogen_counter, uint32_t *oxygen_counter, uint32_t *noM, int32_t *barrier_counter, bool *H_green_flag, bool *O_green_flag, uint32_t *oxygen_pass_counter, uint32_t *hydrogen_pass_counter);

/**
 * @brief čeká náhodný čas nez se zaradi do fronty
 * 
 * @param params struktura s parametry
 */
void sleep_before_queue(params_t *params);

/**
 * @brief simuluje vytvareni molekuly
 * 
 * @param params struktura s parametry
 */
void sleep_molecule(params_t *params);

#endif //pro PROJ2_H