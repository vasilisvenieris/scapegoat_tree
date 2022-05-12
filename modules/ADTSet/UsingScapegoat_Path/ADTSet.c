///////////////////////////////////////////////////////////
//
// Υλοποίηση του ADT Set μέσω Scapegoat Tree
//
///////////////////////////////////////////////////////////

#include "ADTSet.h"

#define ALPHA 0.5375	// Αlways between (0.5, 1)
#define FACTOR 2 // Always greater than 1

// ceiling( Log_base_FACTOR_(N) )
// maximum number of rebalances from the root for N successive removals
// 
// The bigger FACTOR is, the rarer rebalanced during removals become
// FACTOR = (after what percentage of set_size deletion, rebalancing is triggered)
// 2 = 50%, 2.22 = 55%, 2.5 = 60%, 2.86 = 65%, 3.33 = 70%, 4 = 75%, 5 = 80%, 6.67 = 85%, 10 = 90%, 20 = 95%

// writing subtrees to .csv
// Κ = maximum size of subtrees
// check = write or not
int K = 10000;
bool check = false;

// Implementing ADT Set through Scapegoat tree, essentially a BST.
struct set {
	SetNode root;				// root, NULL if it is empty
	int size;					// size
	int threshold;				// trigger variable for insert/remove, bounded between size and FACTOR * size 
	int *problem;				// stores the number of rebalances after an insertion
	double recip_log;			// 1/ln(1/ALPHA)
	double h_sum;				// current harmonic sum: 1st order
	double sg_log;				// log1byALPHA(threshold)
	CompareFunc compare;		// ordering
	DestroyFunc destroy_value;	// destroys an item in the set
	SetNode first, last;			
};

struct set_node {
	SetNode left, right;		// children
	Pointer value;
	SetNode next, prev;			// next, previous
	
	int tot_length;
	int tree_size;
};

//////////////////////////////////////////////////////////
//////////Functions for scapegoat tree//////////////////
//////////////////////////////////////////////////////////

// tuple (node_edges, node_size) for each node
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

	return node->tot_length + node->tree_size  - 1;
}

// calculates the sum of all depths in the BST in Ο(n)
int set_length(Set set){
	return calc_length(set->root);
}

// number of rotations (for AVL!!)
int set_rotations(Set set){
	return 0;
}

// Υπολογίζει τον φυσικό/νεπέριο λογάριθμο ενός φυσικού αριθμού Ν
// χρησιμοποιώντας την αρμονική σειρά Η_Ν, τη σταθερά Euler-Mascheroni και έως 3 διορθωτικούς όρους Bernoulli.
// Προτείνεται, αντί της συχνής χρήσης log(N) του math.h, για πολύ μεγάλα δέντρα (πχ 1Μ+ κόμβοι).
// (i) Οσο περισσότεροι διορθωτικοί όροι χρησιμοποιούνται, τόσο μικρότερη η απόκλιση,
// (ii) Oσο μεγαλύτερος είναι ο φυσικός αριθμός Ν(δλδ το threshold), τόσο μικρότερη η απόκλιση.
//
double calc_thru_harmonics(Set set){
	// Με περισσότερους όρους αυξάνεται η ακρίβεια, αλλά και ο χρόνος εκτέλεσης
	// για τις ανάγκες του δέντρου, είναι επαρκής η ακρίβεια με έναν όρο
	int terms = 1; 
	int i = set->threshold;

	// 0.5772156649015328: σταθερά Euler-Mascheroni
    if(terms == 3)
        return set->h_sum - 0.5772156649015328 - (double)1/(2*i) + (double)1/(12*i*i) - (double)1/(120*i*i*i*i);
    if(terms == 2)
        return set->h_sum - 0.5772156649015328 - (double)1/(2*i) + (double)1/(12*i*i);
    if(terms == 1)
        return set->h_sum - 0.5772156649015328 - (double)1/(2*i);

    return set->h_sum - 0.5772156649015328;
}

// Υπολογίζει τον λογάριθμο με βάση 1/ALPHA του set->threshold
// Για πολύ μικρό threshold, χρησιμοποιείται η log του math.h, ώστε να επιτυγχάνεται επαρκής ακρίβεια
static double log1byALPHA(Set set){
	if(set->threshold <= 5)
		return set->recip_log * log(set->threshold);

	return set->recip_log * calc_thru_harmonics(set);
}

int sizeofTree(SetNode node){
	return node == NULL ? 0 : 1 + sizeofTree(node->left) + sizeofTree(node->right);
}

