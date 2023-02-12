/**
 * @file proj2.c
 * @author Jakub Čoček (xcocek00), FIT
 * @brief Building H2O
 * @date 2022-04-23
 * 
 */

#include "proj2.h"

/**
 * @brief inicializuje semafory
 * 
 * @param sem struktura se semafory
 */
void semaphore_init(semaphore_t *sem)
{
    sem->barrier_mutex = sem_open(BARRIER_MUTEX, O_CREAT | O_EXCL, 0666, 1);
    if(sem->barrier_mutex == SEM_FAILED){
        fprintf(stderr, "Failed to create barrier_mutex!\n");
        cleanup(sem);
        exit(1);
    }

    sem->barrier_turnstile = sem_open(BARRIER_TURNSTILE, O_CREAT | O_EXCL, 0666, 0);
    if(sem->barrier_turnstile == SEM_FAILED){
        fprintf(stderr, "Failed to create barrier_turnstile!\n");
        cleanup(sem);
        exit(1);
    }

    sem->barrier_turnstile2 = sem_open(BARRIER_TURNSTILE2, O_CREAT | O_EXCL, 0666, 1);
    if(sem->barrier_turnstile2 == SEM_FAILED){
        fprintf(stderr, "Failed to create barrier_turnstile2!\n");
        cleanup(sem);
        exit(1);
    }

    sem->mutex = sem_open(MUTEX, O_CREAT | O_EXCL, 0666, 1);
    if(sem->mutex == SEM_FAILED){
        fprintf(stderr, "Failed to create mutex!\n");
        cleanup(sem);
        exit(1);
    }

    sem->Hqueue = sem_open(HQUEUE, O_CREAT | O_EXCL, 0666, 0);
    if(sem->Hqueue == SEM_FAILED){
        fprintf(stderr, "Failed to create Hqueue!\n");
        cleanup(sem);
        exit(1);
    }

    sem->Oqueue = sem_open(OQUEUE, O_CREAT | O_EXCL, 0666, 0);
    if(sem->Oqueue == SEM_FAILED){
        fprintf(stderr, "Failed to create Oqueue!\n");
        cleanup(sem);
        exit(1);
    }

    sem->zapis = sem_open(ZAPIS, O_CREAT | O_EXCL, 0666, 1);
    if(sem->zapis == SEM_FAILED){
        fprintf(stderr, "Failed to create zapis semaphore!\n");
        cleanup(sem);
        exit(1);
    }

    sem->molecule_done = sem_open(MOLECULE_DONE, O_CREAT | O_EXCL, 0666, 0);
    if(sem->molecule_done == SEM_FAILED){
        fprintf(stderr, "Failed to create molecule_done semaphore!\n");
        cleanup(sem);
        exit(1);
    }

    sem->hydrogen_check = sem_open(HYDROGEN_CHECK, O_CREAT | O_EXCL, 0666, 1);
    if(sem->hydrogen_check == SEM_FAILED){
        fprintf(stderr, "Failed to create hydrogen_check semaphore!\n");
        cleanup(sem);
        exit(1);
    }
}

/**
 * @brief funkce pro unlink semaforu
 * 
 * @param sem strukrura se semafoty
 */
void semaphore_unlink()
{
    sem_unlink(BARRIER_MUTEX);
    sem_unlink(BARRIER_TURNSTILE);
    sem_unlink(BARRIER_TURNSTILE2);
    sem_unlink(MUTEX);
    sem_unlink(HQUEUE);
    sem_unlink(OQUEUE);
    sem_unlink(ZAPIS);
    sem_unlink(MOLECULE_DONE);
    sem_unlink(HYDROGEN_CHECK);
}

/**
 * @brief funkce pro uzavreni semaforu
 * 
 * @param sem strukrura se semafoty
 */
void semaphore_close(semaphore_t *sem)
{
    sem_close(sem->barrier_mutex);
    sem_close(sem->barrier_turnstile);
    sem_close(sem->barrier_turnstile2);
    sem_close(sem->mutex);
    sem_close(sem->Hqueue);
    sem_close(sem->Oqueue);
    sem_close(sem->zapis);
    sem_close(sem->molecule_done);
    sem_close(sem->hydrogen_check);
}

/**
 * @brief funkce na celkovy uklid
 * 
 * @param sem struktura semaforu
 */
void cleanup(semaphore_t *sem)
{
    semaphore_close(sem);
    semaphore_unlink();
}

