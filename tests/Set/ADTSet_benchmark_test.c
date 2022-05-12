//////////////////////////////////////////////////////////////////
//
// Unit tests or ADT Set.
//
//////////////////////////////////////////////////////////////////

#include "acutest.h"			// library for unit testing
#include "ADTSet.h"

clock_t start_t, end_t;
double total_t;
int iters = 1;
int N = 12000;

// checks whether the set is proper or it contains errors (BEWARE: !!ΧΡΟΝΟΒΟΡΟ!!)
bool set_is_proper(Set set);
bool set_proper_check = false;

// Shuffles value_array before items are inserted in the set
bool value_shuffle_check = false;

// Shuffles index_array so that accessing is different to value_array
bool index_shuffle_check = false;

// bulk_check overrides value_shuffle_check
bool bulk_check = false; 

//
//  Helper functions
//
// Compare function
int compare_ints(Pointer a, Pointer b) {
	return *(int*)a - *(int*)b;
}

// Checks insert, wrapper function
void insert_and_test(Set set, Pointer value) {
	set_insert(set, value);
	if(set_proper_check)
		TEST_ASSERT(set_is_proper(set));
	TEST_ASSERT(set_find(set, value) == value);
}

// shuffles arrays
void shuffle(int *array, int n) {
    srand(time(NULL));
	for (int i = 0; i < n; i++) {
		int j = i + rand() / (RAND_MAX / (n - i) + 1);
		int t = array[j];
		array[j] = array[i];
		array[i] = t;
	}
}

//
//  Test functions 
//
////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////

void test_benchmark_create() {
	total_t = 0;

	int *value_array = malloc(N * sizeof(*value_array));
	Vector vector = vector_create(0, NULL);
	for(int i = 0; i < N; i++){
		value_array[i] = i;
		vector_insert_last(vector, &value_array[i]);
	}
		
	for(int i = 0; i < iters; i++){
		start_t = clock();

		// creates a set and chooses whether it will be initialized with values
		Set set = bulk_check ? set_create(compare_ints, NULL, vector) : set_create(compare_ints, NULL, NULL);
		end_t = clock();

		set_destroy(set);
		total_t += (end_t - start_t)/(double)CLOCKS_PER_SEC;
	}

	vector_destroy(vector);
	free(value_array);
	printf("Avg time for set_create: %f",total_t/iters);
}

void test_benchmark_insert(){
	total_t = 0;
	Set set;

    srand(time(NULL));

    int *value_array = malloc(N * sizeof(*value_array));
    Vector vector = vector_create(0, NULL);
	for(int i = 0; i < N; i++){
		value_array[i] = i;
		vector_insert_last(vector, &value_array[i]);
	}

	for(int i = 0; i < iters; i++){
		// shuffles value_array so that there is a uniform insert of values
		// eg. if values are inserted monotonically (as in, sorted order), if set is implemented through BST
		// nodes will only be added in the right subtree and thus set_remove will not be fully tested
		if(!bulk_check && value_shuffle_check)
			shuffle(value_array, N);
		
		if(!bulk_check){
			set = set_create(compare_ints, NULL, NULL);
			
			start_t = clock();
			// inserting new values to the set each time
			for(int i = 0; i < N; i++)
				set_insert(set, &value_array[i]);

			end_t = clock();
		}
		else{
			start_t = clock();
			set = set_create(compare_ints, NULL, vector);
			end_t = clock();
		}
			
		set_destroy(set);
		total_t += (end_t - start_t)/(double)CLOCKS_PER_SEC;
	}

    free(value_array);
	vector_destroy(vector);
	printf("Avg time for set_insert: %f",total_t/iters);
}

// calculates average node depth and average rotations per item
void test_benchmark_bst_size(){
	total_t = 0;
	unsigned int total_rot = 0, total_len = 0;
	Set set;

    srand(time(NULL));

    int *value_array = malloc(N * sizeof(*value_array));
    Vector vector = vector_create(0, NULL);
	for(int i = 0; i < N; i++){
		value_array[i] = i;
		vector_insert_last(vector, &value_array[i]);
	}

	for(int i = 0; i < iters; i++){
		// shuffles value_array so that there is a uniform insert of values
		// eg. if values are inserted monotonically (as in, sorted order), if set is implemented through BST
		// nodes will only be added in the right subtree and thus set_remove will not be fully tested
		if(!bulk_check && value_shuffle_check)
			shuffle(value_array, N);
		
		if(!bulk_check){
			set = set_create(compare_ints, NULL, NULL);

			start_t = clock();
			// inserting new values to the set each time
			for(int i = 0; i < N; i++)
				set_insert(set, &value_array[i]);

			end_t = clock();
		}
		else{
			start_t = clock();
			set = set_create(compare_ints, NULL, vector);
			end_t = clock();
		}
		
		total_len += set_length(set);
		total_rot += set_rotations(set);
		//printf("Total length: %d || Total rotations: %d", set_length(set), set_rotations(set));
		set_destroy(set);
		total_t += (end_t - start_t)/(double)CLOCKS_PER_SEC;
	}

	printf("Avg len: %.5f || Avg rot: %.5f || ", (double)total_len/(double)iters/(double)N, (double)total_rot/(double)iters/(double)N);
    free(value_array);
	vector_destroy(vector);
	printf("Avg time for set_insert: %f",total_t/iters);
}

