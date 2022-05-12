///////////////////////////////////////////////////////////////////
// ADT Vector
// Abstract "array" μεταβλητού μεγέθους.
///////////////////////////////////////////////////////////////////

#pragma once

#include "common_types.h"

typedef struct vector_node* VectorNode;

typedef struct vector* Vector;

VectorNode create_vector_node(Pointer value);

//Δημιουργεί και επιστρέφει ένα νεό vector μεγέθους size, με στοιχεία αρχικοποιημένα σε NULL.
//Αν δεν υπάρχει διαθέσιμη μνήμη επιστρέφει VECTOR_FAIL.
//Αν destroy_value != NULL, τότε καλείται destroy_value(value) κάθε φορά που αφαιρείται (ή αντικαθίσταται) ένα στοιχείο.
Vector vector_create(int size, DestroyFunc destroy_value);

//Για χρήση σε hashtables

void vector_insert(Vector vec, Pointer item);
void vector_remove(Vector vec, VectorNode node);

DestroyFunc vector_set_destroy_key(Vector vec, DestroyFunc destroy_key);
void vector_set_hash_function(Vector vec, HashFunc func);

//Επιστρέφει τον αριθμό στοιχείων που περιέχει το vector vec.
int vector_size(Vector vec);

// //Eπιστρέφει το capacity του vec
// int vector_capacity(Vector vec);

//Προσθέτει την τιμή value στο _τέλος_ του vector vec. Το μέγεθος του vector μεγαλώνει κατά 1.
void vector_insert_last(Vector vec, Pointer value);

//Αφαιρεί το τελευταίο στοιχείο του vector. Το μέγεθος του vector μικραίνει κατά 1. Αν το vector είναι κενό η συμπεριφορά είναι μη ορισμένη.
void vector_remove_last(Vector vec);

//Επιστρέφει την τιμή στη θέση pos του vector vec (μη ορισμένο αποτέλεσμα αν pos < 0 ή pos >= size)
Pointer vector_get_at(Vector vec, int pos);

//Αλλάζει την τιμή στη θέση pos του Vector vec σε value. ΔΕΝ μεταβάλλει το μέγεθος του vector, αν pos >= size το αποτέλεσμα δεν είναι ορισμένο.
void vector_set_at(Vector vec, int pos, Pointer value);

//Βρίσκει και επιστρέφει το πρώτο στοιχείο στο vector που να είναι ίσο με value ή NULL αν δεν βρεθεί κανένα στοιχείο.
Pointer vector_find(Vector vec, Pointer value, CompareFunc compare);

//Αλλάζει τη συνάρτηση που καλείται σε κάθε αφαίρεση/αντικατάσταση στοιχείου σε destroy_value. Επιστρέφει την προηγούμενη τιμή της συνάρτησης.
DestroyFunc vector_set_destroy_value(Vector vec, DestroyFunc destroy_value);

//Ελευθερώνει όλη τη μνήμη που δεσμεύει το vector vec.
void vector_destroy(Vector vec);


/////////////////////////////////////////// Διάσχιση του vector ////////////////////////////////////////////////////////////

//Επιστρέφουν τον πρώτο και τον τελευταίο κομβο του vector ή NULL αν το vector είναι κενό

VectorNode vector_first(Vector vec);
VectorNode vector_last(Vector vec);

//Επιστρέφουν τον επόμενο και τον προηγούμενο κομβο του node ή NULL αν ο node δεν έχει επόμενο / προηγούμενο.

VectorNode vector_next(Vector vec, VectorNode node);
VectorNode vector_previous(Vector vec, VectorNode node);

//Επιστρέφει το περιεχόμενο του κόμβου node
Pointer vector_node_value(Vector vec, VectorNode node);

//Βρίσκει το πρώτο στοιχείο στο vector που να είναι ίσο με value. Επιστρέφει τον κόμβο του στοιχείου ή NULL αν δεν βρεθεί.
VectorNode vector_find_node(Vector vec, Pointer value, CompareFunc compare);
