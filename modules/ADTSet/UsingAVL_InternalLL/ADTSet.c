///////////////////////////////////////////////////////////
//
// Υλοποίηση του ADT Set μέσω AVL Tree
//
///////////////////////////////////////////////////////////

#include "ADTSet.h"


// Υλοποιούμε τον ADT Set μέσω AVL, οπότε το struct set είναι ένα AVL Δέντρο.
struct set {
	SetNode root;				// η ρίζα, NULL αν είναι κενό δέντρο
	int size;					// μέγεθος, ώστε η set_size να είναι Ο(1)
	CompareFunc compare;		// η διάταξη
	DestroyFunc destroy_value;	// Συνάρτηση που καταστρέφει ένα στοιχείο του set
	SetNode first, last;
	int rotations;
};

// Ενώ το struct set_node είναι κόμβος ενός AVL Δέντρου Αναζήτησης
struct set_node {
	SetNode left, right;//, parent;		// Παιδιά
	Pointer value;				// Τιμή κόμβου
	int height;					// Ύψος που βρίσκεται ο κόμβος στο δέντρο
	SetNode next, prev;

	int tot_length;
	int tree_size;
};


//// Συναρτήσεις που υλοποιούν επιπλέον λειτουργίες του AVL σε σχέση με ένα απλό BST /////////////////////////////////////
// Εστω tuple (node_edges, node_size) σε κάθε node
// To άθροισμα όλων των ακμών ενός δέντρου υπολογίζεται ακολούθως:
// Εστω (node_edges_left = x1, node_size_left = y1), 
//		(node_edges_right = x2, node_size_right = y2),
//
// (node_edges_total, node_size_total) = (x1 + y1 + x2 + y2, y1 + y2 + 1)
int calc_length(SetNode node){
	if(node == NULL)
		return 0;

	node->tot_length = calc_length(node->left) + calc_length(node->right);

	if(node->left != NULL)
		node->tree_size += node->left->tree_size;
	if(node->right != NULL)
		node->tree_size += node->right->tree_size;

	return node->tot_length + node->tree_size - 1;
}

// υπολογίζει το άθροισμα όλων των μηκών ενός BST σε Ο(n)
int set_length(Set set){
	return calc_length(set->root);
}

// επιστρέφει τον αριθμό των rotations (για AVL!!)
int set_rotations(Set set){
	return set->rotations;
}

// Επιστρέφει τη max τιμή μεταξύ 2 ακεραίων

static int int_max(int a, int b) {
	return (a > b) ? a : b ;
}

// Επιστρέφει το ύψος που βρίσκεται ο κόμβος στο δέντρο

static int node_height(SetNode node) {
	if (!node) return 0;
	return node->height;
}

// Ενημερώνει το ύψος ενός κόμβου

static void node_update_height(SetNode node) {
	node->height = 1 + int_max(node_height(node->left), node_height(node->right));
}

// Επιστρέφει τη διαφορά ύψους μεταξύ αριστερού και δεξιού υπόδεντρου

static int node_balance(SetNode node) {
	return node_height(node->left) - node_height(node->right);
}

// Rotations : Όταν η διαφορά ύψους μεταξύ αριστερού και δεξιού υπόδεντρου είναι
// μεγαλύτερη του 1 το δέντρο δεν είναι πια AVL. Υπάρχουν 4 διαφορετικά
// rotations που εφαρμόζονται ανάλογα με την περίπτωση για να αποκατασταθεί η
// ισορροπία. Η κάθε συνάρτηση παίρνει ως όρισμα τον κόμβο που πρέπει να γίνει
// rotate, και επιστρέφει τη ρίζα του νέου υποδέντρου.

// Single left rotation

static SetNode node_rotate_left(SetNode node) {
	SetNode right_node = node->right;
	SetNode left_subtree = right_node->left;

	right_node->left = node;
	node->right = left_subtree;

	node_update_height(node);
	node_update_height(right_node);
	
	return right_node;
}

// Single right rotation

