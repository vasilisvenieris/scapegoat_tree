#pragma once

#include <stdbool.h> 
#include <assert.h>
#include <limits.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <time.h>
#include <math.h>
#include <signal.h>
#include <error.h>
#include <errno.h>

typedef enum{
	EMPTY, OCCUPIED, REMOVED
} Status;

// Pointer προς ένα αντικείμενο οποιουδήποτε τύπου.
typedef void* Pointer;

typedef char* String;

//unsigned int
typedef unsigned int uint;

//Δείκτης σε συνάρτηση που συγκρίνει 2 στοιχεία a και b και επιστρέφει:
// < 0  αν a < b
//   0  αν a και b είναι ισοδύναμα (_όχι_ αναγκαστικά ίσα)
// > 0  αν a > b
typedef int (*CompareFunc)(Pointer a, Pointer b);

//Δείκτης σε συνάρτηση που καταστρέφει ένα στοιχείο value
typedef void (*DestroyFunc)(Pointer value);

//Τύπος συνάρτησης κατακερματισμού
typedef uint (*HashFunc)(Pointer);