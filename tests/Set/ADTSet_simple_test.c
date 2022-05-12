//////////////////////////////////////////////////////////////////
//
// Unit tests για τον ADT Set.
// Οποιαδήποτε υλοποίηση οφείλει να περνάει όλα τα tests.
//
//////////////////////////////////////////////////////////////////

#include "acutest.h"			// Απλή βιβλιοθήκη για unit testing
#include "ADTSet.h"

clock_t start_t, end_t;
double total_t;
int iters = 1000;
int N = 100000;

// Η συνάρτηση αυτή δεν υπάρχει στο public interface του Set αλλά χρησιμεύει
// στα tests, για να ελέγχει αν το set είναι σωστό μετά από κάθε λειτουργία.
// Ελέγχει αν το set είναι σωστό ή με σφάλματα ταξινόμησης (BEWARE: !!ΧΡΟΝΟΒΟΡΟ!!)
bool set_is_proper(Set set);
bool activate_set_check = false;

//
//  Βοηθητικές συναρτήσεις 
//

// Επιστρέφει έναν ακέραιο σε νέα μνήμη με τιμή value
int* create_int(int value) {
	int* p = malloc(sizeof(int));
	*p = value;
	return p;
}

// Δημιουργούμε μια ειδική compare συνάρτηση
int compare_ints(Pointer a, Pointer b) {
	return *(int*)a - *(int*)b;
}

// Έλεγχος της insert σε λιγότερο χώρο
void insert_and_test(Set set, Pointer value) {
	set_insert(set, value);
	if(activate_set_check)
		TEST_ASSERT(set_is_proper(set));
	TEST_ASSERT(set_find(set, value) == value);
}

// Βοηθητική συνάρτηση για το ανακάτεμα του πίνακα τιμών
void shuffle(int** array, int n) {
    srand(time(NULL));
	for (int i = 0; i < n; i++) {
		int j = i + rand() / (RAND_MAX / (n - i) + 1);
		int* t = array[j];
		array[j] = array[i];
		array[i] = t;
	}
}

//
//  Test συναρτήσεις 
//

void test_create() {
	total_t = 0;

	for(int i = 0; i < iters; i++){
		start_t = clock();
		// Δημιουργούμε ένα κενό set (χωρίς συνάρτηση αποδέσμευσης)
		Set set = set_create(compare_ints, NULL, NULL);
		set_set_destroy_value(set, NULL);

		TEST_ASSERT(set != NULL);
		TEST_ASSERT(set_size(set) == 0);

		end_t = clock();
		set_destroy(set);
		
		total_t += (end_t - start_t)/(double)CLOCKS_PER_SEC;
	}
	printf("Avg time for set_create: %f",total_t/iters);
}

void test_insert() {
	total_t = 0;

    srand(time(NULL));

	int** value_array = malloc(N * sizeof(*value_array));

	for(int i = 0; i < iters; i++){
		for(int i = 0; i < N; i++)
			value_array[i] = create_int(i);

		Set set = set_create(compare_ints, free, NULL);

        shuffle(value_array, N);

		start_t = clock();
		// Δοκιμάζουμε την insert με νέες τιμές κάθε φορά και με αυτόματο free
		for(int i = 0; i < N; i++) {
			insert_and_test(set, value_array[i]);
			TEST_ASSERT(set_size(set) == (i + 1));
		}
        end_t = clock();
		
		// Δοκιμάζουμε την insert με τιμές που υπάρχουν ήδη στο Set
		// και ελέγχουμε ότι δεν ενημερώθηκε το size (καθώς δεν προστέθηκε νέος κόμβος)
		int* new_value = create_int(0);
		insert_and_test(set, new_value);
		TEST_ASSERT(set_size(set) == N);
		set_destroy(set);
		
		// Δοκιμάζουμε την insert χωρίς αυτόματο free
		Set set2 = set_create(compare_ints, NULL, NULL);
		int local_value1 = 0, local_value2 = 1, local_value3 = 1;

		insert_and_test(set2, &local_value1);
		insert_and_test(set2, &local_value2);
		insert_and_test(set2, &local_value3);		// ισοδύναμη τιμή => replace

		
		set_destroy(set2);
		total_t += (end_t - start_t)/(double)CLOCKS_PER_SEC;
	}

    free(value_array);

	printf("Avg time for set_insert: %f",total_t/iters);
}