static SetNode node_rotate_right(SetNode node) {
	SetNode left_node = node->left;
	SetNode left_right = left_node->right;

	left_node->right = node;
	node->left = left_right;

	node_update_height(node);
	node_update_height(left_node);
	
	return left_node;
}

// Double left-right rotation

static SetNode node_rotate_left_right(SetNode node) {
	node->left = node_rotate_left(node->left);
	return node_rotate_right(node);
}

// Double right-left rotation

static SetNode node_rotate_right_left(SetNode node) {
	node->right = node_rotate_right(node->right);
	return node_rotate_left(node);
}

// Επισκευή του AVL property αν δεν ισχύει

static SetNode node_repair_balance(Set set, SetNode node) {
	node_update_height(node);

	int balance = node_balance(node);
	if (balance > 1) {
		set->rotations++;
		// το αριστερό υπόδεντρο είναι unbalanced
		if (node_balance(node->left) >= 0)
			return node_rotate_right(node);
		else{
			set->rotations++;
			return node_rotate_left_right(node);
		}
	}
	else if (balance < -1) {
		set->rotations++;
		// το δεξί υπόδεντρο είναι unbalanced
		if (node_balance(node->right) <= 0)
			return node_rotate_left(node);
		else{
			set->rotations++;
			return node_rotate_right_left(node);
		}	
	}

	// δεν χρειάστηκε να πραγματοποιηθεί rotation
	return node;
}


//// Συναρτήσεις που είναι (σχεδόν) _ολόιδιες_ με τις αντίστοιχες της BST υλοποίησης ////////////////
//
// Είναι σημαντικό να κατανοήσουμε πρώτα τον κώδικα του BST πριν από αυτόν του AVL.
// Θα μπορούσαμε οργανώνοντας τον κώδικα διαφορετικά να επαναχρησιμοποιήσουμε τις συναρτήσεις αυτές.
//
// Οι διαφορές είναι σημειωμένες με "AVL" σε σχόλιο

// Δημιουργεί και επιστρέφει έναν κόμβο με τιμή value (χωρίς παιδιά)
//
static SetNode node_create(Pointer value) {
	SetNode node = malloc(sizeof(*node));
	node->left = NULL;
	node->right = NULL;
	node->value = value;
	node->height = 1;			// AVL
	node->next = NULL;			//για Ο(1) πρόσβαση στον επόμενο
	node->prev = NULL;			//για Ο(1) πρόσβαση στον προηγούμενο

	node->tot_length = 0;
	node->tree_size = 1;
	return node;
}

// Επιστρέφει τον κόμβο με τιμή ίση με value στο υποδέντρο με ρίζα node, διαφορετικά NULL

static SetNode node_find_equal(SetNode node, CompareFunc compare, Pointer value) {
	// κενό υποδέντρο, δεν υπάρχει η τιμή
	if (node == NULL)
		return NULL;
	
	// Το πού βρίσκεται ο κόμβος που ψάχνουμε εξαρτάται από τη διάταξη της τιμής
	// value σε σχέση με την τιμή του τρέχοντος κόμβο (node->value)
	//
	int compare_res = compare(value, node->value);			// αποθήκευση για να μην καλέσουμε την compare 2 φορές
	if (compare_res == 0)									// value ισοδύναμη της node->value, βρήκαμε τον κόμβο
		return node;
	else if (compare_res < 0)								// value < node->value, ο κόμβος που ψάχνουμε είναι στο αριστερό υποδέντρο
		return node_find_equal(node->left, compare, value);
	else													// value > node->value, ο κόμβος που ψάχνουμε είνια στο δεξιό υποδέντρο
		return node_find_equal(node->right, compare, value);
}

// Επιστρέφει τον μικρότερο κόμβο του υποδέντρου με ρίζα node

static SetNode node_find_min(SetNode node) {
	return node != NULL && node->left != NULL
		? node_find_min(node->left)				// Υπάρχει αριστερό υποδέντρο, η μικρότερη τιμή βρίσκεται εκεί
		: node;									// Αλλιώς η μικρότερη τιμή είναι στο ίδιο το node
}

// Επιστρέφει τον μεγαλύτερο κόμβο του υποδέντρου με ρίζα node

