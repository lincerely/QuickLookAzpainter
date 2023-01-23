#include <CoreFoundation/CoreFoundation.h>
#include <CoreServices/CoreServices.h>
#include <QuickLook/QuickLook.h>
#include "apd.h"

OSStatus GeneratePreviewForURL(void *thisInterface, QLPreviewRequestRef preview, CFURLRef url, CFStringRef contentTypeUTI, CFDictionaryRef options);
void CancelPreviewGeneration(void *thisInterface, QLPreviewRequestRef preview);

/* -----------------------------------------------------------------------------
 Generate a preview for file
 
 This function's job is to create preview for designated file
 ----------------------------------------------------------------------------- */

OSStatus GeneratePreviewForURL(void *thisInterface, QLPreviewRequestRef preview, CFURLRef url, CFStringRef contentTypeUTI, CFDictionaryRef options)
{
    CFStringRef rawPath = CFURLCopyPath(url);
    CFStringRef path = CFURLCreateStringByReplacingPercentEscapes(kCFAllocatorDefault, rawPath, CFSTR(""));
    CFRelease(rawPath);
    CFIndex length  = CFStringGetLength(path);
    CFIndex maxSize = CFStringGetMaximumSizeForEncoding(length, kCFStringEncodingUTF8);
    
    char* c_path = (char*)malloc(maxSize);
    CFStringGetCString(path, c_path, maxSize, kCFStringEncodingUTF8);
    CFRelease(path);
	
	aPicture picture;
	
	int ret = load_picture(c_path, &picture);
	free(c_path);
	if (ret != 0) {
		return noErr;
	}
	
	int samples = 3;
	CGBitmapInfo bitmapInfo = kCGBitmapByteOrderDefault;
	CGSize imageSize = CGSizeMake(picture.width, picture.height);
	CGContextRef ctx = QLPreviewRequestCreateContext(preview, imageSize, true, options);
	if(!ctx) {
		free(picture.rgbdata);
		return noErr;
	}
	
	CGDataProviderRef provider = CGDataProviderCreateWithData(NULL, picture.rgbdata, picture.width*picture.height*samples, NULL);
	
	CGColorSpaceRef deviceRGBColorSpace = CGColorSpaceCreateDeviceRGB();
	CGImageRef image =  CGImageCreate(picture.width, picture.height, 8, 8 * samples, picture.width * samples, deviceRGBColorSpace, bitmapInfo, provider, NULL, false, kCGRenderingIntentDefault);
	
	CGContextDrawImage(ctx, CGRectMake(0, 0, picture.width, picture.height), image);
	
	CGDataProviderRelease(provider);
	CGColorSpaceRelease(deviceRGBColorSpace);
	CGImageRelease(image);
	free(picture.rgbdata);
	
	CGContextFlush(ctx);
	QLPreviewRequestFlushContext(preview, ctx);
	CFRelease(ctx);
    
	return noErr;
}

void CancelPreviewGeneration(void *thisInterface, QLPreviewRequestRef preview)
{
    // Implement only if supported
}
