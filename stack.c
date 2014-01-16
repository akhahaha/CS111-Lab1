

/*
 * watch out for segfaults
 * max items is 100
 * remember to set num_items before use
 */

struct stack {
	command_t commands[100];
	int num_items;
};
typedef struct stack stack;

command_t peek(stack *s)
{
	return s->commands[s->num_items - 1];
}

command_t pop(stack *s)
{
	s->num_items--;
	return s->commands[s->num_items];
}

command_t push(stack *s, command_t cmd)
{
	s->num_items++;
	s->commands[s->num_items - 1] = cmd;
	
	return cmd;
}

int is_empty(stack *s)
{
	return s->num_items == 0;
}