/**
 * @brief funkce na uvolneni sdilenych promennych
 */
void shr_var_dtor(semaphore_t *sem, uint32_t *oxygen_counter, uint32_t *hydrogen_counter, uint32_t *counter, uint32_t *noM, uint32_t *idH, uint32_t *idO, int32_t *barrier_counter, bool *O_green_flag, bool *H_green_flag, uint32_t *oxygen_pass_counter, uint32_t *hydrogen_pass_counter)
{
    munmap(sem, sizeof(semaphore_t));
    munmap(oxygen_counter, 32);
    munmap(hydrogen_counter, 32);
    munmap(counter, 32);
    munmap(noM, 32);
    munmap(idH, 32);
    munmap(idO, 32);
    munmap(barrier_counter, 32);
    munmap(O_green_flag, sizeof(bool));
    munmap(H_green_flag, sizeof(bool));
    munmap(oxygen_pass_counter, 32);
    munmap(hydrogen_pass_counter, 32);
}

/**
 * @brief funkce na zpracovani parametru
 * 
 * @param argc pocet parametru
 * @param argv pole paramtru
 * @param params struktura pro ulozeni pamarametru
 */
void parms_f(int argc, char *argv[], params_t *params)
{
    //chybny pocet argumentu
    if(argc != 5){
        fprintf(stderr, "Invalid input!\nExpected: %s NO NH TI TB\n", argv[0]);
        exit(1);
    }

    //testovani na NAN
    size_t lenght;
    for(int i = 1; i < 5; i++){
        lenght = strlen(argv[i]);
        if(lenght == 0){
            fprintf(stderr, "Invalid input!\nExpected: %s NO NH TI TB\n", argv[0]);
            exit(1);
        }
        for(size_t k = 0; k < lenght; k++){
            if(!isdigit(argv[i][k])){
                fprintf(stderr, "%s is not a number!\n", argv[i]);
                exit(1);
            }
        }
    }

    //NO
    params->no = atoi(argv[1]);
    if(params->no <= 0){
        fprintf(stderr, "NO must be > 0!\n");
        exit(1);
    }

    //NH
    params->nh = atoi(argv[2]);
    if(params->nh <= 0){
        fprintf(stderr, "NH must be > 0!\n");
        exit(1);
    }

    //TI
    params->ti = atoi(argv[3]);
    if(params->ti < 0 || params->ti > 1000){
        fprintf(stderr, "TI must be 0<=TI<=1000!\n");
        exit(1);
    }

    //TB
    params->tb = atoi(argv[4]);
    if(params->tb < 0 || params->tb > 1000){
        fprintf(stderr, "TB must be 0<=TB<=1000!\n");
        exit(1);
    }

}

/**
 * @brief čeká náhodný čas nez se zaradi do fronty
 * 
 * @param params struktura s parametry
 */
void sleep_before_queue(params_t *params)
{
    if(params->ti != 0){
        usleep((rand()%params->ti)*1000);
    }
    else{
        usleep(0);
    }
}

/**
 * @brief simuluje vytvareni molekuly
 * 
 * @param params struktura s parametry
 */
void sleep_molecule(params_t *params)
{
    if(params->tb != 0){
        usleep((rand()%params->tb)*1000);
    }
    else{
        usleep(0);
    }
}

/**
 * @brief funkce simulujici proces kyslik
 */
