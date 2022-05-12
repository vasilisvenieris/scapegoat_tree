# Κάνοντας compile το <foo>_test.c με μια υλοποίηση <foo>.c του
# συγκεκριμένου τύπου, παράγουμε ένα tets για την υλοποίηση αυτή.


VECTOR_DYN_ARRAY = $(MODULES)/ADTVector/ADTVector.o
SET_AVL_LL = $(MODULES)/ADTSet/UsingAVL_InternalLL/ADTSet.o
SET_SG_PATH = $(MODULES)/ADTSet/UsingScapegoat_Path/ADTSet.o

SET_SIMPLE_TEST = $(TESTS)/Set/ADTSet_simple_test.o
SET_BENCHMARK_TEST = $(TESTS)/Set/ADTSet_benchmark_test.o


#------------------------------------------------------------------------------------

##################################################
###################ADTSet_αρχή####################

# Both implementation have internal linked lists for O(1) next/previous

#ΑVL implementation
#UsingAVL_IntLL_ADTSet_test_OBJS = $(TESTS)/Set/ADTSet_benchmark_test.o $(SET_AVL_LL) $(VECTOR_DYN_ARRAY)

#Scapegoat tree implementation
#UsingSG_ADTSet_test_OBJS = $(SET_SIMPLE_TEST) $(SET_SG_PATH)
UsingSG_Path_ADTSet_test_OBJS = $(SET_BENCHMARK_TEST) $(SET_SG_PATH) $(VECTOR_DYN_ARRAY)
###################ADTSet_τέλος###################
##################################################

#------------------------------------------------------------------------------------

# Ο βασικός κορμός του Makefile
include ../common.mk
