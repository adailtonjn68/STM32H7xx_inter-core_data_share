/*
 * cores_communication.c
 *
 *  Created on: 19 de mar de 2022
 *      Author: Adailton Braga Júnior
 *              adailton.braga@acad.ufsm.br
 *
 *
 * Copyright (c) 2022 Adailton Braga Júnior
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.

 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#include "cores_communication.h"
#include <stdatomic.h>
#include <limits.h>


#if BUFFSHAREDSIZE > INT_MAX
#error BUFFSHAREDSIZE must be less than INT_MAX
#endif


struct _shared {
	atomic_bool lock1, lock2;
	int buffer1[BUFFSHAREDSIZE], buffer2[BUFFSHAREDSIZE];
	unsigned int buffer1_size, buffer2_size;
};


static struct _shared shared_data __attribute__((section(".shared")));


/**
 * @brief Used to clear the buffer locks and their size holders
 *        It has to be called in only one of the cores
 */
void core_share_init(void)
{
	atomic_flag_clear(&shared_data.lock1);
	atomic_flag_clear(&shared_data.lock2);
	shared_data.buffer1_size = 0;
	shared_data.buffer2_size = 0;
}


/**
 * @brief Send data from M7 to M4
 * @param buffer
 * @param size
 * @return -1 if lock is not acquired, otherwise how many items were transfered
 */
int put_to_m4(const int *const restrict buffer, const unsigned int size)
{
	/* Try and get lock */
	if (atomic_flag_test_and_set(&shared_data.lock1)) {
		/* Return -1 in case lock is not acquired (used by other core)*/
		return -1;
	}

	/* Limit size of buffer to be transfered to the size of destine buffer */
	shared_data.buffer1_size = (size <= BUFFSHAREDSIZE) ? size : BUFFSHAREDSIZE;

	/* Copy from buffer[] to shared_data.buffer[] */
	for (unsigned int i = 0; i < shared_data.buffer1_size; i++) {
		shared_data.buffer1[i] = buffer[i];
	}

	/* Clear lock */
	atomic_flag_clear(&shared_data.lock1);

	/* Return how many items were transfered */
	return shared_data.buffer1_size;
}


/**
 * @brief Get data from M4
 * @param buffer
 * @param size
 * @return -1 if lock is not acquired, otherwise how many items were read
 */
int get_from_m4(int *const restrict buffer, unsigned int size)
{
	/* Try and get lock */
	if (atomic_flag_test_and_set(&shared_data.lock2)) {
		/* Return -1 in case lock is not acquired (used by other core)*/
		return -1;
	}

	/* Verify whether we are trying to read more items than are available */
	if (size >= shared_data.buffer2_size) {
		size = shared_data.buffer2_size;
	}

	/* Copy items from shared_data.buffer[] to buffer[] */
	for (unsigned int i = 0; i < size; i++) {
		buffer[i] = shared_data.buffer2[i];
	}

	/* Clear shared_data.buffer_size (there is no more data available) */
	shared_data.buffer2_size = 0;

	/* Clear lock */
	atomic_flag_clear(&shared_data.lock2);

	/* Return how many items were read */
	return size;
}


/**
 * @brief Verify whether CM4 has data ready for CM7 to read
 * @return -1 if lock is not acquired, otherwise how many items are available for reading
 */
int m4_has_data(void)
{
	int n_items;
	/* Try and get lock */
	if (atomic_flag_test_and_set(&shared_data.lock2)) {
		/* Return -1 in case lock is not acquired (used by other core)*/
		return -1;
	}

	n_items = (int)shared_data.buffer2_size;

	/* Clear lock */
	atomic_flag_clear(&shared_data.lock2);

	return n_items;
}


/**
 * @brief Send data from M4 to M7
 * @param buffer
 * @param size
 * @return -1 if lock is not acquired, otherwise how many items were transfered
 */
int put_to_m7(const int *const restrict buffer, const unsigned int size)
{
	/* Try and get lock */
	if (atomic_flag_test_and_set(&shared_data.lock2)) {
		/* Return -1 in case lock is not acquired (used by other core)*/
		return -1;
	}

	/* Limit size of buffer to be transfered to the size of destine buffer */
	shared_data.buffer2_size = (size <= BUFFSHAREDSIZE) ? size : BUFFSHAREDSIZE;

	/* Copy from buffer[] to shared_data.buffer[] */
	for (unsigned int i = 0; i < shared_data.buffer2_size; i++) {
		shared_data.buffer2[i] = buffer[i];
	}

	/* Clear lock */
	atomic_flag_clear(&shared_data.lock2);

	/* Return how many items were transfered */
	return shared_data.buffer2_size;
}


/**
 * @brief Get data from M7
 * @param buffer
 * @param size
 * @return -1 if lock is not acquired, otherwise how many items were read
 */
int get_from_m7(int *const restrict buffer, unsigned int size)
{
	/* Try and get lock */
	if (atomic_flag_test_and_set(&shared_data.lock1)) {
		/* Return -1 in case lock is not acquired (used by other core)*/
		return -1;
	}

	/* Verify whether we are trying to read more items than are available */
	if (size >= shared_data.buffer1_size) {
		size = shared_data.buffer1_size;
	}

	/* Copy items from shared_data.buffer[] to buffer[] */
	for (unsigned int i = 0; i < size; i++) {
		buffer[i] = shared_data.buffer1[i];
	}

	/* Clear shared_data.buffer_size (there is no more data available) */
	shared_data.buffer1_size = 0;

	/* Clear lock */
	atomic_flag_clear(&shared_data.lock1);

	/* Return how many items were read */
	return size;
}


/**
 * @brief Verify whether CM7 has data ready for CM4 to read
 * @return -1 if lock is not acquired, otherwise how many items are available for reading
 */
int m7_has_data(void)
{
	int n_items;
	/* Try and get lock */
	if (atomic_flag_test_and_set(&shared_data.lock1)) {
		/* Return -1 in case lock is not acquired (used by other core)*/
		return -1;
	}

	n_items = (int)shared_data.buffer1_size;

	/* Clear lock */
	atomic_flag_clear(&shared_data.lock1);

	return n_items;
}