// Έλεγχος της remove σε λιγότερο χώρο
void remove_and_test(Set set, Pointer value) {
	TEST_ASSERT(set_remove(set, value));
	if(activate_set_check)
		TEST_ASSERT(set_is_proper(set));
	TEST_ASSERT(set_find(set, value) == NULL);
}

void test_remove() {
	total_t = 0;

    srand(time(NULL));

    int** value_array = malloc(N * sizeof(*value_array));
    for(int i = 0; i < N; i++)
		value_array[i] = create_int(i);


	for(int i = 0; i < iters; i++){
		Set set = set_create(compare_ints, NULL, NULL);
		
		// Ανακατεύουμε το value_array ώστε να υπάρχει ομοιόμορφη εισαγωγή τιμών
		// Πχ εάν εισάγουμε δείκτες με αύξουσα σειρά τιμών, τότε εάν το Set υλοποιείται με BST,
		// οι κόμβοι θα προστίθενται μόνο δεξιά της ρίζας, άρα και η set_remove δεν θα δοκιμάζεται πλήρως
		
        shuffle(value_array, N);
        
		for(int i = 0; i < N; i++)
			set_insert(set, value_array[i]);

		// Δοκιμάζουμε, πριν διαγράψουμε κανονικά τους κόμβους, ότι η map_remove διαχειρίζεται 
		// σωστά ένα κλειδί που δεν υπάρχει στο Map και γυρνάει NULL 
		int not_exists = 123456789;
		TEST_ASSERT(!set_remove(set, &not_exists));
		
        start_t = clock();
		// Διαγράφουμε όλους τους κόμβους
		for(int i = 0; i < N; i++) {
			remove_and_test(set, value_array[i]);
			TEST_ASSERT(set_size(set) == (N - i - 1));
		}
        end_t = clock();

		set_destroy(set);

		// Δοκιμάζουμε τη remove χωρίς αυτόματο free
		Set set2 = set_create(compare_ints, NULL, NULL);
		int local_value1 = 0;
		insert_and_test(set2, &local_value1);
		TEST_ASSERT(set_remove(set2, &local_value1));
		TEST_ASSERT(set_is_proper(set2));
		TEST_ASSERT(set_size(set2) == 0);

		set_destroy(set2);

		total_t += (end_t - start_t)/(double)CLOCKS_PER_SEC;
	}
	printf("Avg time for set_remove: %f",total_t/iters);

    for(int i = 0; i < N; i++)
        free(value_array[i]);
    free(value_array);
}

// void test_remove_node() {
// 	total_t = 0;
//     srand(time(NULL));

//     int** value_array = malloc(N * sizeof(*value_array));
//     for(int i = 0; i < N; i++)
// 		value_array[i] = create_int(i);

// 	for(int i = 0; i < iters; i++){
// 		Set set = set_create(compare_ints, NULL, NULL);
		
// 		// Ανακατεύουμε το value_array ώστε να υπάρχει ομοιόμορφη εισαγωγή τιμών
// 		// Πχ εάν εισάγουμε δείκτες με αύξουσα σειρά τιμών, τότε εάν το Set υλοποιείται με BST,
// 		// οι κόμβοι θα προστίθενται μόνο δεξιά της ρίζας, άρα και η set_remove δεν θα δοκιμάζεται πλήρως
		
//         shuffle(value_array, N);
        
// 		for(int i = 0; i < N; i++){
//             set_insert(set, value_array[i]);
//             printf("value inserted: %d\n", *(int*)value_array[i]);
//         }
			

//         SetNode *setnode_array = malloc(N * sizeof(*setnode_array));
//         for(int i = 0; i < N; i++){
//             setnode_array[i] = set_find_node(set, value_array[i]);
//         }
		
