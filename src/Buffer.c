/*Buffer - Circular FIFO Buffer implementation*/

#include "Buffer.h"
#include <stdlib.h>
/* --------------------------------- String Buffer ------------------------------- */
// Adds an element to the end of the queue
extern void Buffer_add(Buffer_t* b, const char* str, uint8_t len){
	// Insert element
	memcpy(b->data[b->idx_to_load], str, len);
	b->data_len[b->idx_to_load] = len;
	b->idx_to_load++;
	b->idx_to_load %= MAX_BUFFER_SIZE;
	b->size++;

	// Check if buffer is full
	if(b->size > MAX_BUFFER_SIZE)
	{
		// Remove oldest element
		b->idx_to_pop++;
		b->idx_to_pop %= MAX_BUFFER_SIZE;
		b->size--;
		b->overflow_cnt++;
	}
}

// Removes an element from the front of the queue
extern int Buffer_pop(Buffer_t* b, char* data) {
	uint8_t ret = 0;

	// Check if the buffer has anything to pop
	if(b->size)
	{
		// Pop oldest element and store it in data
		memcpy(data, b->data[b->idx_to_pop], b->data_len[b->idx_to_pop]);
		ret = b->data_len[b->idx_to_pop];
		b->idx_to_pop++;
		b->idx_to_pop %= MAX_BUFFER_SIZE;
		b->size--;
	}

	return ret;
}

// Reset all variables of the buffer
extern void Buffer_init(Buffer_t* b){
	b->idx_to_load = 0;
	b->idx_to_pop = 0;
	b->size = 0;
	b->overflow_cnt = 0;
}

// Get the size of the buffer
extern int Buffer_size(Buffer_t* b){
	return b->size;
}

// Get the number of overflows that have occurred, and resets overflow count
extern int Buffer_overflow(Buffer_t* b){
	uint8_t overflow = b->overflow_cnt;
	b->overflow_cnt = 0;
	return overflow;
}

/* ------------------------------------- Char Buffer --------------------------------- */
// Adds an element to the end of the queue
extern void CharBuffer_add(CharBuffer_t* b, const char character){
	// Insert element
	b->data[b->idx_to_load] = character;
	b->idx_to_load++;
	b->idx_to_load %= MAX_BUFFER_SIZE;
	b->size++;

	// Check if buffer is full
	if(b->size > MAX_BUFFER_SIZE)
	{
		// Remove oldest element
		b->idx_to_pop++;
		b->idx_to_pop %= MAX_BUFFER_SIZE;
		b->size--;
		b->overflow_cnt++;
	}
}

// Removes an element from the front of the queue
extern void CharBuffer_pop(CharBuffer_t* b, char data) {
	// Check if the buffer has anything to pop
	if(b->size)
	{
		// Pop oldest element and store it in data
		data = b->data[b->idx_to_pop];
		b->idx_to_pop++;
		b->idx_to_pop %= MAX_BUFFER_SIZE;
		b->size--;
	}
}

// Reset all variables of the buffer
extern void CharBuffer_init(CharBuffer_t* b){
	b->idx_to_load = 0;
	b->idx_to_pop = 0;
	b->size = 0;
	b->overflow_cnt = 0;
}

// Get the size of the buffer
extern int CharBuffer_size(CharBuffer_t* b){
	return b->size;
}

// Get the number of overflows that have occurred, and resets overflow count
extern int CharBuffer_overflow(CharBuffer_t* b){
	uint8_t overflow = b->overflow_cnt;
	b->overflow_cnt = 0;
	return overflow;
}
/*	Copy Values from CharBuffer to a temp string, then push into Buffer
 *  	-Also resets the CharBuffer to default values
 * */
extern void CharBuffer_to_Buffer(CharBuffer_t* Src, Buffer_t* Dest){
	uint8_t len = Src->size;
	if(len != 0){
		char tempString[MAX_BUFFER_DATA] = "";
		/* Copy into tempString and set to \0 in Src */
		for(int i = 0; i < len; i++){
			tempString[i] = Src->data[i];
			Src->data[i] = '\0';
		}
		/* ReInit CharBuffer */
		Src->idx_to_load = 0;
		Src->idx_to_pop = 0;
		Src->size = 0;
		Src->overflow_cnt = 0;
		/* Insert tempString into Dest */
		memcpy(Dest->data[Dest->idx_to_load], tempString, len);
		Dest->data_len[Dest->idx_to_load] = len;
		Dest->idx_to_load++;
		Dest->idx_to_load %= MAX_BUFFER_SIZE;
		Dest->size++;
		/* Check if Dest is full */
		if(Dest->size > MAX_BUFFER_SIZE){
			// Remove oldest element
			Dest->idx_to_pop++;
			Dest->idx_to_pop %= MAX_BUFFER_SIZE;
			Dest->size--;
			Dest->overflow_cnt++;
		}
	}
};
