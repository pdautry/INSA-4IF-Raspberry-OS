#include <stdint.h>
#include "kheap.h"
#include "hw.h"

// Globales -------------------------------------------
// Structure de file simple
struct fl {
	struct fl	*next;
	unsigned int	size;
} *freelist = (struct fl *) 0;

// Deux constantes definissant le haut du tas et la limite du tas
uint8_t* kernel_heap_top;
uint8_t* kernel_heap_limit;
uint8_t* kernel_heap_start;
/*  
    Protection d'initialisation
*/
int kheap_init_locker = 0;

// Fonctions -------------------------------------------


/**
 * 	Retourne une addr ...
 *	@param addr
 * 		Valeur d'adresse
 *  @param pwr_of_2
 *		X dans 2^X
 */
unsigned int aligned_value(unsigned int addr, unsigned int pwr_of_2)
{    
	// On decale 1 de power_of_2 bits vers la gauche
	// ex : si addr = 0b01101 et pwr_of_2 = 4 
    unsigned int modulo = (1 << pwr_of_2);
    // ex : alors modulo = 0b000010000 
    // On soustrait 1 a la valeur precedente
    unsigned int max_value_modulo = modulo - 1;
    // ex : alors max_value_modulo =  0b000001111
    // On retourne l'adresse donnee en parametre a laquelle on 
    // additionne la valeur du max du module et on applique un 
    // filtre pour mettre a zero les power_of_2 - 1 bits de 
    // poids faible 
    return (addr + max_value_modulo) & ~max_value_modulo;
    // ex :   0b000001101 
    //		+ 0b000001111
    //		= 0b000011100
    //		& 0b111110000
    //		= 0b000010000
}

/**
 *
 *	@param size
 *		
 *	@param pwr_of_2
 *		
 */
uint8_t* kAlloc_aligned(unsigned int size, unsigned int pwr_of_2)
{
	register struct fl *cfl = freelist, **prev;
	unsigned int aligned_cfl = aligned_value((unsigned int) cfl, pwr_of_2);
	unsigned int size_aligned = aligned_value(size, 2);

	prev = &freelist;
	while (cfl && (aligned_cfl != (unsigned int) cfl || cfl->size != size_aligned))
	{
	    prev = &(cfl->next);
	    cfl = cfl->next;
	    aligned_cfl = aligned_value((unsigned int) cfl, pwr_of_2);
	}

	if (! cfl)
	{
	    cfl = (struct fl *) kernel_heap_top;
	    aligned_cfl = aligned_value((unsigned int) cfl, pwr_of_2);

	    if (aligned_cfl == (unsigned int) cfl)
	    {
		kernel_heap_top += size_aligned;
	    }
	    else
	    {
		kFree((uint8_t*) cfl, (aligned_cfl - (unsigned int) cfl));
		kernel_heap_top = (uint8_t*) (aligned_cfl + size_aligned);
	    }

	    /* No space available anymore */
	    if (kernel_heap_top > kernel_heap_limit)
		return FORBIDDEN_ADDRESS;
	}
	else
	{
	    if (aligned_cfl == (unsigned int) cfl)
	    {
		*prev = cfl->next;
	    }
	    else
	    {
		kFree((uint8_t*) cfl, cfl->size - size_aligned);
	    }
	}

	/* Fill with FORBIDDEN_BYTE to debug (more) easily */
	for (int i = 0 ; i < size_aligned ; i++) {
	    *(((uint8_t*) aligned_cfl) + i) = FORBIDDEN_BYTE;
	}
	
	return ((uint8_t *) aligned_cfl);
}

/**
 *	Cette fonction alloue un bloc de size octets et 
 *	retourne l'adresse de depart de ce bloc 
 *	@param size
 *		Taille du bloc de memoire allouee en octet
 */
uint8_t* kAlloc(unsigned int size)
{
	register struct fl *cfl = freelist, **prev;
	unsigned int size_aligned = (size + 3) & ~3;

	prev = &freelist;

	while (cfl && cfl->size != size_aligned)
	{
	    prev = &cfl->next;
	    cfl = cfl->next;
	}

	if (! cfl)
	{
	    cfl = (struct fl *) kernel_heap_top;
	    kernel_heap_top += size_aligned;

	    /* No space available anymore */
	    if (kernel_heap_top > kernel_heap_limit)
		return FORBIDDEN_ADDRESS;
	}
	else
	{
	    *prev = cfl->next;
	}

	/* Fill with FORBIDDEN_BYTE to debug (more) easily */
	for (int i = 0 ; i < size_aligned ; i++) {
	    *((uint8_t*) cfl) = FORBIDDEN_BYTE;
	}

	return ((uint8_t *) cfl);
}

/**
 *	Cette fonction libere un bloc de size octets a partir de 
 *  l'adresse ptr
 *	@param ptr
 *		Adresse de depart du bloc a liberer
 *	@param size
 *		Taille du bloc a liberer en octets
 */
void kFree(uint8_t* ptr, unsigned int size)
{
	register struct fl* cfl = (struct fl*) ptr;

	cfl->size = (size + 3) & ~3;
	cfl->next = freelist;
	freelist = cfl;
}

/**
 * 	Cette fonction initialise les adresses de reference du tas
 */
void kheap_init()
{
	if(kheap_init_locker)
	{	log_err("Heap initialization called twice !");
		log_cr();
	}

    kernel_heap_top = (uint8_t*) &__kernel_heap_start__;
    kernel_heap_limit = (uint8_t*) &__kernel_heap_end__;
    kernel_heap_start = (uint8_t*) &__kernel_heap_start__;

    // Activation de la protection
    kheap_init_locker = 1;
}
