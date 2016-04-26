# Hex
Hex is basic C utility to convert between base 10 and base 16.

It is hardcoded to convert from base 10 to 16 or base 16 to 10, depending on compiler flags.

It generates two executables 'hexc' and 'decc', which convert *to* the base in the name.

##Usage:
```
./hexc \<values to convert\>
./decc \<values to convert\>
```

##Examples:
````
./hexc 10 20 255

10  =>  0xa
20  => 0x14
255 => 0xff
```

```
./decc ff 0x1234 0012

ff     =>     255
0x1234 =>    4660
0012   =>      18
```
