## Spotify Display
---
I made a spotify display case, it does a number of things, it is recharchable by a battery module(it is connected to a TP4056 charging module with 3.3V output for the esp32), it has holes for both uploading the code as well as charging the battery

### Display
#### Startup
on startup is shows an animation of a red heart thumping, the text "Manis Music" and "by ahmed" on the screen in green, this lasts for 8 second I beleive
it switches to a loading... screen
#### Main screen
THE MAIN SCREEN shows up which has the pakistan flag on the top right
it has the artist name and track in text that cycles through rainbow colours every 1 second, and the text also has a shadow
there is a progress bar as well the minutes and second below it of how much of the song is complete
there is also a volume bar which changes by increments of 5, starting at 50 percent on startup, this can be contorlled by the volume up and down buttons(this changes the spotify volume, does not work on mobile spotify, only desktop)
the case as a whole has 5 buttons: next, play/pause, previous, Vol up, vol down
#### The Case layout
the screen is in the front of the case, the case has a lid at the back that can be latched on and off.
inside the case there is a space for the screen, screw holes to latch it on, as well as a place for the ESP32 which also has holes to latch it on with a bolt or screw, there is a space for the USB C charging module as well as the battery securing spot.
I also have spots for the keyswitches to sit securely

### Example Images
---
<img width="1100" height="750" alt="image" src="https://github.com/user-attachments/assets/b943b6e9-f9e5-430e-abf4-5fcda510fce7" />
<img width="953" height="715" alt="image" src="https://github.com/user-attachments/assets/f2205d7b-c61c-46a9-ba88-062878ddb63d" />
<img width="1021" height="628" alt="image" src="https://github.com/user-attachments/assets/e74c8c5e-2935-4362-9aac-0d8250ab96dc" />
<img width="1154" height="506" alt="image" src="https://github.com/user-attachments/assets/cc293a77-3b60-4b63-bc40-4f0dbdad194b" />
<img width="1227" height="412" alt="image" src="https://github.com/user-attachments/assets/fb133b27-c106-4e66-ac8c-bef2e85924fa" />

### wiring diagram
---
<img width="890" height="545" alt="image" src="https://github.com/user-attachments/assets/7fb1581b-f311-4a84-af12-2840e7bba404" />



this is the final, with the space for for all componets to sit securely in the case, and for it to securely close, as well as a heart in which an led will glow, preferrably red!!!

### IM GOING TO PAY FOR THIS ALL MYSELF

## BOM

| **DISPLAYparts** | **cost USD** | **where to get from/link** | 
|------------------|--------------|----------------------------|
| 3d printer parts | 133 g(diff filaments diff cost but about 1.8 USD) | I get my filament from microless and print with my personal printer |
| Keyboard switches(better reliability) | 7 USD(pakistan local online shop/I suggest getting from another for obvious reasons) | [Daraz Pakistan](https://www.daraz.pk/products/mechanical-keyboard-switches-3-pin-rgb-hot-swappable-blue-red-brown-black-silent-tactile-switches-for-gaming-keyboard-70-million-clicks-durability-pc-gaming-keyboard-accessories-diy-keyboard-switches-set-i936558752-s3994103046.html) |
| ST7735 SPI | 4.29 USD | [Ali Express](https://ar.aliexpress.com/item/1005010340668858.html?spm=a2g0o.order_list.order_list_main.40.15e2180255B5UM&gatewayAdapt=glo2ara) |
| C3 MINI LOLIN | 14.36 USD | [Ali Express](https://ar.aliexpress.com/item/1005004740051202.html?spm=a2g0o.order_list.order_list_main.4.15e2180255B5UM&gatewayAdapt=glo2ara) |
| BATTERY 3.7V LiPo rechargeable | 2 for 3.9 USD each | (local) |
| TP4056 with protection(MIcro usb-c) | USD 2.32 | [Ali Express](https://ar.aliexpress.com/item/1005007355163444.html?spm=a2g0o.order_list.order_list_main.28.15e2180255B5UM&gatewayAdapt=glo2ara) |
| 6 M3 screws for the screen|depends on your local source | Local bought |
| multimeter to adjust boost converter | 13 USD,(local) | - |
| **TOTAL cost(just the case and componets)** | **40 USD** | - |

# Step by step tutorial/explanation
---
1. upload the code to the esp32, and fill in the internet information, client information from spotify, and the refresh token that you can create, here(https://spotify-refresh-token-generator.netlify.app/#info)
2. solder header pins to the esp32 and the display board, solder the battery to the mt3608 charger module
3. connect the esp32 and display with dupont wires
4. connect the mt3608 to the LED/VCC from display and the 3v3 pin on esp32, this needs to be solered together into a bus, as its 3 wires for one connection, I personally soldered them all to one wire and used that wire to connect to the mt3608 VOUT+
5. use the dupont wires with pins on the other end to connect to the button pins on the esp32, solder the pin end to one end of the key, solder a normal wire with two pins to the other end, as this will be the ground
6. at this point everything is connected if youve gone by the wiring diagram, the code uploaded, and once all the ground wires are connected into one bus, and connected to mt3608 VOUT-, then the display should power on, from here it plays the animation, and displays the song playing, buttons are in the order you arranged them.


# FINAL EXAMPLE IMAGES
---
### the esp32, tp4056 and battery are shown here below all of the messy wiring
<img width="1600" height="721" alt="image" src="https://github.com/user-attachments/assets/47309561-ddeb-4cae-a4ed-05d317fe5405" />

### the front showing the display, and what it is displaying
<img width="381" height="691" alt="image" src="https://github.com/user-attachments/assets/d72bbe22-64b6-47df-a3cc-29a228ee27a7" />

this shows the artist, song, progress of song, and the volume percent, once paused it stops the progress, volume bar changes with the press of the buttons, and it switches every refresh when a new song is played, with previous or next buttons

### the door
<img width="1600" height="721" alt="image" src="https://github.com/user-attachments/assets/be794686-9e0e-4502-b1f0-4250800d49f5" />

the door obviously has space for the two usb ports to work

### the keyswitches on top
<img width="366" height="731" alt="image" src="https://github.com/user-attachments/assets/fcf31d68-0c5c-4640-9cec-a73e2fca116f" />

all 6 keyswitches, play/pause on the bottom right, next middle bottom, previous is bottom left, vol down button on the top right, and vol up button on the top left, of course the orientation of these changes depending on which pin the wire connected is in each keyslot, but the upper middle button does nothing as of now, it is meant to be for an extra purpose, such as on/off, or to toggle the led which can be kept in the heart, additionallyt if put between the power bus or GND bus, it can be used to restart the display, rerunning the animation, thats what it does for me as of now, a reset button

### the esp32, tp4056 and battery meant to be used
<img width="475" height="479" alt="image" src="https://github.com/user-attachments/assets/17d3e2ef-4a24-47d6-ad77-297e98559431" />
<img width="136" height="221" alt="image" src="https://github.com/user-attachments/assets/528e2d56-09c2-45a0-9a53-d17f402559c7" />
<img width="290" height="394" alt="image" src="https://github.com/user-attachments/assets/9ccfd4ff-98c3-4ec0-bc86-f206ceb134bc" />
