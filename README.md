# logue-vosim
VOSIM oscillator for logue SDK (see https://www.kaegi.nl/werner/userfiles/downloads/vosim-system.pdf)

## Build instructions

1. Copy directories `minilogue-xd/`, `nutekt-digital/`, `prologue/` from repository to directory `<your cloned logue-sdk location>/platform`
    - Ensure that `minilogue-xd/`, `nutekt-digital/`, `prologue/` directories contains `vosim` directory
2. Go to needed platform directory and build oscillator using `make`
3. Upload resulting module to device

```
[~/workspace/logue-sdk/platform/nutekt-digital/vosim]$ make
Compiler Options
../../../tools/gcc/gcc-arm-none-eabi-5_4-2016q3/bin/arm-none-eabi-gcc -c -mcpu=cortex-m4 -mthumb -mno-thumb-interwork -DTHUMB_NO_INTERWORKING -DTHUMB_PRESENT -g -Os -mlittle-endian -mfloat-abi=hard -mfpu=fpv4-sp-d16 -fsingle-precision-constant -fcheck-new -std=c11 -mstructure-size-boundary=8 -W -Wall -Wextra -Wa,-alms=./build/lst/ -DSTM32F446xE -DCORTEX_USE_FPU=TRUE -DARM_MATH_CM4 -D__FPU_PRESENT -I. -I./inc -I./inc/api -I../inc -I../inc/dsp -I../inc/utils -I../../ext/CMSIS/CMSIS/Include

Compiling _unit.c
Compiling vosim.cpp
Linking build/VOSIM.elf
Creating build/VOSIM.hex
Creating build/VOSIM.bin
Creating build/VOSIM.dmp

   text    data     bss     dec     hex filename
   1016       0      36    1052     41c build/VOSIM.elf

Creating build/VOSIM.list
Packaging to ./VOSIM.ntkdigunit

Done
```

## Params description

| param | logue param | closest term in Kaegi paper |
| --- | --- | --- |
| M | shape | delay |
| b | shift-shape | attenuation |
| Freq | param1 | reciprocal of pulse T |
| N | param2 | number of pulses per period |
| LFO Target | param3 | LFO Target: 1 means delay, 2 means attenuation |

## Disclaimer
Only NTS-1 module is tested, please confirm if it works for other platforms
