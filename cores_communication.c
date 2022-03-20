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


static unsigned int buffer_size_limited_4to7, buffer_size_limited_7to4;


/*
 * core_share_init()
 * Set nreading and nwriting flags to 1
 */
void core_share_init() {
	shared_data->status_CM4_nwriting = ~0;
	shared_data->status_CM4_nreading = ~0;
	shared_data->status_CM7_nwriting = ~0;
	shared_data->status_CM7_nreading = ~0;
}


/*
 * Get data from M4 to M7
 */
void get_from_M4(unsigned int *buffer) {
	if (shared_data->status_CM4_nwriting) {		// if M4 to M7 buffer has data
		shared_data->status_CM7_nreading = 0;	// Lock buffer

		for(unsigned int n = 0; n < shared_data->buff4to7_size; ++n) {
			*(buffer+n) = shared_data->buff4to7[n];	// Transfer data
		}
		shared_data->status_CM7_nreading = 1;	// Unlock buffer
	}
}


/*
 * Send data from M7 to M4
 */
void put_to_M4(unsigned int buffer[], unsigned int buffer_size) {

	if (shared_data->status_CM4_nreading) {	// if M7 to M4 buffer is not locked
		shared_data->status_CM7_nwriting = 0;	// Lock buffer

		buffer_size_limited_7to4 = (buffer_size > BUFF_CORES_SIZE) ? BUFF_CORES_SIZE : buffer_size;

		shared_data->buff7to4_size = buffer_size_limited_7to4;
		for (unsigned int n = 0; n < buffer_size_limited_7to4; ++n) {
			shared_data->buff7to4[n] = buffer[n];	// Transfer data
		}

		shared_data->status_CM7_nwriting = 1;

	}
}


/*
 * Get data from M7 to M4
 */
void get_from_M7(unsigned int *buffer) {
	if (shared_data->status_CM7_nwriting) {	// if M7 to M4 buffer has data
		shared_data->status_CM4_nreading = 0;		// Lock buffer

		for(unsigned int n = 0; n < shared_data->buff7to4_size; ++n) {
			*(buffer+n) = shared_data->buff7to4[n];	// Transfer data
//			shared_data->buff7to4[n] = 0;			// Clear buffer
		}
		shared_data->status_CM4_nreading = 1;	// Unlock buffer
	}
}


/*
 * Send data from M4 to M7
 */
void put_to_M7(unsigned int buffer[], unsigned int buffer_size) {

	if (shared_data->status_CM7_nreading) {	// if M4 to M7 buffer is not locked
		shared_data->status_CM4_nwriting = 0;		// Lock buffer

		buffer_size_limited_4to7 = (buffer_size > BUFF_CORES_SIZE) ? BUFF_CORES_SIZE : buffer_size;

		shared_data->buff4to7_size = buffer_size_limited_4to7;
		for (unsigned int n = 0; n < buffer_size_limited_4to7; ++n) {
			shared_data->buff4to7[n] = buffer[n];	// Transfer data
		}

		shared_data->status_CM4_nwriting = 1;

	}
}
