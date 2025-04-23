#include <stdio.h>
#include <stdlib.h>

typedef int (*transform_cb)(int);

typedef struct {
	int *data;
	int size;
	transform_cb fn;
} Sequence;

int double_it(int x) { return x * 2; }
int half_it(int x) { return x / 2; }
int square_it(int x) { return x * x; }

transform_cb get_transform(int choice) {
	if (choice == 1) return double_it;
	if (choice == 2) return half_it;
	if (choice == 3) return square_it;
	return NULL;
}

Sequence* create_sequence(int size) {
	Sequence *seq = malloc(sizeof(Sequence));
	if(seq ==  NULL){
	   return NULL;
	}
	seq->data = calloc(size, sizeof(int));
	if(seq->data ==  NULL){
	  return NULL;
	}
	seq->size = size;
	for (int i = 0; i < size; i++)
		seq->data[i] = i + 1;
	return seq;
}

void apply_transform(Sequence *seq) {
	for (int i = 0; i < seq->size; i++)
		seq->data[i] = seq->fn(seq->data[i]);
}

void print_sequence(Sequence *seq) {
	for (int i = 0; i < seq->size; i++)
		printf("%d ", seq->data[i]);
	printf("\n");
}

void destroy_sequence(Sequence **seq_ptr) {
	if (!seq_ptr) return;
	free((*seq_ptr)->data);
	free(*seq_ptr);
	*seq_ptr = NULL;
}

int main() {
	int choice;
	printf("Enter transform (1=double, 2=half, 3=square): ");
	scanf("%d", &choice);

	Sequence *seq = create_sequence(5);
	seq->fn = get_transform(choice);

	apply_transform(seq);
	print_sequence(seq);

       	destroy_sequence(&seq);//  destroy memory!
	return 0;
}