void process_oxygen(semaphore_t *sem, params_t *params, uint32_t *idO, FILE *fp, uint32_t *counter, uint32_t *oxygen_counter, uint32_t *hydrogen_counter, uint32_t *noM, int32_t *barrier_counter, bool *O_green_flag, bool *H_green_flag, uint32_t *oxygen_pass_counter, uint32_t *hydrogen_pass_counter)
{
    //lock zapis
    sem_wait(sem->zapis);

    *idO += 1;
    int id = *idO;
    *counter += 1;

    fprintf(fp, "%d: O %d: started\n", *counter, id);
    fflush(fp);

    //unlock zapis
    sem_post(sem->zapis);

    sleep_before_queue(params);

    //lock zapis
    sem_wait(sem->zapis);
    
    *counter += 1;
    fprintf(fp, "%d: O %d: going to queue\n", *counter, id);
    fflush(fp);
    
    //unlock zapis
    sem_post(sem->zapis);

    //lock mutex
    sem_wait(sem->mutex);

    *oxygen_pass_counter += 1;
    *oxygen_counter += 1;

    //vstup, ktery nevytvori zadnou molekulu (resp. ./proj2 x 1 x x)
    if((*oxygen_pass_counter == (unsigned)params->no) && (*hydrogen_pass_counter == (unsigned)params->nh) && (params->nh == 1)){
        *O_green_flag = true;
        *H_green_flag = true;
        unsigned max1 = *oxygen_counter;
        unsigned max2 = *hydrogen_counter;
        for(unsigned i = 0; i < max1; i++){
            sem_post(sem->Oqueue);
        }
        for(unsigned k = 0; k < max2; k++){
            sem_post(sem->Hqueue);
        }
    }
    else if((*hydrogen_pass_counter == (unsigned)params->nh) && (*hydrogen_counter < 2)){
        *O_green_flag = true;
        *H_green_flag = true;
        unsigned max3 = *oxygen_counter;
        unsigned max4 = *hydrogen_counter;
        for(unsigned i = 0; i < max3; i++){
            sem_post(sem->Oqueue);
            *oxygen_counter -= 1;
        }
        for(unsigned k = 0; k < max4; k++){
            sem_post(sem->Hqueue);
            *hydrogen_counter -= 1;
        }
        sem_post(sem->mutex);
    }
    else if(*hydrogen_counter >= 2){
        //dostatek vodiku
        sem_post(sem->Hqueue);
        sem_post(sem->Hqueue);
        *hydrogen_counter -= 2;
        sem_post(sem->Oqueue);
        *oxygen_counter -= 1;
    }
    else{
        sem_post(sem->mutex);
    }
 

    //fronta
    sem_wait(sem->Oqueue);
/*** vytvareni molekuly ***/

    if(*O_green_flag == false){
    //lock zapis
    sem_wait(sem->zapis);

    *counter += 1;
    fprintf(fp,"%d: O %d: creating molecule %d\n", *counter, id, *noM);
    fflush(fp);

    //unlock zapisu
    sem_post(sem->zapis);

    //simulace vytvareni molekuly
    sleep_molecule(params);

    //informuje vodiky
    sem_post(sem->molecule_done);
    sem_post(sem->molecule_done);

    //bariera
    sem_wait(sem->barrier_mutex);
    
    *barrier_counter += 1;
    if(*barrier_counter == 3){
        sem_wait(sem->barrier_turnstile2);  //zamceni druhe casti bariery
        sem_post(sem->barrier_turnstile);   //odemceni prvni casti bariery
    }
    
    sem_post(sem->barrier_mutex);

    sem_wait(sem->barrier_turnstile);   //prvni turniket
    sem_post(sem->barrier_turnstile);
    
    //lock zapis
    sem_wait(sem->zapis);

    *counter += 1;
    fprintf(fp,"%d: O %d: molecule %d created\n", *counter, id, *noM);
    fflush(fp);

    //unlock zapisu
    sem_post(sem->zapis);

    sem_wait(sem->barrier_mutex);

    *barrier_counter -= 1;
    if(*barrier_counter == 0){
        sem_wait(sem->barrier_turnstile);
        sem_post(sem->barrier_turnstile2);
    }
    
    sem_post(sem->barrier_mutex);

    sem_wait(sem->barrier_turnstile2);  //druhy turniket

    sem_post(sem->barrier_turnstile2);
    //konec bariery

    //inkrementace cisla molekuly
    *noM += 1; 

    if((*hydrogen_pass_counter == (unsigned)params->nh) && (*oxygen_pass_counter == (unsigned)params->no) && (*oxygen_counter > 0)){
        *O_green_flag = true;
        unsigned max5 = *oxygen_counter;
        for(unsigned i = 0; i < max5; i++){
            sem_post(sem->Oqueue);
            *oxygen_counter -= 1;
        }
    }
    
    //unlock mutex
    sem_post(sem->mutex);
    }
    else{
        //lock zapis
        sem_wait(sem->zapis);

        *counter += 1;
        fprintf(fp,"%d: O %d: not enough H\n", *counter, id);
        fflush(fp);

        //unlock zapisu
        sem_post(sem->zapis);
    }
    exit(0);

}

/**
 * @brief funkce simulujici proces vodik
 */
