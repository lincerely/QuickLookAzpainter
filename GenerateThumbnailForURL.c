#include <CoreFoundation/CoreFoundation.h>
#include <CoreServices/CoreServices.h>
#include <QuickLook/QuickLook.h>
#include "apd.h"

OSStatus GenerateThumbnailForURL(void *thisInterface, QLThumbnailRequestRef thumbnail, CFURLRef url, CFStringRef contentTypeUTI, CFDictionaryRef options, CGSize maxSize);
void CancelThumbnailGeneration(void *thisInterface, QLThumbnailRequestRef thumbnail);

/* -----------------------------------------------------------------------------
 Generate a thumbnail for file
 
 This function's job is to create thumbnail for designated file as fast as possible
 ----------------------------------------------------------------------------- */

OSStatus GenerateThumbnailForURL(void *thisInterface, QLThumbnailRequestRef thumbnail, CFURLRef url, CFStringRef contentTypeUTI, CFDictionaryRef options, CGSize maxSize)
{
    CFStringRef rawPath = CFURLCopyPath(url);
    CFStringRef path = CFURLCreateStringByReplacingPercentEscapes(kCFAllocatorDefault, rawPath, CFSTR(""));
    CFRelease(rawPath);
    CFIndex length  = CFStringGetLength(path);
    CFIndex max = CFStringGetMaximumSizeForEncoding(length, kCFStringEncodingUTF8);
    
    char* c_path = (char*)malloc(max);
    CFStringGetCString(path, c_path, max, kCFStringEncodingUTF8);
    CFRelease(path);
    
    aPicture picture;
	
	int ret = load_picture(c_path, &picture);
	free(c_path);
	if (ret != 0) {
		return noErr;
	}
	
	int samples = 3;
	CGBitmapInfo bitmapInfo = kCGBitmapByteOrderDefault;
	CGDataProviderRef provider = CGDataProviderCreateWithData(NULL, picture.rgbdata, picture.width*picture.height*samples, NULL);
	CGColorSpaceRef deviceRGBColorSpace = CGColorSpaceCreateDeviceRGB();
	CGImageRef image =  CGImageCreate(picture.width, picture.height, 8, 8 * samples, picture.width * samples, deviceRGBColorSpace, bitmapInfo, provider, NULL, false, kCGRenderingIntentDefault);
	
	QLThumbnailRequestSetImage(thumbnail, image, NULL);
	
	CGDataProviderRelease(provider);
	CGColorSpaceRelease(deviceRGBColorSpace);
	CGImageRelease(image);
	free(picture.rgbdata);
	
    return noErr;
}

void CancelThumbnailGeneration(void *thisInterface, QLThumbnailRequestRef thumbnail)
{
    // Implement only if supported
}