// Τοποθετεί τους κόμβους του δέντρου σε πίνακα
int node_tree_to_sortedArr(SetNode node, SetNode arr[], int i){
    if(node == NULL)
        return i;

    i = node_tree_to_sortedArr(node->left, arr, i);
    arr[i] = node;
    i++;

    return node_tree_to_sortedArr(node->right, arr, i);
}

// Δημιουργεί ένα ισορροπημένο δυαδικό δέντρο αναζήτησης από Ν ταξινομημένα στοιχεία σε Ο(Ν)
SetNode node_sortedArr_to_tree(SetNode *arr, int start, int end){
    if(end == 0)
        return NULL;
    int mid = end/2;

    arr[start + mid]->left = node_sortedArr_to_tree(arr, start, mid);    
    arr[start + mid]->right = node_sortedArr_to_tree(arr, start + mid + 1, end - mid - 1);

    return arr[start + mid];
}

// Ξανακτίζει το δέντρο γύρω απ' το node-scapegoat
void node_rebuild_tree(Set set, SetNode node, SetNode parent, int asize){
	if(node == NULL)
		return;
	
	// Για μικρά δέντρα μεγέθους έως 10k, δεσμεύεται πίνακας από το stack
	// Αν το δέντρο είναι πολύ μεγάλο (συνήθως για πάνω από 10k, τα rebalances προκαλούνται από την remove),
	// ο χώρος δεσμεύεται από το heap
	// Ετσι αποφεύγεται η συνεχής χρήση malloc και επιταχύνονται οι λειτουργίες
	SetNode arr_10k[10000];
	SetNode *arr = (asize > 10000) ? malloc(asize * sizeof(*arr)) : arr_10k;

    node_tree_to_sortedArr(node, arr, 0);
    if(parent == NULL)
        set->root = node_sortedArr_to_tree(arr, 0, asize);
    else if(parent->right == node)
		parent->right = node_sortedArr_to_tree(arr, 0, asize);
    else
		parent->left = node_sortedArr_to_tree(arr, 0, asize);
	
	// αν έχει δεσμευτεί χώρος από το heap, απελευθερώνεται
	if(asize > 10000)
		free(arr);
}

///////////////////////////////////////////////////////////////////
////////////Tέλος συναρτήσεων scapegoat tree///////////////////////
///////////////////////////////////////////////////////////////////

// Παρατηρήσεις για τις node_* συναρτήσεις
// - είναι βοηθητικές (κρυφές από το χρήστη) και υλοποιούν διάφορες λειτουργίες πάνω σε κόμβους του BST.
// - είναι αναδρομικές, η αναδρομή είναι γενικά πολύ βοηθητική στα δέντρα.
// - όσες συναρτήσεις _τροποποιούν_ το δέντρο, ουσιαστικά ενεργούν στο _υποδέντρο_ με ρίζα τον κόμβο node, και επιστρέφουν τη νέα
//   ρίζα του υποδέντρου μετά την τροποποίηση. Η νέα ρίζα χρησιμοποιείται από την προηγούμενη αναδρομική κλήση.
//
// Οι set_* συναρτήσεις (πιο μετά στο αρχείο), υλοποιούν τις συναρτήσεις του ADTSet, και είναι απλές, καλώντας τις αντίστοιχες node_*.


// Δημιουργεί και επιστρέφει έναν κόμβο με τιμή value (χωρίς παιδιά)

