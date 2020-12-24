DSC Alarm Shield for WemosD1 Mini and WemosD1 Mini PRO
==================================

![Top view](https://raw.githubusercontent.com/PipeDeveloper/esphome-dsckeybus/master/PCB%20Layouts/WemosD1%20Mini/SMD/WemosD1_Mini_SMD1206_boardtop.png)

![Top view](https://raw.githubusercontent.com/PipeDeveloper/esphome-dsckeybus/master/PCB%20Layouts/WemosD1%20Mini/SMD/WemosD1_SMD1206_schematic.png)

Tools you need to build:

 * Multimeter
 * Solder Iron + thin tip
 * Solder Wire, best quality you can get, 0.5mm recomended or lower for SMD components, 0.8mm for the rest of components
 * Isopropilic Alcohol (to clean your board once finished)
 * Small (-) screwdriver
 * flux and solder paste (Optional)

Materials and Components:
 * 8 Female Headerpins x2
 * 3 Male Headerpins
 * 1 Two pin jumper
 * 1206 SMD resistor 33k x2
 * 1206 SMD resistor 10k x2
 * 1206 SMD resistor 1k
 * 2N3904 or 2N2222 Transistor
 * LM2596 Module
 * 4 Terminalblocks (can be 2 terminal block x2)

How to order
------------
If you think that the board is enough and you dont need to modify it, you can download the gerber file.zip and that's all. That file is asked on PCB manufacturer to start build the board.
There are a lot of chinese webpages where you can order 5 units for about 2-10dollars, just choose one where the shipping cost isn't too high in your country.

Some popular sites:

[All PCB](https://www.allpcb.com/?Mb_InviteId=69183) (For Chile Recomended via EMS)

[JLCPCB](https://jlcpcb.com/)

[PCBway](https://www.pcbway.com/setinvite.aspx?inviteid=432106)

For the rest of components you may order via Aliexpress (my favorite) or you may buy them locally on any electronic store.

Building and instalation
------------
:warning: **IMPORTANT:** Please follow this instructions. Plug the jumper in the wrong place without adjusting the LM2596 voltage first, may fry your ESP8266.
 
**Considerations:** Work on a clean area and clean your board first before start soldering.

1. Solder all resistances first. Make sure to place the correct values on the labeled pads.

2. Solder the transistor.

3. solder the terminalblocks.

4. solder the headerpins.

5. Last, solder the LM2596 Module. After finish, clean your board.

6. On this step we need to plug the board to the DSC Alarm Panel.

:warning: **Warning** I Recomend to unplug your DSC Alarm Panel from power, includding the battery before you plug the board. This is to avoid any short circuit mistake when manipulating powered cables.

Connect each terminal labeled, RED-RED, YEL-YEL, GRN-GRN, BLK-BKL. Then you may power again your DSC Alarm Panel. If everything is ok, a blue LED should light on the LM2596 module.

7. Regulate the LM2596 module to the right voltage:
With a multimeter, place the points on the OUT + and - pads and measure the voltage.

Use a small (-) screwdriver and turn the potentiometer to start drop the voltage till 3.3v (highly recommended).
**Double check the voltage and ensure to set the correct tension**

8. Plug in your WemosD1 Mini / WemosD1 Mini PRO facing as the board footprint. Make sure that pins labeled on the board match. (dont forget load the firmware first)

9. Final step: place the jumper to the selected voltage.

:warning: **Warning: regulate the LM2596 with 5v or more and plug the jumper on 3.3v pin may fry your ESP8266. Be sure to plug the jumper on the correct position.**

And you're ready to go!
