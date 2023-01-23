# QuickLookAzpainter

Quicklook preview generator for Azpainter data.

Currently support `APD` version 3 and 4.

This program generates preview by extracting the embedded single picture image from the APD. This embedding should be enabled by default. 

To check your settings, open menu "Setting > Environmental setting > Flags", make sure the flag "Do not write a single picture image when saving APD" is off.

## Install

### Install prebuilt binary

Download lastest release.zip from [Github's release page](https://github.com/lincerely/QuickLookAzpainter/releases).

Then, unzip it and open `install.command` (or right-click -> open, if it can't be opened because of unidentified developer).

### Build from source

Just build from Xcode, `QuickLookAzpainter.qlgenerator` would be copied to `~/Library/QuickLook/` automatically. After that, a bash script [add_dynUTI.sh](./add_dynUTI.sh) would run to determine appropriate dynamic UTI and added to the installed generator.

## Related Links

- [Azpainter Homepage](http://azsky2.html.xdomain.jp/soft/azpainter.html)
- [Hexfiend binary template for APD v4](https://github.com/lincerely/HexFiend_templates/blob/master/APDv4.tcl)
- [Azpainterb Hompage](http://azsky2.html.xdomain.jp/soft/azpainterb.html)

## License

See [LICENSE](./LICENSE)
