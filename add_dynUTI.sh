#!/usr/bin/env bash
# add the dynamic UTI to the installed Quicklook generator
# ref: https://github.com/whomwah/qlstephen/issues/87#issuecomment-773664993

if [ $# -lt 2 ]; then
	echo "Adds the dynamic UTI to the target Quicklook generator" 2>&1
	echo "" 2>&1
	echo "usage:" 2>&1
	echo "    $0 ANY_APD_FILE QLGENERATOR_FILE" 2>&1
	echo "" 2>&1
	echo "example:" 2>&1
	echo "    $0 ./testv4.apd ~/Library/QuickLook/QuickLookAzpainter.qlgenerator" 2>&1
	exit 1
fi

type=`/usr/bin/mdls -name kMDItemContentType "$1" | sed -n 's/^kMDItemContentType = \"\(.*\)\"$/\1/p'`
plutil -insert CFBundleDocumentTypes.0.LSItemContentTypes.0 -string "$type" "$2"/Contents/Info.plist
qlmanage -r