static SetNode node_create(Pointer value) {
	SetNode node = malloc(sizeof(*node));
	node->left = NULL;
	node->right = NULL;
	node->prev = NULL;
	node->next = NULL;

    node->value = value;
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
	else													// value > node->value, ο κόμβος που ψάχνουμε είναι στο δεξιό υποδέντρο
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

static SetNode node_insert(Set set, SetNode node, CompareFunc compare, Pointer value, bool* inserted, Pointer* old_value, int *depth, SetNode *path) {
	// Αν το υποδέντρο είναι κενό, δημιουργούμε νέο κόμβο ο οποίος γίνεται ρίζα του υποδέντρου
	if (node == NULL) {
		*inserted = true;			// κάναμε προσθήκη
		path[*depth] = node_create(value);
        return path[*depth]; // ο κόμβος που εισάγεται στο BST δέντρο αποθηκεύεται στο τέλος του μονοπατιού
	}

	// Ο κόμβος αποθηκεύεται στον πίνακα μονοπατιού
	path[*depth] = node;

	//Το βάθος του δυνητικού κόμβου αυξάνεται κατά 1
	(*depth)++;

	// Το που θα γίνει η προσθήκη εξαρτάται από τη διάταξη της τιμής
	// value σε σχέση με την τιμή του τρέχοντος κόμβου (node->value)
	//
	int compare_res = compare(value, node->value);
	if (compare_res == 0) {
		// βρήκαμε ισοδύναμη τιμή, κάνουμε update
		*inserted = false;
		*old_value = node->value;
		node->value = value;
		//δεν τοποθετήθηκε, οπότε επιστρέφει άκυρο βάθος
		*depth = -1; 
	} else if (compare_res < 0) {
		// value < node->value, συνεχίζουμε αριστερά.
		node->left = node_insert(set, node->left, compare, value, inserted, old_value, depth, path);
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
		node->right = node_insert(set, node->right, compare, value, inserted, old_value, depth, path);
		if(node->right->next == NULL && node->right->prev == NULL){
			if(node->next != NULL){
				node->next->prev = node->right;
				node->right->next = node->next;
			}
			node->next = node->right;
			node->right->prev = node;
		}
	}

	return node;	// η ρίζα του υποδέντρου δεν αλλάζει
}

// Αφαιρεί και αποθηκεύει στο min_node τον μικρότερο κόμβο του υποδέντρου με ρίζα node.
// Επιστρέφει τη νέα ρίζα του υποδέντρου.

static SetNode node_remove_min(SetNode node, SetNode* min_node) {
	if (node->left == NULL) {
		// Δεν έχουμε αριστερό υποδέντρο, οπότε ο μικρότερος είναι ο ίδιος ο node
		*min_node = node;
		return node->right;		// νέα ρίζα είναι το δεξιό παιδί

	} else {
		// Εχουμε αριστερό υποδέντρο, οπότε η μικρότερη τιμή είναι εκεί. Συνεχίζουμε αναδρομικά
		// και ενημερώνουμε το node->left με τη νέα ρίζα του υποδέντρου.
		node->left = node_remove_min(node->left, min_node);
		return node;			// η ρίζα δεν μεταβάλλεται
	}
}

// Διαγράφει το κόμβο με τιμή ισοδύναμη της value, αν υπάρχει. Επιστρέφει τη νέα ρίζα του
// υποδέντρου, και θέτει το *removed σε true αν έγινε πραγματικά διαγραφή.

static SetNode node_remove(SetNode node, CompareFunc compare, Pointer value, bool* removed, Pointer* old_value) {
	if (node == NULL) {
		*removed = false;		// κενό υποδέντρο, δεν υπάρχει η τιμή
		return NULL;
	}

	int compare_res = compare(value, node->value);
	if (compare_res == 0) {
		// Βρέθηκε ισοδύναμη τιμή στον node, οπότε τον διαγράφουμε. Το πώς θα γίνει αυτό εξαρτάται από το αν έχει παιδιά.
		*removed = true;
		*old_value = node->value;

		//αποσυνδέουμε το node από τους υπόλοιπους κόμβους
		if(node->next != NULL)
			node->next->prev = node->prev;
		if(node->prev != NULL)
			node->prev->next = node->next;

		if (node->left == NULL) {
			// Δεν υπάρχει αριστερό υποδέντρο, οπότε διαγράφεται απλά ο κόμβος και νέα ρίζα μπαίνει το δεξί παιδί
			SetNode right = node->right;	// αποθήκευση πριν το free!
			free(node);
			return right;

		} else if (node->right == NULL) {
			// Δεν υπάρχει δεξί υποδέντρο, οπότε διαγράφεται απλά ο κόμβος και νέα ρίζα μπαίνει το αριστερό παιδί
			SetNode left = node->left;		// αποθήκευση πριν το free!
			free(node);
			return left;

		} else {
			// Υπάρχουν και τα δύο παιδιά. Αντικαθιστούμε την τιμή του node με την μικρότερη του δεξιού υποδέντρου, η οποία
			// αφαιρείται. Η συνάρτηση node_remove_min κάνει ακριβώς αυτή τη δουλειά.

			SetNode min_right;
			node->right = node_remove_min(node->right, &min_right);

			// Σύνδεση του min_right στη θέση του node
			min_right->left = node->left;
			min_right->right = node->right;

			free(node);
			return min_right;
		}
	}

	// compare_res != 0, συνεχίζουμε στο αριστερό ή δεξί υποδέντρο, η ρίζα δεν αλλάζει.
	if (compare_res < 0)
		node->left  = node_remove(node->left,  compare, value, removed, old_value);
	else
		node->right = node_remove(node->right, compare, value, removed, old_value);

	return node;
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

//// Συναρτήσεις του ADT Set. Γενικά πολύ απλές, αφού καλούν τις αντίστοιχες node_*

Set set_create(CompareFunc compare, DestroyFunc destroy_value, Vector values) {
	// δημιουργούμε το set
	Set set = malloc(sizeof(*set));
	set->root = NULL;			// κενό δέντρο
	set->size = 0;
	set->threshold = 0;			// η μεταβλητή που ενεργοποιεί την node_rebuild στο set_insert/remove
	if(check)
		set->problem = calloc(K, sizeof(int));

	// η αρμονική σειρά Η_(set->size), πχ Η_5 = 1 + 1/2 + 1/3 + 1/4 + 1/5
	// εναλλακτικός τρόπος υπολογισμού του lnN, με Ν φυσικό αριθμό
	set->h_sum = 0;	

	//ο φυσικός λογάριθμος είναι υπολογιστικά βαριά πράξη, οπότε προϋπολογίζεται και αποθηκεύεται στο set
	set->recip_log = (double)1/log(1/ALPHA); 

	// Αντίστοιχα, με αυτή την μεταβλητή, αποφεύγεται η πολλαπλή κλήση λογαρίθμου
	set->sg_log = 0;	

	set->compare = compare;
	set->destroy_value = destroy_value;

	set->first = NULL;
	set->last = NULL;

	// Αν εισήχθη vector με (ταξινομημένα) στοιχεία, αρχικοποιείται το δέντρο
	if(values != NULL){
		set->size = vector_size(values);
		set->threshold = set->size;
		
		// Δημιουργία πίνακα για τα ταξινομημένα SetNodes
		SetNode *arr = malloc(set->size * sizeof(*arr));

		// Αρχικοποίηση της αρμονικής και των next/prev δεικτών των SetNodes
		for(int i = 0; i < set->size; i++){
			set->h_sum += (double)1/(i + 1);

			arr[i] = node_create(vector_get_at(values, i));

			if(i > 0){
				arr[i]->prev = arr[i - 1];
				arr[i - 1]->next = arr[i];
			}	
		}
		
		// Αρχικοποίηση του log1byALPHA(threshold);
		set->sg_log = log1byALPHA(set);

		// Αρχικοποίηση των first, last σε Ο(1)
		set->first = arr[0];
		set->last = arr[set->size - 1];

		// Aρχικοποίηση των left/right δεικτών των SetNodes
		set->root = node_sortedArr_to_tree(arr, 0, set->size);

		free(arr);
	}
		
	return set;
}

int set_size(Set set) {
	return set->size;
}

void set_insert(Set set, Pointer value) {
	bool inserted;
	Pointer old_value;
	int depth = 0;

	// υπολογισμός μέγιστου μεγέθους δυνητικού μονοπατιού
	const int arr_size = (set->size == 0) ? 0 : 2*ceil(set->sg_log);
	SetNode path[arr_size];
	
	set->root = node_insert(set, set->root, set->compare, value, &inserted, &old_value, &depth, path);

    //update τα set->last και set->first
	if(set->first == NULL)
		set->first = set->root;
	else if(set->first->prev != NULL)
		set->first = set->first->prev;

	if(set->last == NULL)
		set->last = set->root;
	else if(set->last->next != NULL)
		set->last = set->last->next;

	// Το size αλλάζει μόνο αν μπει νέος κόμβος. Στα updates καταστρέφουμε την παλιά τιμή
	if (inserted){
		set->size++;
		set->h_sum += (double)1/set->size;
		set->threshold++;
		set->sg_log = log1byALPHA(set);
	}
	else if (set->destroy_value != NULL)
		set->destroy_value(old_value);
	
	// Aν το βάθος του νεοτοποθετηθέντος στοιχείου ξεπεράσει το log1/ALPHA(threshold),
	// αναζητείται το scapegoat node και επανασυστήνεται το υποδέντρο του node
	if(inserted && (double)depth > set->sg_log){
		int current_depth = depth;

		// Ορίζουμε τον τρέχοντα κόμβο, τον πατέρα και τον αδελφό
        SetNode current_node = (current_depth != 0) ? path[--current_depth] : NULL;
		SetNode parent_node = (current_depth != 0) ? path[current_depth - 1] : NULL; 
		SetNode sibling_node = NULL;
		if(parent_node != NULL)
			sibling_node = (parent_node->left == current_node) ? parent_node->right : parent_node->left;

		// Βρίσκουμε το μέγεθος των δέντρων του τρέχοντα και του αδελφού κόμβου
		int cur_size = sizeofTree(current_node);
		int sibling_size = sizeofTree(sibling_node);
		
		// Ο βρόχος τρέχει μέχρι να βρεθεί το scapegoat node
        while((double)cur_size <= (double)ALPHA*(1 + cur_size + sibling_size)){
			// Επικαιροποιούμε τους κόμβους
			current_depth--;
			current_node = parent_node;
			parent_node = (current_depth != 0) ? path[current_depth - 1] : NULL; 

			if(parent_node != NULL)
				sibling_node = (parent_node->left == current_node) ? parent_node->right : parent_node->left;
			else
				sibling_node = NULL;

			// επικαιροποιούμε τα μεγέθη των δέντρων
			cur_size = 1 + cur_size + sibling_size;
			sibling_size = sizeofTree(sibling_node);
		}

		// Το scapegoat node είναι ο πατέρας του τελευταίου current_node, οπότε επικαιροποιούμε άλλη μια φορά
		parent_node = (current_depth != 0) ? path[--current_depth] : NULL;
		if(parent_node != NULL)
			sibling_node = (parent_node->left == current_node) ? parent_node->right : parent_node->left;
		else
			sibling_node = NULL;

		// επικαιροποιούμε τα μεγέθη των δέντρων μια τελευταία φορά
		cur_size = (parent_node != NULL) ? 1 + cur_size + sizeofTree(sibling_node) : set->size;
		current_node = parent_node;
		parent_node = (current_depth != 0) ? path[current_depth - 1] : NULL;
        
		if(check && cur_size < K)
			set->problem[cur_size]++;
		
		// επανακτίζεται το δέντρο του current_node, o γονέας του μπαίνει στη συνάρτηση βοηθητικά
        node_rebuild_tree(set, current_node, parent_node, cur_size);
    }
}

bool set_remove(Set set, Pointer value) {
	bool removed;
	Pointer old_value = NULL;

	// update τα set->last και set->first
	if(set->first != NULL && set->compare(set->first->value, value) == 0)
		set->first = set->first->next;
	if(set->last != NULL && set->compare(set->last->value, value) == 0)
		set->last = set->last->prev;
	
	set->root = node_remove(set->root, set->compare, value, &removed, &old_value);

	// Το size αλλάζει μόνο αν πραγματικά αφαιρεθεί ένας κόμβος
	if(removed){
		set->h_sum -= (double)1/set->size;
		set->size--;

		if(FACTOR * (double)set->size <= (double)set->threshold){
			node_rebuild_tree(set, set->root, NULL, set->size);
			set->threshold = set->size;
		}

		if(set->destroy_value != NULL)
			set->destroy_value(old_value);
	}

	return removed;
}

void set_remove_node(Set set, SetNode node){
    
}

Pointer set_find(Set set, Pointer value) {
	SetNode node = node_find_equal(set->root, set->compare, value);
	return node == NULL ? NULL : node->value;
}

DestroyFunc set_set_destroy_value(Set set, DestroyFunc destroy_value) {
	DestroyFunc old = set->destroy_value;
	set->destroy_value = destroy_value;
	return old;
}

void set_destroy(Set set) {
	node_destroy(set->root, set->destroy_value);

	if(check){
		FILE *fp1 = fopen("ADTSet_SGTree.csv","a+");
		for(int i = 0; i < K; i++)
			if(set->problem[i] != 0)
				fprintf(fp1, "%d, %d\n", i, set->problem[i]);

		fclose(fp1);
		free(set->problem);
	}
	
	free(set);
}

SetNode set_first(Set set) {
	return set->first;
}

SetNode set_last(Set set) {
	return set->last;
}

SetNode set_previous(Set set, SetNode node) {
	return node->prev;
}

SetNode set_next(Set set, SetNode node) {
	return node->next;
}

Pointer set_node_value(Set set, SetNode node) {
	return node->value;
}

SetNode set_find_node(Set set, Pointer value) {
	return node_find_equal(set->root, set->compare, value);
}

// Συναρτήσεις που δεν υπάρχουν στο public interface αλλά χρησιμοποιούνται στα tests.
// Ελέγχουν ότι το δέντρο είναι ένα σωστό BST.

// LCOV_EXCL_START (δε μας ενδιαφέρει το coverage των test εντολών, και επιπλέον μόνο τα true branches εκτελούνται σε ένα επιτυχημένο test)

static bool node_is_bst(SetNode node, CompareFunc compare) {
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

	return res &&
		node_is_bst(node->left, compare) &&
		node_is_bst(node->right, compare);
}

bool set_is_proper(Set node) {
	return node_is_bst(node->root, node->compare);
}

void set_visit(Set set, VisitFunc visit) {
	for(SetNode my_node = set->first; my_node != NULL; my_node = my_node->next)
		visit(set_node_value(set, my_node));
}




// LCOV_EXCL_STOP