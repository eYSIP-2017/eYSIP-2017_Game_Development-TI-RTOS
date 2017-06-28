VM_RTOS Summary
----------------
This is the switching statechart implementation of a vending machine abstraction, 
using TI-RTOS. User can enter money using switch presses, select desired soda once 
money has been entered and soda is dispensed using LED blink, followed by change 
dispensing using LEDs.

Peripherals Exercised
---------------------
It uses a game console interfaced with Tiva C TM4C123GH6PM Board, and utilizes
switches, LEDs and GLCD interfaced with the Board.

Example Usage
-------------
This can be used as a simple Vending Machine Firmware abstraction, which receives inputs
using switch presses and displays outputs as LED blinks.

Application Design Details
--------------------------
This Project is built out of the Empty project which 
is the same as the "Empty (Minimal)" example except many
development and debug features are enabled. For example:
    - Logging is enabled
    - Assert checking is enabled
    - Kernel Idle task
    - Stack overflow checking
    - Default kernel heap is present

Please refer to the "Memory Footprint Reduction" section in the TI-RTOS User
Guide (spruhd4.pdf) for a complete and detailed list of the differences
between the empty minimal and empty projects.

For GNU and IAR users, please read the following website for details about semi-hosting
http://processors.wiki.ti.com/index.php/TI-RTOS_Examples_SemiHosting.
