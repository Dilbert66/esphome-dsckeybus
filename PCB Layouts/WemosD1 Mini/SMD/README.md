DSC Alarm Shield for WemosD1 Mini and WemosD1 Mini PRO
==================================

![Top view](https://raw.githubusercontent.com/PipeDeveloper/esphome-dsckeybus/master/PCB%20Layouts/WemosD1%20Mini/SMD/WemosD1_Mini_SMD1206_boardtop.png)

![Top view](https://raw.githubusercontent.com/PipeDeveloper/esphome-dsckeybus/master/PCB%20Layouts/WemosD1%20Mini/SMD/WemosD1_SMD1206_schematic.png)

Getting started
------------
Tools you will need:

 * Multimeter or Digital Voltimeter
 * Soldering Iron and thin tip
 * Soldering Wire, best quality you can get, 0.5mm recomended or lower for SMD components, 0.8mm for the rest of components
 * Isopropilic Alcohol (to clean your board once finished)
 * Small (-) screwdriver
 * (Optional) Flux and Soldering Paste

Materials and Components:
 * 8 Female Headerpins x2
 * 3 Male Headerpins
 * 1 Two Pin jumper
 * 1206 SMD resistor 33k x2
 * 1206 SMD resistor 10k x2
 * 1206 SMD resistor 1k
 * 2N3904 or 2N2222 Transistor
 * LM2596 Module
 * 4 Terminal Blocks (can be 2 terminal blocks x2)
 * Optional: M3 screws or standoffs / spacers to secure the board. x4

How to order
------------
If you think that the board is enough and you dont need to modify it, you can download the `gerber.zip` file and that's all. PCB manufacturer will ask for that file in order for you to start building the board.
There are a lot of chinese webpages where you can order 5 units for about 2-10 dollars, just choose one where the shipping cost isn't too high in your country.

Some popular sites:

[All PCB](https://www.allpcb.com/?Mb_InviteId=69183) (For Chile Recomended via EMS)

[JLCPCB](https://jlcpcb.com/)

[PCBway](https://www.pcbway.com/setinvite.aspx?inviteid=432106)

You can order the rest of the components on Aliexpress (my favorite) or you may buy them locally at any electronic store.

Building
------------

**Considerations:** Work on a clean area and clean your board first before you start soldering.

1. Solder all resistances first. Make sure to place the correct values on the labeled pads.

2. Solder the transistor.

3. solder the terminal blocks.

4. solder the headerpins.

5. Last, solder the LM2596 Module.

6. After finish clean your board.

Installing to your DSC Alarm Panel
------------

:warning: **IMPORTANT:** Please follow these instructions. Plug the jumper in the wrong place without adjusting the LM2596 voltage first, may fry your ESP8266.
Before installing make sure all removable components are disconected. WemosD1 Mini and jumper MUST BE DISCONECTED.

Optional: Before installing it’s recommended that you secure the board using an enclosure, standoffs/spacers or screw it on a non conductive surface. A quick way is to just cover the bottom of the circuit with electrical tape.

1. Plug the board to the DSC Alarm Panel.

:warning: **Warning** I recommend unplugging your DSC alarm panel from power first, including the battery, before you plug the board in. This is to avoid any short circuit mistake when manipulating powered cables.

Connect each terminal labeled, RED-RED, YEL-YEL, GRN-GRN, BLK-BKL. Then you may power your DSC Alarm Panel again. If everything is ok, a blue LED should light on the LM2596 module.

2. Regulate the LM2596 module to the right voltage:
With a multimeter, place the points on the OUT + and - pads and measure the voltage.

Use a small (-) screwdriver and turn the patentiometer to start to drop the voltage till 3.3v (highly recommended)
**Double check the voltage and ensure you set the correct tension**

3. Plug your WemosD1 Mini / WemosD1 Mini PRO facing as the board footprint. Make sure that pins labeled on the board match. **Don't forget load the firmware first**

4. Place the jumper at the selected voltage.

:warning: **Warning: Regulating the LM2596 with 5v or more and plugging the jumper on 3.3v pin may fry your ESP8266. Be sure to plug the jumper in the correct position.**

And you're ready to go!
