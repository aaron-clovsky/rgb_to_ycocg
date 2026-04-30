# RGB <-> YCoCg Conversion Implementation and Test

## MOTIVATION

While YCoCg seems to be the modern standard for Luma + Chrominance encoding, finding an integer based encoder/decoder implementation that actually works when tested is harder than it should be. The [original paper](https://www.microsoft.com/en-us/research/wp-content/uploads/2016/06/Malvar_Sullivan_YCoCg-R_JVT-I014r3-2.pdf) is also not very clear on some important points which make implementation in all-integer situations (especially important for an HDL implementation) difficult.

Hopefully this helps anyone trying to implement YUV 4:2:2 decompression to RGB in FPGA.

## CREDIT

The authors of the [original paper](https://www.microsoft.com/en-us/research/wp-content/uploads/2016/06/Malvar_Sullivan_YCoCg-R_JVT-I014r3-2.pdf).
 
## BUILD INSTRUCTIONS

- This is intended to be built and run on Linux
- Install a C compiler if one is not present
  (Run ```sudo apt install build-essential``` on Debian-based distros)
- Install Icarus Verilog
  (Run ```sudo apt install iverilog``` on Debian-based distros)
- ```cd``` to this repo's directory
- Run ```make``` to build the program ```rgb_to_ycocg``` and the Verilog simulation ```sim_rgb_to_ycocg```
- Run ```make test``` or ```test.sh``` to build and validate C vs Verilog output

## SYNOPSIS

```
rgb_to_ycocg <MODE> [OPTIONS]
 modes: RGB | YCoCg | analyze | all | allRGB | allYCoCg"
  RGB Options: <R> <G> <B>
  YCoCg Options: <Y> <Co> <Cg>
```

## COMMAND-LINE OPTIONS


```RGB```
Converts a 24-bit RGB value to a 24-bit YCoCg value

```YCoCg```
Converts a 24-bit YCoCg value to a 24-bit RGB value

```analyze```
Examines every possible 24-bit RGB value, converts each value to 24-bit YCoCg and then converts each value back to 24-bit RGB and gathers statistics on conversion errors

```all```
Prints the following in 9 column CSV output:
Columns 1-3: Every possible 24-bit RGB value
Columns 4-6: The 24-bit YCoCg conversion of the RGB values in columns 1-3
Columns 7-9: The 24-bit RGB conversion of the YCoCg values in columns 4-6

```allRGB```
Prints the following in 6 column CSV output:
Columns 1-3: Every possible 24-bit RGB value
Columns 4-6: The 24-bit YCoCg conversion of the RGB values in columns 1-3

```allYCoCg```
Prints the following in 6 column CSV output:
Columns 1-3: Every possible 24-bit YCoCg value
Columns 4-6: The 24-bit RGB conversion of the YCoCg values in columns 4-6

## NOTES

- ```sim_rgb_to_ycocg``` should produce identical output to ```rgb_to_ycocg allYCoCg``` (this is what ```test.sh``` verifies)


## MATH NOTES

- The decoding formulas: 
	- *G = Y + Cg* 
	- *R = Y – Cg + Co*
	- *B = Y – Cg – Co* 
	
	Require the following to work when Y, Co and Cg are each 8 bits: 
    - Y is treated as unsigned
    - Co and Cg are treated as signed
    - At least 10-bit precision is used for addition and subtraction operations

- The encoding formulas as they appear in the paper: 
	- *Y = G / 2 + (R + B) / 4* 
	- *Co = R - B* 
	- *Cg = G - (R + B) / 2* 
	
	Are not usable as written, because the next sentence says: 
    
    "which is identical to the original definition, but with Co and Cg scaled up by a factor of two"  
    
    Which means we really have:
	- *Y = G / 2 + (R + B) / 4* 
	- *Co = (R - B) / 2* 
	- *Cg = G / 2 - (R + B) / 4* 
	
    While these work, they don't work as well as they could when doing integer divides because of chop rounding, to fix this we make the following changes:
    - *Y = (R + (G * 2) + B + 2) / 4* 
	- *Co = (R - B) / 2* 
	- *Cg = ((G * 2) - R - B + 2) / 4* 

	This fixes nearly all of the problems of doing the calculations as integers but leaves eactly one glaring outlier: RGB(0, 255, 0) becomes YCoCg(128,0,128) which in turn converts back to RGB(255, 0, 255), which is an especially bad result and is fixed by making the following adjustment:
    - *Y = (R + (G * 2) + B + 1) / 4* 
	- *Co = (R - B) / 2* 
	- *Cg = ((G * 2) - R - B + 1) / 4* 
	
	This results in RGB(0, 255, 0) becoming YCoCg(127,0,127) which in turn converts back to RGB(0, 254, 0), which is a much better result and doesn't seem to have any other negative effects overall

- The accuracy of these conversion functions is such that after converting *RGB->YCoCg->RGB* with an output precision of 24-bits at each step ~25% of the doubly converted RGB values are identical to the original values and the remaining ~75% differ from the original by exactly one in exactly one color channel


## LICENSE
This software is licensed under the
[MIT License](https://opensource.org/licenses/MIT).
