# Waffle_sw

Software for Waffle, a compact Pager and general RF multitool.

## Table of Contents

- [Compiling & Installing](#compiling--installing)
  - [```config.json```](#configjson)
  - [TTGO LoRa32](#ttgo-lora32)
- [Contributing](#contributing)
- [License & Acknowledgements](#license--acknowledgements)

## Compiling & Installing

### ```config.json```

In the ```waffle_platformio/data``` folder, there's a ```config.json``` file that you can use to configure the device.

```json
{
  "CALLSIGN": "ON4PFD",
  "XTIME_ADDRESS": 208,
  "ADDRESSES" : [ [100, 1],
                  [420, 2],
                  [-1, -1],
                  [-1, -1],
                  [-1, -1],
                  [-1, -1],
                  [-1, -1],
                  [-1, -1],
                  [-1, -1],
                  [-1, -1]
                ]
}
```

- ```CALLSIGN``` : Your callsign, will be displayed on the screen.
- ```XTIME_ADDRESS``` : The address of the XTIME POCSAG messages, leave it at 208 for DAPNET.
- ```ADDRESSES``` : The addresses of the POCSAG messages, the first element is the address, the second is the alert priority (-1: ignore, 0: store, 1: vibrate+screen, 2: buzzer+vibrate+screen).  
  You can add up to 10 RICs, or leave everything at -1 to ignore them.

### TTGO LoRa32

Clone the repo and install the submodules :

```bash
git clone https://github.com/Jankyneering/Waffle_sw.git
cd Waffle_sw
git submodule update --init --recursive
```

Open the ```waffle_platformio``` folder in PlatformIO compile and upload the project.  
You'll also need to upload the SPIFFS image to the board, you can do this by running the following command :

```bash
pio run -t uploadfs
```

## Contributing

Thank you for your interest ! Please fork the repository and submit a pull request with a branch named after the feature you are working on.  
Please make sure to follow the coding style and conventions of the project, a clang format setting for VSCode is provided below :

VSCode ```C_Cpp.clang_format_fallbackStyle```  
Settings : ```{ BasedOnStyle: LLVM, UseTab: Never, IndentWidth: 4, TabWidth: 4, AllowShortIfStatementsOnASingleLine: true, IndentCaseLabels: false, ColumnLimit: 0, AccessModifierOffset: -4, NamespaceIndentation: All, FixNamespaceComments: false, AlignConsecutiveAssignments: AcrossEmptyLines}```

## License & Acknowledgements

Made with ❤️, lots of ☕️, and lack of 🛌

[![License: GPL v3](https://www.gnu.org/graphics/gplv3-127x51.png)](https://www.gnu.org/licenses/gpl-3.0.en.html)  
[GNU GPLv3](https://www.gnu.org/licenses/gpl-3.0.en.html)
