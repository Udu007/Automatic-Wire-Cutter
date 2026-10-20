# Automatic Wire Cutter and Stripper

A desktop machine that **feeds, strips and cuts wire to length**. You enter the strip length, the wire length and the number of pieces from a phone, and the machine produces the batch on its own. Two stepper motors and a servo do the mechanical work, driven by an **Arduino Nano**. An **ESP32** connects the machine to the **Blynk IoT** app over Wi-Fi.

All the mechanical parts were designed in **Fusion 360** and 3D-printed.

---

## How it works

```
 Blynk app (phone) ──Wi-Fi──► ESP32 ──Serial "A,B,C"──► Arduino Nano ──► stepper X (wire feed)
                                                                   ├──► stepper Y (blade)
                                                                   └──► servo (blade guide)
```

The user sets three values:

| Value | Meaning |
|-------|---------|
| **A** | strip length at each end of the wire |
| **B** | length of the wire between the stripped ends |
| **C** | number of pieces to make |

For each of the **C** pieces, the Nano runs this cycle:

1. Feed **A** of wire, then turn the blade stepper one revolution to strip.
2. Feed **B** of wire, then strip again.
3. Feed **A** of wire.
4. Move the servo guide to the cutting position and turn the blade stepper to cut the wire.
5. Move the servo back to the stripping position.

The feed stepper converts length to motor steps with a calibrated factor of 4.29 steps per unit length, using 1/16 microstepping.

**Emergency stop:** the Nano keeps listening for a `STOP` command during the whole sequence. Motor moves are broken into small chunks and every delay checks the serial port, so a stop takes effect almost immediately.

## Repository structure

```
Code/
├── Arduino Nano Code.ino   # Motion control: parses A,B,C / STOP and runs the strip-and-cut sequence
└── ESP32 Code.ino          # Blynk IoT interface: reads A, B, C from the app and sends them on button press
Hardware/
├── Full Project.f3d        # Complete Fusion 360 assembly
├── *.f3d                   # Editable Fusion 360 parts (stands, servo mount, base)
├── *.stl                   # Printable parts: feeder shaft, wire puller, cutter, servo guide, SMPS stand, supports
└── Servo Guide Drawing.pdf # Dimensioned drawing of the servo guide
```

## Hardware

| Component | Role |
|-----------|------|
| Arduino Nano | Real-time control of the motors and servo |
| ESP32 | Wi-Fi link to the Blynk app |
| 2 × NEMA-style stepper motors (200 steps/rev) with step/dir drivers | Wire feed (X) and blade drive (Y) |
| Servo motor | Moves the blade guide between strip and cut positions |
| SMPS power supply | Powers the motors |
| 3D-printed parts | Feeder shaft, wire puller, cutter stands, servo guide, base |

**Pin mapping (Nano):**

| Function | Pins |
|----------|------|
| X stepper STEP/DIR | D4 / D7 |
| Y stepper STEP/DIR | D3 / D6 |
| Servo | D9 |
| ESP32 serial (SoftwareSerial RX/TX) | D2 / D3 |

## Getting started

1. Install the Arduino libraries:
   - [`StepperDriver`](https://github.com/laurb9/StepperDriver), which provides `BasicStepperDriver` and `SyncDriver`
   - `Servo`
   - [`Blynk`](https://github.com/blynkkk/blynk-library), for the ESP32
2. In the Blynk console, create a template with integer datastreams **V1** (A), **V2** (B) and **V3** (C), and a button on **V0** (Send).
3. Put your own Blynk template ID, auth token and Wi-Fi credentials into `ESP32 Code.ino`, then flash it to the ESP32.
4. Flash `Arduino Nano Code.ino` to the Nano and connect the ESP32's TX to the Nano's RX (D2), with a common ground.
5. Print the STL files, assemble the machine, enter A, B and C in the app and press **Send**.

## Status and known issues

- The ESP32 sketch currently prints the values on button press. The line that forwards `A,B,C` to the Nano is still commented out (`sendAllValues()`), and there is no `STOP` button yet. Both need finishing for full wireless control.
- The Nano uses **D3** both as the Y-stepper STEP pin and as SoftwareSerial TX. Moving one of them to a free pin will avoid conflicts.

## Possible improvements

- Add a STOP button in the app and forward it to the Nano.
- Add a wire-detect sensor so the machine stops automatically when the spool runs out.
- Report progress (pieces done) back to the app.