static SetNode node_find_max(SetNode node) {
	return node != NULL && node->right != NULL
		? node_find_max(node->right)			// Υπάρχει δεξί υποδέντρο, η μεγαλύτερη τιμή βρίσκεται εκεί
		: node;									// Αλλιώς η μεγαλύτερη τιμή είναι στο ίδιο το node
}

// Αν υπάρχει κόμβος με τιμή ισοδύναμη της value, αλλάζει την τιμή του σε value, διαφορετικά προσθέτει
// νέο κόμβο με τιμή value. Επιστρέφει τη νέα ρίζα του υποδέντρου, και θέτει το *inserted σε true
// αν έγινε προσθήκη, ή false αν έγινε ενημέρωση.

static SetNode node_insert(Set set, SetNode node, CompareFunc compare, Pointer value, bool* inserted, Pointer* old_value) {
	// Αν το υποδέντρο είναι κενό, δημιουργούμε νέο κόμβο ο οποίος γίνεται ρίζα του υποδέντρου
	if (node == NULL) {
		*inserted = true;			// κάναμε προσθήκη
		return node_create(value);
	}
	
	// Το πού θα γίνει η προσθήκη εξαρτάται από τη διάταξη της τιμής
	// value σε σχέση με την τιμή του τρέχοντος κόμβου (node->value)
	//
	int compare_res = compare(value, node->value);
	if (compare_res == 0) {
		// βρήκαμε ισοδύναμη τιμή, κάνουμε update
		*inserted = false;
		*old_value = node->value;
		node->value = value;

	} else if (compare_res < 0) {
		// value < node->value, συνεχίζουμε αριστερά.
		node->left = node_insert(set, node->left, compare, value, inserted, old_value);
		if(node->left->next == NULL && node->left->prev == NULL){
			if(node->prev != NULL){
				node->prev->next = node->left;
				node->left->prev = node->prev;
			}
			node->prev = node->left;
			node->left->next = node;
		}
	} else {
		// value > node->value, συνεχίζουμε δεξιά
		node->right = node_insert(set, node->right, compare, value, inserted, old_value);
		if(node->right->next == NULL && node->right->prev == NULL){
			if(node->next != NULL){
				node->next->prev = node->right;
				node->right->next = node->next;
			}
			node->next = node->right;
			node->right->prev = node;
		}
	}
	
	return node_repair_balance(set, node);	// AVL
}

// Αφαιρεί και αποθηκεύει στο min_node τον μικρότερο κόμβο του υποδέντρου με ρίζα node.
// Επιστρέφει τη νέα ρίζα του υποδέντρου.

static SetNode node_remove_min(Set set, SetNode node, SetNode* min_node) {
	if (node->left == NULL) {
		// Δεν έχουμε αριστερό υποδέντρο, οπότε ο μικρότερος είναι ο ίδιος ο node
		*min_node = node;
		return node->right;		// νέα ρίζα είναι το δεξιό παιδί

	} else {
		// Εχουμε αριστερό υποδέντρο, οπότε η μικρότερη τιμή είναι εκεί. Συνεχίζουμε αναδρομικά
		// και ενημερώνουμε το node->left με τη νέα ρίζα του υποδέντρου.
		node->left = node_remove_min(set, node->left, min_node);

		return node_repair_balance(set, node);	// AVL
	}
}

// Διαγράφει το κόμβο με τιμή ισοδύναμη της value, αν υπάρχει. Επιστρέφει τη νέα ρίζα του
// υποδέντρου, και θέτει το *removed σε true αν έγινε πραγματικά διαγραφή.