void test_benchmark_remove(){
	total_t = 0;

    srand(time(NULL));

    int *value_array = malloc(N * sizeof(*value_array));
    Vector vector = vector_create(0, NULL);
	for(int i = 0; i < N; i++){
		value_array[i] = i;
		vector_insert_last(vector, &value_array[i]);
	}

	// our dummy array, to be shuffled
	int *index_array = malloc(N * sizeof(*index_array));
	for(int i = 0; i < N; i++)
		index_array[i] = i;

	for(int i = 0; i < iters; i++){
		Set set = bulk_check ? set_create(compare_ints, NULL, vector) : set_create(compare_ints, NULL, NULL);

		// shuffles value_array so that there is a uniform insert of values
		// eg. if values are inserted monotonically (as in, sorted order), if set is implemented through BST
		// nodes will only be added in the right subtree and thus set_remove will not be fully tested
		if(!bulk_check && value_shuffle_check)
			shuffle(value_array, N);
		
		if(!bulk_check)
			for(int j = 0; j < N; j++)
				set_insert(set, &value_array[j]);

		if(index_shuffle_check)
			shuffle(index_array, N);

        start_t = clock();
		// erases all values, one by one
		for(int j = 0; j < N; j++)
			set_remove(set, &value_array[index_array[j]]);
		
        end_t = clock();

		set_destroy(set);
		total_t += (end_t - start_t)/(double)CLOCKS_PER_SEC;
	}

	free(value_array);
	free(index_array);
	vector_destroy(vector);
	printf("Avg time for set_remove: %f",total_t/iters);
}

void test_benchmark_find(){
	total_t = 0;

	int *value_array = malloc(N * sizeof(*value_array));

	Vector vector = vector_create(0, NULL);
	for(int i = 0; i < N; i++){
		value_array[i] = i;
		vector_insert_last(vector, &value_array[i]);
	}

	Set set = bulk_check ? set_create(compare_ints, NULL, vector) : set_create(compare_ints, NULL, NULL);

	// as in set_remove, if the tree isn't correctly balanced, find function could fail even if the items are in in
	if(!bulk_check && value_shuffle_check)
		shuffle(value_array, N);
	
	if(!bulk_check)
		for(int i = 0; i < N; i++)
			set_insert(set, &value_array[i]);

	// our dummy array, to be shuffled
	int *index_array = malloc(N * sizeof(*index_array));
	for(int i = 0; i < N; i++)
		index_array[i] = i;
	
	for(int i = 0; i < iters; i++){
		if(index_shuffle_check)
			shuffle(index_array, N);

		start_t = clock();
		for(int j = 0; j < N; j++)
			set_find(set, &value_array[index_array[j]] );

		end_t = clock();
		total_t += (end_t - start_t)/(double)CLOCKS_PER_SEC;
	}
		
	set_destroy(set);
	free(value_array);
	free(index_array);
	vector_destroy(vector);
	printf("Avg time for set_find:   %f",total_t/iters);
}