//         start_t = clock();
// 		// Διαγράφουμε όλους τους κόμβους
// 		for(int i = 0; i < N; i++) {
//             TEST_ASSERT(*(int*)set_node_value(NULL, setnode_array[i]) == *(int*)value_array[i]);
//             set_remove_node(set, setnode_array[i]);
//             TEST_ASSERT(set_is_proper(set));
//             //TEST_ASSERT(set_size(set) == (N - i - 1));
//             printf("complete!!\n");
// 		}
//         end_t = clock();

// 		set_destroy(set);
//         free(setnode_array);

// 		total_t += (end_t - start_t)/(double)CLOCKS_PER_SEC;
// 	}
// 	printf("Avg time for set_r_node: %f",total_t/iters);

//     for(int i = 0; i < N; i++)
//         free(value_array[i]);
//     free(value_array);
// }


void test_find() {
	total_t = 0;

	Set set = set_create(compare_ints, free, NULL);
	int** value_array = malloc(N * sizeof(*value_array));

	for(int i = 0; i < N; i++)
		value_array[i] = create_int(i);

	// Παρόμοια με την set_remove, εάν το δέντρο δεν είναι σωστά ισορροπημένο, οι συναρτήσεις εύρεσης
	// στοιχείων δεν θα ελέγχονται πλήρως
	shuffle(value_array, N);

	for(int i = 0; i < N; i++) {
		set_insert(set, value_array[i]);
		SetNode found_node 	= set_find_node(set, value_array[i]);
		Pointer found_value = set_node_value(set, found_node);
		TEST_ASSERT(found_node != NULL);
		TEST_ASSERT(found_value == value_array[i]);
	}

	start_t = clock();
	for(int i = 0; i < iters; i++){
		
		// Αναζήτηση στοιχείου που δεν υπάρχει στο set
		int not_exists = 123456789;
		TEST_ASSERT(set_find_node(set, &not_exists) == NULL);
		TEST_ASSERT(set_find(set, &not_exists) == NULL);
	
		// Αναζήτηση μέγιστων/ελάχιστων στοιχείων
		// Συγκρίνουμε τις τιμές των δεικτών και όχι τους ίδιους τους δείκτες, καθώς
		// δεν γνωρίζουμε την θέση τους μετά απο το ανακάτεμα του πίνακα, αλλά γνωρίζουμε
		// ποιές τιμές υπάρχουν στο Set. Στη συγκεκριμένη περίπτωση, γνωρίζουμε ότι Set = {0, 1, ..., N-1}
		SetNode first_node = set_first(set);
		Pointer first_value = set_node_value(set, first_node);
		TEST_ASSERT((*(int *)first_value) == 0);
	
		SetNode next = set_next(set, first_node);
		Pointer next_value = set_node_value(set, next);
		TEST_ASSERT((*(int *)next_value) == 1);
	
		SetNode last_node = set_last(set);
		Pointer last_node_value = set_node_value(set, last_node);
		TEST_ASSERT((*(int *)last_node_value) == N-1);
	
		SetNode prev = set_previous(set, last_node);
		Pointer prev_value = set_node_value(set, prev);
		TEST_ASSERT((*(int *)prev_value) == N-2);
		// Ελέγχουμε και ότι βρίσκουμε σωστά τις τιμές από ενδιάμεσους κόμβους
		SetNode middle_node = set_find_node(set, value_array[N/2]);
		SetNode middle_node_prev = set_previous(set, middle_node);
	
		Pointer middle_node_value = set_node_value(set, middle_node);
		Pointer middle_node_value_prev = set_node_value(set, middle_node_prev);
	
		TEST_ASSERT(*(int *)middle_node_value == *(int *)middle_node_value_prev + 1);
	}
	end_t = clock();
	total_t += (end_t - start_t)/(double)CLOCKS_PER_SEC;
	set_destroy(set);
	free(value_array);
	
	printf("Avg time for set_find:   %f",total_t/iters);
}