static SetNode node_remove(Set set, SetNode node, CompareFunc compare, Pointer value, bool* removed, Pointer* old_value) {
	if (node == NULL) {
		*removed = false;		// κενό υποδέντρο, δεν υπάρχει η τιμή
		return NULL;
	}

	int compare_res = compare(value, node->value);
	if(compare_res == 0){
		// Βρέθηκε ισοδύναμη τιμή στον node, οπότε τον διαγράφουμε. Το πώς θα γίνει αυτό εξαρτάται από το αν έχει παιδιά.
		*removed = true;
		*old_value = node->value;

        //αποσυνδέουμε το node από τους υπόλοιπους κόμβους
		if(node->next != NULL)
			node->next->prev = node->prev;
		if(node->prev != NULL)
			node->prev->next = node->next;

		if(node->left == NULL){
			// Δεν υπάρχει αριστερό υποδέντρο, οπότε διαγράφεται απλά ο κόμβος και νέα ρίζα μπαίνει το δεξί παιδί
			SetNode right = node->right;	// αποθήκευση πριν το free!
			free(node);
			return right;

		} 
        else if(node->right == NULL){
			// Δεν υπάρχει δεξί υποδέντρο, οπότε διαγράφεται απλά ο κόμβος και νέα ρίζα μπαίνει το αριστερό παιδί
			SetNode left = node->left;		// αποθήκευση πριν το free!
			free(node);
			return left;

		}
        else{
			// Υπάρχουν και τα δύο παιδιά. Αντικαθιστούμε την τιμή του node με την μικρότερη του δεξιού υποδέντρου, η οποία
			// αφαιρείται. Η συνάρτηση node_remove_min κάνει ακριβώς αυτή τη δουλειά.

			SetNode min_right;
			node->right = node_remove_min(set, node->right, &min_right);

			// Σύνδεση του min_right στη θέση του node
			min_right->left = node->left;
			min_right->right = node->right;

			free(node);

			return node_repair_balance(set, min_right);	// AVL
		}
	}

	// compare_res != 0, συνεχίζουμε στο αριστερό ή δεξί υποδέντρο, η ρίζα δεν αλλάζει.
	if(compare_res < 0)
		node->left  = node_remove(set, node->left,  compare, value, removed, old_value);
	else
		node->right = node_remove(set, node->right, compare, value, removed, old_value);

	return node_repair_balance(set, node);	// AVL
}

// Καταστρέφει όλο το υποδέντρο με ρίζα node

static void node_destroy(SetNode node, DestroyFunc destroy_value) {
	if (node == NULL)
		return;
	
	// πρώτα destroy τα παιδιά, μετά free το node
	node_destroy(node->left, destroy_value);
	node_destroy(node->right, destroy_value);

	if (destroy_value != NULL)
		destroy_value(node->value);

	free(node);
}


//// Συναρτήσεις του ADT Set. Γενικά πολύ απλές, αφού καλούν τις αντίστοιχες node_* //////////////////////////////////
//
// Επίσης ολόιδιες με αυτές του BST-based Set

Set set_create(CompareFunc compare, DestroyFunc destroy_value, Vector values){
	// δημιουργούμε το set
	Set set = malloc(sizeof(*set));
	set->root = NULL;			// κενό δέντρο
	set->size = 0;
	set->compare = compare;
	set->destroy_value = destroy_value;
	set->rotations = 0;
	set->first = NULL;
	set->last = NULL;

	return set;
}

int set_size(Set set) {
	return set->size;
} 

void set_insert(Set set, Pointer value){
	bool inserted;
	Pointer old_value;
	set->root = node_insert(set, set->root, set->compare, value, &inserted, &old_value);
	
    //update τα set->last και set->first
	if(set->first == NULL)
		set->first = set->root;
	else if(set->first->prev != NULL)
		set->first = set->first->prev;

	if(set->last == NULL)
		set->last = set->root;
	else if(set->last->next != NULL)
		set->last = set->last->next;
	
	// Το size αλλάζει μόνο αν μπει νέος κόμβος. Στα updates κάνουμε destroy την παλιά τιμή
	if (inserted)
		set->size++;
	else if (set->destroy_value != NULL)
		set->destroy_value(old_value); 
}

bool set_remove(Set set, Pointer value){
	bool removed;
	Pointer old_value = NULL;

	//update τα set->last και set->first
	if(set->first != NULL && set->compare(set->first->value, value) == 0)
		set->first = set->first->next;
	if(set->last != NULL && set->compare(set->last->value, value) == 0)
		set->last = set->last->prev;
	
	set->root = node_remove(set, set->root, set->compare, value, &removed, &old_value);

	// Το size αλλάζει μόνο αν πραγματικά αφαιρεθεί ένας κόμβος
	if(removed){
		set->size--;

		if(set->destroy_value != NULL)
			set->destroy_value(old_value);
	}

	return removed;
}