void process_hydrogen(semaphore_t *sem, params_t *params, uint32_t *idH, FILE *fp, uint32_t *counter, uint32_t *hydrogen_counter, uint32_t *oxygen_counter, uint32_t *noM, int32_t *barrier_counter, bool *H_green_flag, bool *O_green_flag, uint32_t *oxygen_pass_counter, uint32_t *hydrogen_pass_counter)
{
    //lock zapis
    sem_wait(sem->zapis);

    *idH += 1;
    int id = *idH;
    *counter += 1;

    fprintf(fp, "%d: H %d: started\n", *counter, id);
    fflush(fp);

    //unlock zapis
    sem_post(sem->zapis);

    sleep_before_queue(params);

    //lock zapis
    sem_wait(sem->zapis);
    
    *counter += 1;
    fprintf(fp, "%d: H %d: going to queue\n", *counter, id);
    fflush(fp);
    
    //unlock zapis
    sem_post(sem->zapis);

    //lock mutex
    sem_wait(sem->mutex);

    *hydrogen_counter += 1;
    *hydrogen_pass_counter += 1;
     
    //vstup, ktery nevytvori zadnou molekulu (resp. ./proj2 x 1 x x)
    if((*oxygen_pass_counter == (unsigned)params->no) && (*hydrogen_pass_counter == (unsigned)params->nh) && (params->nh == 1)){
        *O_green_flag = true;
        *H_green_flag = true;
        unsigned max1 = *oxygen_counter;
        unsigned max2 = *hydrogen_counter;
        for(unsigned i = 0; i < max1; i++){
            sem_post(sem->Oqueue);
            *oxygen_counter -= 1;
        }
        for(unsigned k = 0; k < max2; k++){
            sem_post(sem->Hqueue);
            *hydrogen_counter -= 1;
        }
    }
    else if(((*hydrogen_pass_counter == (unsigned)params->nh) && (*hydrogen_counter < 2)) || ((*oxygen_pass_counter == (unsigned)params->no) && (*oxygen_counter == 0))){
        *O_green_flag = true;
        *H_green_flag = true;
        unsigned max3 = *oxygen_counter;
        unsigned max4 = *hydrogen_counter;
        for(unsigned i = 0; i < max3; i++){
            sem_post(sem->Oqueue);
        }
        for(unsigned k = 0; k < max4; k++){
            sem_post(sem->Hqueue);
        }
        sem_post(sem->mutex);
    }
    else if((*hydrogen_counter >= 2) && (*oxygen_counter >= 1)){
        sem_post(sem->Hqueue);
        sem_post(sem->Hqueue);
        *hydrogen_counter -= 2;
        sem_post(sem->Oqueue);
        *oxygen_counter -= 1;
    }
    else{
        sem_post(sem->mutex);
    }

    //fronta vodiku
    sem_wait(sem->Hqueue);

/*** vytvareni molekuly ***/

    if(*H_green_flag == false){
    //lock zapis
    sem_wait(sem->zapis);

    *counter += 1;
    fprintf(fp,"%d: H %d: creating molecule %d\n", *counter, id, *noM);
    fflush(fp);

    //unlock zapisu
    sem_post(sem->zapis);

    //cekani na signal od kysliku
    sem_wait(sem->molecule_done);

    //bariera
    sem_wait(sem->barrier_mutex);
    
    *barrier_counter += 1;
    if(*barrier_counter == 3){
        sem_wait(sem->barrier_turnstile2);  //zamceni druhe casti bariery
        sem_post(sem->barrier_turnstile);   //odemceni prvni casti bariery
    }
    
    sem_post(sem->barrier_mutex);

    sem_wait(sem->barrier_turnstile);   //prvni turniket
    sem_post(sem->barrier_turnstile);

    //lock zapis
    sem_wait(sem->zapis);

    *counter += 1;
    fprintf(fp,"%d: H %d: molecule %d created\n", *counter, id, *noM);
    fflush(fp);

    //unlock zapisu
    sem_post(sem->zapis);

    sem_wait(sem->barrier_mutex);

    *barrier_counter -= 1;
    if(*barrier_counter == 0){
        sem_wait(sem->barrier_turnstile);
        sem_post(sem->barrier_turnstile2);
    }

    sem_post(sem->barrier_mutex);

    sem_wait(sem->barrier_turnstile2);  //druhy turniket

    sem_post(sem->barrier_turnstile2);
    //konec bariery

    sem_wait(sem->hydrogen_check);
    if((*hydrogen_pass_counter == (unsigned)params->nh) && (*oxygen_pass_counter == (unsigned)params->no) && (*hydrogen_counter > 0)){
        *H_green_flag = true;
        unsigned max5 = *hydrogen_counter;
        for(unsigned k = 0; k < max5; k++){
            sem_post(sem->Hqueue);
            *hydrogen_counter -= 1;
        }
    }
    sem_post(sem->hydrogen_check);

    }
    else{
        //lock zapis
        sem_wait(sem->zapis);

        *counter += 1;
        fprintf(fp,"%d: H %d: not enough O or H\n", *counter, id);
        fflush(fp);

        //unlock zapisu
        sem_post(sem->zapis);
    }

    exit(0);
}

