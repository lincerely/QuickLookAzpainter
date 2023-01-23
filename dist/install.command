#!/usr/bin/env bash

(
	cd `/usr/bin/dirname $0`
	
	echo "remove quarantine flag from the generator"
	xattr -d -r "com.apple.quarantine" ./QuickLookAzpainter.qlgenerator
	
	echo "insert dynamic UTI to the generator"
	./add_dynUTI.sh ./test.apd ./QuickLookAzpainter.qlgenerator
	
	echo "install as ~/Library/QuickLook/QuickLookAzpainter.qlgenerator"
	cp -r ./QuickLookAzpainter.qlgenerator ~/Library/QuickLook/
	
	echo "script completed"
	read -p 'press enter to exit'
)
