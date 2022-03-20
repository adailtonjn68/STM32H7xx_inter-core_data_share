# STM32H7xx_inter-core_data_share

Include the files cores_communication.c and cores_communication.h in both core projects or in a common folder.

In one of the cores the function core_share_init() must be called.

The function get_from_M4(unsigned int \*buffer) reads from the M4 core. buffer is an array in the M4 core to hold the read values.
The function put_to_M4(unsigned int buffer[], unsigned int buffer_size) writes from core M7 to M4 the data in buffer. buffer_size is its size.
The same goes to functions get_from_M7(unsigned int \*buffer) and put_to_M7(unsigned int buffer[], unsigned int buffer_size).