int main(int argc, char *argv[])
{
    //inicializace struktury pro parametry
    params_t params;
    parms_f(argc, argv, &params); 

    //otevreni souboru proj2.out
    FILE *fp;
    fp = fopen("proj2.out", "w");

    if(fp == NULL){
        fprintf(stderr, "Failed to open / create proj2.out!\n");
        exit(1);
    }

    //nastaveni "splachovani" outputu (zabraneni buffrovani)
   // setbuf(fp, NULL);

    //sdilene promenne (podrobnejsi komentare v .h souboru)
    semaphore_t *sem = mmap(NULL, sizeof(semaphore_t), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    uint32_t *oxygen_counter = mmap(NULL, 32, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    uint32_t *hydrogen_counter = mmap(NULL, 32, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    uint32_t *counter = mmap(NULL, 32, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    uint32_t *noM = mmap(NULL, 32, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    uint32_t *idO = mmap(NULL, 32, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0); 
    uint32_t *idH = mmap(NULL, 32, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    int32_t *barrier_counter = mmap(NULL, 32, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    bool *O_green_flag = mmap(NULL, sizeof(bool), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    bool *H_green_flag = mmap(NULL, sizeof(bool), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    uint32_t *oxygen_pass_cnt = mmap(NULL, sizeof(bool), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    uint32_t *hydrogen_pass_cnt = mmap(NULL, sizeof(bool), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);

    *oxygen_counter = 0;
    *hydrogen_counter = 0;
    *counter = 0;
    *noM = 1;
    *idO = 0;
    *idH = 0;
    *barrier_counter = 0;
    *O_green_flag = false;
    *H_green_flag = false;
    *oxygen_pass_cnt = 0;
    *hydrogen_pass_cnt = 0;

    //unlink semaforu pro pripad, ze existuji
    semaphore_unlink();

    //vytvoreni (inicializace) semaforu   
    semaphore_init(sem);

/*
****** start hlavniho procesu ******
*/

    //oxygen procesy
    for(int i = 0; i < params.no; i++){
        pid_t oxygen_pid = fork();
    
        if(oxygen_pid == 0){
            process_oxygen(sem, &params, idO, fp, counter, oxygen_counter, hydrogen_counter, noM, barrier_counter, O_green_flag, H_green_flag, oxygen_pass_cnt, hydrogen_pass_cnt);
        }
        else if(oxygen_pid < 0){
            fprintf(stderr, "Failed to create oxygen process!\n");
            cleanup(sem);
            shr_var_dtor(sem, oxygen_counter, hydrogen_counter, counter, noM, idH, idO, barrier_counter, O_green_flag, H_green_flag, oxygen_pass_cnt, hydrogen_pass_cnt);
            fclose(fp);
            exit(1);
        }
    
    }

    //hydrogen procesy
    for(int k = 0; k < params.nh; k++){
        pid_t hydrogen_pid = fork();
        if(hydrogen_pid == 0){
            process_hydrogen(sem, &params, idH, fp, counter, hydrogen_counter, oxygen_counter, noM, barrier_counter, H_green_flag, O_green_flag, oxygen_pass_cnt, hydrogen_pass_cnt);
        }
        else if(hydrogen_pid < 0){
            fprintf(stderr, "Failed to create hydrogen process!\n");
            cleanup(sem);
            shr_var_dtor(sem, oxygen_counter, hydrogen_counter, counter, noM, idH, idO, barrier_counter, O_green_flag, H_green_flag, oxygen_pass_cnt, hydrogen_pass_cnt);
            fclose(fp);
            exit(1);
        }
    }

    //cekani na smrt vsech potomku
    while(wait(NULL) > 0);

    //uklid
    cleanup(sem);

    //uzavreni souboru
    fclose(fp);

    //uvolneni sdilenych promennych
    shr_var_dtor(sem, oxygen_counter, hydrogen_counter, counter, noM, idH, idO, barrier_counter, O_green_flag, H_green_flag, oxygen_pass_cnt, hydrogen_pass_cnt);


    return 0;
}