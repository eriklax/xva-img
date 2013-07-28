xvatool
=======

XVA (Cirtix Xen format) virtual disk tool. Supports RAW disk image exports and imports. Citrix Xen uses a custom virtual appliance format for import/export called "XVA". it's basically a strangely crafted tar-file. You don't need this program to unpack this tar-file, just use your favourite tar unpacker (tar, gtar, bsdtar). Once unpacked you will end up with a lot of different files, ova.xml (which contains the settings for the virtual appliance, think VMware vmx) and a number of folders called Ref:<number>/, this is your disks. Each of these folders contain hundreds of files named 00000000, 00000001 with a accompanying .CHECKSUM file (SHA1). Each file is a 1MB slice of the disk, but some of the files in the sequence will probably be missing this is because XVA do not use compression; instead it will exclude slices of the disk that only contains zeros (are empty). This tool can assemble the disk for you (you will end up with a RAW disk) that can easily be mounted and modified. It can then also split the file again and generate checksum. Once ready, you will probably want to use the "package" command to rebuild the XVA file.

Example
=======
Extract the XVA file.

 # mkdir my-virtual-machine
 # tar -xf my-virtual-machine.xva -C my-virtual-machine
 # chmod -R 755 my-virtual-machine

Extract one of the disks

 # xvatool -p disk-export my-virtual-machine/Ref\:1/ disk.raw

Splice the disk file (remove all files in the folder, required not to make any unnecessary mistakes)

 # rm -rf my-virtual-machine/Ref\:1/*
 # xvatool -p disk-import my-virtual-machine/Ref\:1/ disk.raw

Rebuild the XVA file

 # xvatool -p package my-virtual-machine-fixed.xva my-virtual-machine/ova.xml my-virtual-machine/Ref\:1/
