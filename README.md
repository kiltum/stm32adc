# STM32 ADC HAL multiple channels bug fix

If you want to configure multiple ADC channels with STM32CubeMX, and
use them without interrupts/DMA, you will meet this bug.

By some reason you cannot configure HAL to read only one channel from multiple configured. You only can ADD or REMOVE channels from list. 

In adc.c you will find working code snippet

1. Create with CubeMX your configuration.
2. Use snippet for reading that channel that you need when you need, not random channel from configured list. 
3. ...
4. Profit!