void set_remove_node(Set set, SetNode node){
    Pointer old_value = NULL;
    bool removed;

	//update τα set->last και set->first
	if(set->first != NULL && set->first == node)
		set->first = set->first->next;
	if(set->last != NULL && set->last == node)
		set->last = set->last->prev;
	
	node = node_remove(set, node, set->compare, node->value, &removed, &old_value);

	// Το size αλλάζει μόνο αν πραγματικά αφαιρεθεί ένας κόμβος
	if(removed){
		set->size--;

		if (set->destroy_value != NULL)
			set->destroy_value(old_value);
	}
}

Pointer set_find(Set set, Pointer value){
	SetNode node = node_find_equal(set->root, set->compare, value);
	return (node == NULL) ? NULL : node->value;
}

DestroyFunc set_set_destroy_value(Set set, DestroyFunc destroy_value){
	DestroyFunc prev_destroy_value = set->destroy_value;
	set->destroy_value = destroy_value;
	return prev_destroy_value;
}

void set_destroy(Set set){
	node_destroy(set->root, set->destroy_value);
	free(set);
}

SetNode set_first(Set set){
	return set->first;
}

SetNode set_last(Set set){
	return set->last;
}

SetNode set_previous(Set set, SetNode node){
	assert(node != NULL);
	return node->prev;
}

SetNode set_next(Set set, SetNode node){
	assert(node != NULL);
	return node->next;
}

Pointer set_node_value(Set set, SetNode node){
	assert(node != NULL);
	return node->value;
}

SetNode set_find_node(Set set, Pointer value){
	return node_find_equal(set->root, set->compare, value);
}

// Συναρτήσεις που δεν υπάρχουν στο public interface αλλά χρησιμοποιούνται στα tests
// Ελέγχουν ότι το δέντρο είναι ένα σωστό AVL.

// LCOV_EXCL_START (δε μας ενδιαφέρει το coverage των test εντολών, και επιπλέον μόνο τα true branches εξετάζονται σε ένα επιτυχημένο test)

bool node_is_avl(SetNode node, CompareFunc compare) {
	if (node == NULL)
		return true;

	// Ελέγχουμε την ιδιότητα:
	// κάθε κόμβος είναι > αριστερό παιδί, > δεξιότερο κόμβο του αριστερού υποδέντρου, < δεξί παιδί, < αριστερότερο κόμβο του δεξιού υποδέντρου.
	// Είναι ισοδύναμη με την BST ιδιότητα (κάθε κόμβος είναι > αριστερό υποδέντρο και < δεξί υποδέντρο) αλλά ευκολότερο να ελεγθεί.
	bool res = true;
	if(node->left != NULL)
		res = res && compare(node->left->value, node->value) < 0 && compare(node_find_max(node->left)->value, node->value) < 0;
	if(node->right != NULL)
		res = res && compare(node->right->value, node->value) > 0 && compare(node_find_min(node->right)->value, node->value) > 0;

	// Το ύψος είναι σωστό
	res = res && node->height == 1 + int_max(node_height(node->left), node_height(node->right));

	// Ο κόμβος έχει την AVL ιδιότητα
	int balance = node_balance(node);
	res = res && balance >= -1 && balance <= 1;

	// Τα υποδέντρα είναι σωστά
	res = res &&
		node_is_avl(node->left, compare) &&
		node_is_avl(node->right, compare);

	return res;
}

bool set_is_proper(Set set) {
	return node_is_avl(set->root, set->compare);
}

// LCOV_EXCL_STOP

//Επαναληπτική υλοποίηση της set_visit
void set_visit(Set set, VisitFunc visit) {
	for(SetNode my_node = set->first; my_node != NULL; my_node = my_node->next)
		visit(set_node_value(set, my_node));
}