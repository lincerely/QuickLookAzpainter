# QuickLookAzpainter

Quicklook preview generator for Azpainter data.

Currently only support `APD` version 4.

This program generates preview by extracting the embedded single picture image from the APD. This embedding should be enabled by default. 

To check your settings, open menu "Setting > Environmental setting > Flags", make sure the flag "Do not write a single picture image when saving APD" is off.

## Install

Just build from Xcode, `QuickLookAzpainter.qlgenerator` would be copied to `~/Library/QuickLook/` automatically. After that, a bash script [add_dynUTI.sh](./add_dynUTI.sh) would run to determine appropriate dynamic UTI and added to the installed generator.

## Related Links

- [Azpainter Homepage](http://azsky2.html.xdomain.jp/soft/azpainter.html)
- [Hexfiend binary template for APD v4](https://github.com/lincerely/HexFiend_templates/blob/master/APDv4.tcl)

## License

See [LICENSE](./LICENSE)