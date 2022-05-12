////////////////////////////////////////////////////////////////////////
// ADT Set
// Abstract διατεταγμένο σύνολο. Τα στοιχεία είναι διατεταγμένα βάσει
// της compare και καθένα εμφανίζεται το πολύ μία φορά.
// Παρέχεται γρήγορη αναζήτηση με ισότητα και ανισότητα.
////////////////////////////////////////////////////////////////////////

#pragma once

#include "common_types.h"
#include "ADTVector.h"

typedef struct set* Set;

//Δημιουργεί set με compare. Αν destroy_value != NULL, τότε καλείται destroy_value(value) κάθε φορά που αφαιρείται ένα στοιχείο.
Set set_create(CompareFunc compare, DestroyFunc destroy_value, Vector values);

//Επιστρέφει τον αριθμό στοιχείων που περιέχει το set.
int set_size(Set set);

//Προσθέτει την τιμή value στο σύνολο, αντικαθιστώντας τυχόν προηγούμενη τιμή ισοδύναμη της value.
void set_insert(Set set, Pointer value);

//Αφαιρεί τη μοναδική τιμή ισοδύναμη της value από το σύνολο, αν υπάρχει. Επιστρέφει true αν βρέθηκε η τιμή αυτή, false διαφορετικά.
bool set_remove(Set set, Pointer value);

//Επιστρέφει την μοναδική τιμή του set που είναι ισοδύναμη με value, ή NULL αν δεν υπάρχει
Pointer set_find(Set set, Pointer value);

//Αλλάζει τη συνάρτηση που καλείται σε κάθε αφαίρεση/αντικατάσταση στοιχείου σε destroy_value/key. Επιστρέφει την προηγούμενη τιμή της συνάρτησης.
DestroyFunc set_set_destroy_value(Set set, DestroyFunc destroy_value);

//Ελευθερώνει όλη τη μνήμη που δεσμεύει το σύνολο.
void set_destroy(Set set);

/////////////////////////////Διάσχιση του set ////////////////////////////////////////////////////////////
//Η διάσχιση γίνεται με τη σειρά διάταξης.

typedef struct set_node* SetNode;

//Επιστρέφουν τον πρώτο και τον τελευταίο κομβο του set, ή NULL αντίστοιχα αν το set είναι κενό

SetNode set_first(Set set);
SetNode set_last(Set set);

//Επιστρέφουν τον επόμενο και τον προηγούμενο κομβο του node, ή NULL αντίστοιχα αν ο node δεν έχει επόμενο/προηγούμενο.

SetNode set_next(Set set, SetNode node);
SetNode set_previous(Set set, SetNode node);

//Επιστρέφει το περιεχόμενο του κόμβου node
Pointer set_node_value(Set set, SetNode node);

//Βρίσκει το μοναδικό στοιχείο στο set που να είναι ίσο με value. Επιστρέφει τον κόμβο του στοιχείου, ή SET_EOF αν δεν βρεθεί.
SetNode set_find_node(Set set, Pointer value);

//Αφαιρεί το node από το set σε O(1)
void set_remove_node(Set set, SetNode node);

//Δείκτης σε συνάρτηση που "επισκέπτεται" ένα στοιχείο value
typedef void (*VisitFunc)(Pointer value);

// Καλεί τη visit(value) για κάθε στοιχείο του set σε διατεταγμένη σειρά
void set_visit(Set set, VisitFunc visit);

// Υπολογίζει το συνολικό μήκος μονοπατιών (για BST!)
int set_length(Set set);

// Eπιστρέφει τον αριθμό των rotations (για AVL!!)
int set_rotations(Set set);