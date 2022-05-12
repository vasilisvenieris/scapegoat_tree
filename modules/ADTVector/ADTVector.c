///////////////////////////////////////////////////////////
// ADT Vector implementation with Dynamic Array.
///////////////////////////////////////////////////////////

#include "ADTVector.h"

#define VECTOR_MIN_CAPACITY 10

struct vector_node{
	Pointer value;
};

struct vector{
    // array 
	VectorNode array;			

	int size, capacity;		

	DestroyFunc destroy_value;
};

// creates the vector 
Vector vector_create(int size, DestroyFunc destroy_value){
	Vector vec = malloc(sizeof(*vec));

	vec->size = size;
	vec->destroy_value = destroy_value;

	// array is created
	vec->capacity = size < VECTOR_MIN_CAPACITY ? VECTOR_MIN_CAPACITY : size;

    // all indexes initialized to NULL
	vec->array = calloc(vec->capacity, sizeof(*vec->array));

	return vec;
}

int vector_size(Vector vec){
	return vec->size;
}

Pointer vector_get_at(Vector vec, int pos){
	assert(pos >= 0 && pos < vec->size);

	return vec->array[pos].value;
}

void vector_set_at(Vector vec, int pos, Pointer value){
	assert(pos >= 0 && pos < vec->size);

	//if destroy_value is defined, it is used to free up space
	if (value != vec->array[pos].value && vec->destroy_value != NULL)
		vec->destroy_value(vec->array[pos].value);

	vec->array[pos].value = value;
}

void vector_insert_last(Vector vec, Pointer value){
	// array doubles in size if it becomes full
	if(vec->capacity == vec->size){
		vec->capacity *= 2;
		vec->array = realloc(vec->array, vec->capacity * sizeof(*vec->array));
	}

	// adding item
	vec->array[vec->size].value = value;
	vec->size++;
}

void vector_remove_last(Vector vec){
	assert(vec->size != 0);

	//if destroy_value is defined, it is used to free up space
	if(vec->destroy_value != NULL)
		vec->destroy_value(vec->array[vec->size - 1].value);

	vec->size--;

	// Halves array if capacity is more than 4 times the size (75%+ of array is empty)
	if(vec->capacity > vec->size * 4 && vec->capacity > 2*VECTOR_MIN_CAPACITY){
		vec->capacity /= 2;
		vec->array = realloc(vec->array, vec->capacity * sizeof(*vec->array));
	}
}

Pointer vector_find(Vector vec, Pointer value, CompareFunc compare){
	// runs through array
	for(int i = 0; i < vec->size; i++)
		if(compare(vec->array[i].value, value) == 0)
			return vec->array[i].value;

	return NULL;
}

DestroyFunc vector_set_destroy_value(Vector vec, DestroyFunc destroy_value){
	DestroyFunc prev_destroy_value = vec->destroy_value;
	vec->destroy_value = destroy_value;
	return prev_destroy_value;
}

void vector_destroy(Vector vec){
	//if destroy_value is defined, it is used to free up space
	if(vec->destroy_value != NULL)
		for(int i = 0; i < vec->size; i++)
			vec->destroy_value(vec->array[i].value);


	free(vec->array);
	free(vec);
}

////////////////////////////////////////accessing though nodes /////////////////////////////////////////////////////

VectorNode vector_first(Vector vec){
    return (vec->size == 0) ? NULL : &vec->array[0];
}

VectorNode vector_last(Vector vec){
    return (vec->size == 0) ? NULL : &vec->array[vec->size-1];
}

VectorNode vector_next(Vector vec, VectorNode node){
    return (node == &vec->array[vec->size-1]) ? NULL : node + 1;
}

VectorNode vector_previous(Vector vec, VectorNode node){
    return (node == &vec->array[0]) ? NULL : node - 1;
}

Pointer vector_node_value(Vector vec, VectorNode node){
	return node->value;
}

VectorNode vector_find_node(Vector vec, Pointer value, CompareFunc compare){
	// runs through array
	for (int i = 0; i < vec->size; i++)
		if (compare(vec->array[i].value, value) == 0)
			return &vec->array[i];

	return NULL;
}