/*
 * cores_communication.h
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


#ifndef CORES_COMMUNICATION_H_
#define CORES_COMMUNICATION_H_

#ifndef BUFF_CORES_SIZE			// Defines the buffer size to transfer data
	#define BUFF_CORES_SIZE	32
#endif

typedef struct {
	// Flags to lock reading or writing
	unsigned char status_CM4_nreading:1;	// CM4 NOT reading flag
	unsigned char status_CM4_nwriting:1;	// CM4 NOT writing flag
	unsigned char status_CM7_nreading:1;	// CM7 NOT reading flag
	unsigned char status_CM7_nwriting:1;	// CM7 NOT writing flag
//	unsigned char status_CM7toCM4_has_data:1;
//	unsigned char status_CM4toCM7_has_data:1;

	unsigned int buff4to7[BUFF_CORES_SIZE];	// Buffer to transfer from core 4 to 7
	unsigned int buff7to4[BUFF_CORES_SIZE];	// Buffer to transfer from core 7 to 4

	// Stored buffer sizes. MUST BE LESS THAN BUFF_CORES_SIZE
	unsigned int buff4to7_size;
	unsigned int buff7to4_size;
} shared_data_TypeDef;


/*
 * Shared data struct
 *
 * It is configured at the beginning of D3 domain, AHB SRAM @ 0x38000000
 */
#define shared_data		((shared_data_TypeDef *) 0x38000000)


void core_share_init();
void get_from_M4(unsigned int *buffer);	// Get data from M4 to M7
void get_from_M7(unsigned int *buffer); // Get data from M7 to M4
void put_to_M4(unsigned int buffer[], unsigned int buffer_size);	// put data from M7 to M4
void put_to_M7(unsigned int buffer[], unsigned int buffer_size);	// put data from M4 to M7


#endif	/* CORES_COMMUNICATION_H_ */
