# STM32H7xx_inter-core_data_share

## Description

This is a library used to share data back and forth between the two cores of the STM32H7xx.
It is designed to be simple, without requiring low level knowledge and provide lock safety for reading and writing.
Besides, it is easy to understand and it uses standard C libraries.


## Instructions

1. Include the files **cores_communication.c** and **cores_communication.h** in both core projects or in a common folder.
2. In one of the cores the function `core_share_init()` must be called.
3. In the M7 core, call `get_from_m4(...)` to read from the M4 core or `put_to_m4(...)` to send data to it.
4. In the M4 core, call `get_from_m7(...)` to read from the M7 core or `put_to_m7(...)` to send data to it.


Add the following in linker script of each core
```c
RAM_D3_SIZE = 64K;

/* Memories definition */
MEMORY
{
    ...
    RAM_D3 (xrw)   : ORIGIN = 0x38000000, LENGTH = RAM_D3_SIZE
    ...
}

SECTIONS
{
    .shared :
    {
        _sshared = .;
        *(.shared);
        _eshared = .;
    } > RAM_D3
    ASSERT((_eshared - _sshared) <= RAM_D3_SIZE, "RAM D3 too big")
}
```

Checkout the Youtube video where this library is designed

[![youtube_video](https://img.youtube.com/vi/SJ_QxraXdHA/0.jpg)](https://www.youtube.com/watch?v=SJ_QxraXdHA  "youtube_video")

## Contact

This project was created by Adailton Braga.  
If you have any question, get in touch by e-mail: adailtonjn68@gmail.com
