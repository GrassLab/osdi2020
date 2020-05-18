# OSDI Lab2

## How to test when running on Raspberry Pi 3

```bash
cd osdi2020
python3 pyserial.py <path to the device>
```

## How to test the functionality of `irq`

```
python3 irq.py <path to the device>
```

Afterward, send command `irq` to the shell and this script will enter in a forever loop for keeping receiving the log of timer interrupt.