void test_iterate() {
	total_t = 0;

	for(int i = 0;i < iters; i++){
		Set set = set_create(compare_ints, free, NULL);
		int** value_array = malloc(N * sizeof(*value_array));

		for (int i = 0; i < N; i++)
			value_array[i] = create_int(i);

		// εισαγωγή τιμών σε τυχαία σειρά
		shuffle(value_array, N);

		for(int i = 0; i < N; i++)
			set_insert(set, value_array[i]);

		start_t = clock();
		// iterate, τα στοιχεία πρέπει να τα βρούμε στη σειρά διάταξης
		int i = 0;
		for(SetNode node = set_first(set); node != NULL; node = set_next(set, node))
			TEST_ASSERT(*(int*)set_node_value(set, node) == i++);
        i = N - 1;
        for(SetNode node = set_last(set); node != NULL; node = set_previous(set, node))
            TEST_ASSERT(*(int*)set_node_value(set, node) == i--);
        
		// Κάποια removes
		i = N - 1;
		set_remove(set, &i);
		i = 40;
		set_remove(set, &i);

		// iterate, αντίστροφη σειρά, τα στοιχεία πρέπει να τα βρούμε στη σειρά διάταξης
		i = N - 2;
		for(SetNode node = set_last(set); node != NULL; node = set_previous(set, node)) {
			if(i == 40)
				i--;					// το 40 το έχουμε αφαιρέσει
			TEST_ASSERT(*(int*)set_node_value(set, node) == i--);
		}

		end_t = clock();
		set_destroy(set);
		free(value_array);
		
		total_t += (end_t - start_t)/(double)CLOCKS_PER_SEC;
	}
	printf("Avg time for set_iterate:%f",total_t/iters);
}

int last_visited;
void visit(Pointer value) {
	TEST_ASSERT(*(int*)value == last_visited+1);
	last_visited = *(int*)value;
    //printf("%d->",last_visited);
}

void test_visit() {
	// Δοκιμή της set_visit σε ένα set με N στοιχεία
	Set set = set_create(compare_ints, free, NULL);
	//μέτρηση χρόνου εκτέλεσης της set_visit
	total_t = 0;

    int** value_array = malloc(N * sizeof(*value_array));
	for(int i = 0; i < N; i++)
		value_array[i] = create_int(i);

	shuffle(value_array, N);

	for(int i = 0; i < N; i++)
		set_insert(set, value_array[i]);

	for(int i = 0; i < iters; i++){
		start_t = clock();
		// Η last_visited περιέχει το τελευταίο στοιχείο που έχουμε επισκεφθεί
		last_visited = -1;
		set_visit(set, visit);
		end_t = clock();

		total_t += (end_t - start_t)/(double)CLOCKS_PER_SEC;
	}
   	printf("Avg time for set_visit:  %f", total_t/iters  );

	TEST_ASSERT(last_visited == N-1);
	set_destroy(set);
    free(value_array);
}

////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////

void test_benchmark_insert(){
	total_t = 0;

    srand(time(NULL));

	int** value_array = malloc(N * sizeof(*value_array));
	for(int i = 0; i < N; i++)
		value_array[i] = create_int(i);

	shuffle(value_array, N);

	for(int i = 0; i < iters; i++){
		Set set = set_create(compare_ints, NULL, NULL);

        

		start_t = clock();
		// Δοκιμάζουμε την insert με νέες τιμές κάθε φορά και με αυτόματο free
		for(int i = 0; i < N; i++) {
			insert_and_test(set, value_array[i]);
			TEST_ASSERT(set_size(set) == (i + 1));
		}
        end_t = clock();
		
		set_destroy(set);
		total_t += (end_t - start_t)/(double)CLOCKS_PER_SEC;
	}

	for(int i = 0; i < N; i++)
		free(value_array[i]);
    free(value_array);

	printf("Avg time for set_insert: %f",total_t/iters);
}

void test_benchmark_remove(){
	
}

void test_benchmark_find(){
	
}