// Chooses operations at random: insert, remove, find
// Writes results into .csv
void test_multi(){
	int *value_array = malloc(N * sizeof(*value_array));
	bool *inserted_array = calloc(N, sizeof(*inserted_array));
	Vector vector = vector_create(N, NULL);

	// queries/integers ratio
	double qRatio = 5;

	int elem_inserted = 0;
	int elem_removed = 0;
	int elem_found = 0;

	double total_inserted = 0;
	double total_removed = 0;
	double total_found = 0;

	for(int j = 0; j < N; j++){
		value_array[j] = j;
		vector_set_at(vector, j, &value_array[j]);
	}
	FILE *fp1 = fopen("benchmark_multi.csv","w+");

	// find/remove bias
	// 0.00 - 0.49... : more remove operations
	// 0.51 - 1.00    : more find operations
	for(double bias = 0; bias <= 1.0001; bias += 0.00625){
		total_t = 0;

		for(int i = 0; i < iters; i++){
			srand(time(NULL));
			Set set = bulk_check ? set_create(compare_ints, NULL, vector) : set_create(compare_ints, NULL, NULL);

			// if items are NOT inserted through bulk, they can be shuffled
			if(!bulk_check && value_shuffle_check)
				shuffle(value_array, N);

			// initialized with bulk_check:
			// true, if items were inserted through bulk
			// false, otherwise
			for(int j = 0; j < N; j++)
				inserted_array[j] = bulk_check;

			elem_found = 0;
			elem_inserted = 0;
			elem_removed = 0;

			start_t = clock();

			// executes qRatio * Ν operations: insertion, removal, search
			for(int j = 0; j < qRatio * N; j++){
				int elem = rand() % N;


				// random index is picked between 0 and Ν-1
				// random operation is picked
				// 0: removal, 1: search
				if(inserted_array[elem] == true){
					if(rand() % 10000 >= 9999 * bias){
						set_remove(set, &value_array[elem]);
						inserted_array[elem] = false;
						elem_removed++;
					}
					else{
						set_find(set, &value_array[elem]);
						elem_found++;
					}
				}
				else{
					set_insert(set, &value_array[elem]);
					inserted_array[elem] = true;
					elem_inserted++;
				}	
			}

			end_t = clock();
			total_inserted += elem_inserted;
			total_removed += elem_removed;
			total_found += elem_found;

			set_destroy(set);
			total_t += (end_t - start_t)/(double)CLOCKS_PER_SEC;
		}

		fprintf(fp1, "%.5f, %f, %.2f, %.2f, %.2f, %.2f, %.2f, %.2f\n", 
					bias, total_t/iters, 
					total_inserted/iters, 100 * total_inserted/N/qRatio/iters,
					total_removed/iters, 100 * total_removed/N/qRatio/iters,
					total_found/iters, 100 * total_found/N/qRatio/iters);

		// printf("\nBias: %.5f,Avg elems|inserted: %.2f(%2.2f%%), removed: %.2f(%2.2f%%), found: %.2f(%2.2f%%)\n", 
		// 				bias, 
		// 				total_inserted/iters, 100 * total_inserted/N/qRatio/iters,
		// 				total_removed/iters,  100 * total_removed/N/qRatio/iters,
		// 				total_found/iters, 	  100 * total_found/N/qRatio/iters);

		total_found = 0;
		total_inserted = 0;
		total_removed = 0;
	}
	
	fclose(fp1);
	vector_destroy(vector);
    free(value_array);
	free(inserted_array);
}

// Writes in .csv: average access time for perfect tree sizes
// Comparison between avl and bulk as to their trees
// Not widely used for the thesis
void test_avl_sgbulk(){
	total_t = 0;
	for(int k = 63; k < 1500000; k = 2 * (k + 1) - 1){
		for(int i = 0; i < iters; i++){
			int *value_array = malloc(k * sizeof(*value_array));
			Vector vector = vector_create(0, NULL);

			for(int i = 0; i < k; i++){
				value_array[i] = i;

				if(bulk_check)
					vector_insert_last(vector, &value_array[i]);
			}

			Set set = bulk_check ? set_create(compare_ints, NULL, vector) : set_create(compare_ints, NULL, NULL);

			// shuffles value_array so that there is a uniform insert of values
			// eg. if values are inserted monotonically (as in, sorted order), if set is implemented through BST
			// nodes will only be added in the right subtree and thus set_remove will not be fully tested
			if(!bulk_check && value_shuffle_check)
				shuffle(value_array, k);
			
			if(!bulk_check)
				for(int i = 0; i < k; i++)
					set_insert(set, &value_array[i]);

			// index array, to be shuffled
			int *index_array = malloc(k * sizeof(*index_array));
			for(int i = 0; i < k; i++)
				index_array[i] = i;

			if(index_shuffle_check)
				shuffle(index_array, k);

			start_t = clock();
			for(int j = 0; j < k; j++)
				set_find(set, &value_array[index_array[j]] );

			end_t = clock();
			total_t += (end_t - start_t)/(double)CLOCKS_PER_SEC;

			set_destroy(set);
			free(value_array);
			free(index_array);
			vector_destroy(vector);
		}

		FILE *fp1 = fopen("benchmark_avl_sgbulk.csv","a+");
		fprintf(fp1, "%d, %.6f\n", k, total_t/iters);
		fclose(fp1);
		
		total_t = 0;
	}
}	



// Test list
TEST_LIST = {
	{ "set_benchmark_create",			test_benchmark_create		},
	{ "set_benchmark_insert", 			test_benchmark_insert		},
	{ "set_benchmark_bst_size", 			test_benchmark_bst_size		},
	{ "set_benchmark_remove", 			test_benchmark_remove	},
	{ "set_benchmark_find", 			test_benchmark_find		},
	{ "set_multi", 			test_multi		},
	{ "set_avl_sgbulk", test_avl_sgbulk},
	{ NULL, NULL }
};