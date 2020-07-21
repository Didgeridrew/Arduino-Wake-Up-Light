# Arduino-Wake-Up-Light

A stand-alone Arduino light with alarm function to slowly brighten LEDs to help people wake up easier in windowless bedrooms.

The fade up is spread over two output pins which I connected to MOSFETs to seperately control two sets of LED strips.
For my build I used one set of warm LEDs and one set of Daylight LEDs to get a nice color change as it fades up.



I've taken this project as far as I can/want to, I'll be transitioning to an esp32-based dev board.  

Known Issues:
  - Daylight Saving Time integration do not work properly. The board will register DST change if you power cycle it, 
    but it does not do it automatically.