void test_multi(){
	//μέτρηση χρόνου εκτέλεσης της set_visit
	srand(time(NULL));
	total_t = 0;

    int** value_array = malloc(N * sizeof(*value_array));
	int a = 11;
	bool* removed_array = calloc(N, sizeof(*removed_array));
	bool iterate_check = false;
	
	for(int i = 0; i < N; i++)
		value_array[i] = create_int(i);

	shuffle(value_array, N);
	for(int i = 0; i < iters; i++){
		for(int j = 0; j < N; j++)
			removed_array[j] = false;

		Set set = set_create(compare_ints, NULL, NULL);
		start_t = clock();

		int prev;

		for(int i = 0; i < N; i++){
			insert_and_test(set, value_array[i]);

			if(iterate_check){
				prev = -1;
				for(SetNode node = set_first(set); node != NULL; node = set_next(set, node)){
					TEST_ASSERT(prev < *(int*)set_node_value(set, node));
					prev = *(int*)set_node_value(set, node);
				}

				prev = N + 1;
				for(SetNode node = set_last(set); node != NULL; node = set_previous(set, node)){
					TEST_ASSERT(prev > *(int*)set_node_value(set, node));
					prev = *(int*)set_node_value(set, node);
				}
			}
		}
			
		for(int i = 0; i < N; i++)
			if(N % a == 0){
				removed_array[i] = true;
				remove_and_test(set, value_array[i]);
			}

		//Λογική XNOR
		//Αν η find επιστρέφει στοιχείο, τότε αυτό ΔΕΝ έχει αφαιρεθεί απ' το set, οπότε η removed_arr περιέχει false και τούμπαλιν
		//Ελέγχεται αν τα στοιχεία αφαιρέθηκαν σωστά
		for(int i = 0; i < N; i++)
			TEST_ASSERT(
						(set_find(set, value_array[i]) == NULL || !removed_array[i]) &&
						(set_find(set, value_array[i]) != NULL ||  removed_array[i]) );

		//Ελέγχεται η αφαίρεση τυχαίων στοιχείων του set και τυχόν επαναφαίρεση ήδη αφαιρεμένων στοιχείων
		for(int i = 0; i < N/3; i++){
			int elem = rand() % N;
			bool exists = set_remove(set, value_array[elem]);
			TEST_ASSERT(exists == !removed_array[elem]);
			if(exists == true)
				removed_array[elem] = exists;
		}

		if(iterate_check){
			prev = -1;

			// iterate, τα στοιχεία πρέπει να τα βρούμε στη σειρά διάταξης
			for(SetNode node = set_first(set); node != NULL; node = set_next(set, node)){
				TEST_ASSERT(prev < *(int*)set_node_value(set, node));
				prev = *(int*)set_node_value(set, node);
			}

			prev = N + 1;
			for(SetNode node = set_last(set); node != NULL; node = set_previous(set, node)){
				TEST_ASSERT(prev > *(int*)set_node_value(set, node));
				prev = *(int*)set_node_value(set, node);
			}
		}
		
            

		end_t = clock();
		total_t += (end_t - start_t)/(double)CLOCKS_PER_SEC;
		set_destroy(set);
	}
   	printf("Avg time for set_multi:  %f", total_t/iters  );
	
	for(int i = 0; i < N; i++)
		free(value_array[i]);

    free(value_array);
	free(removed_array);
}


// Λίστα με όλα τα tests προς εκτέλεση
TEST_LIST = {
	//{ "set_create", 	    test_create 	},
	//{ "set_insert", 	    test_insert 	},
	//{ "set_remove", 	    test_remove 	},
    //{ "set_remove_node", 	test_remove_node 	},
	//{ "set_find", 		    test_find 		},
	//{ "set_iterate",	    test_iterate 	},
	//{ "set_visit",		    test_visit		},
	{ "set_benchmark_insert", 			test_benchmark_insert		},
	//{ "set_benchmark_remove", 			test_benchmark_remove	},
	//{ "set_benchmark_find", 			test_benchmark_find		},
	//{ "set_multi", 			test_multi		},
	{ NULL, NULL } // τερματίζουμε τη λίστα με NULL
};