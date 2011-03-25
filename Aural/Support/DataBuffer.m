//
//  DataBuffer.m
//  Aural
//
//  Created by Karl Stenerud on 2/27/11.
//  Copyright 2011 xxx. All rights reserved.
//

#import "DataBuffer.h"


@implementation DataBuffer

+ (DataBuffer*) bufferWithLength:(unsigned int) numBytes
{
	return [[[self alloc] initWithLength:numBytes] autorelease];
}

+ (DataBuffer*) bufferWithLength:(unsigned int) numBytes
				   freeOnDealloc:(bool) freeOnDealloc
{
	return [[[self alloc] initWithLength:numBytes freeOnDealloc:freeOnDealloc] autorelease];
}

+ (DataBuffer*) bufferWithData:(void*) data
					  numBytes:(unsigned int) numBytes
				 freeOnDealloc:(bool) freeOnDealloc
{
	return [[[self alloc] initWithData:data
							  numBytes:numBytes
						 freeOnDealloc:freeOnDealloc] autorelease];
}

- (id) initWithLength:(unsigned int) numBytes
{
	return [self initWithLength:numBytes freeOnDealloc:YES];
}

- (id) initWithLength:(unsigned int) numBytes
		freeOnDealloc:(bool) freeOnDealloc
{
	if(nil != (self = [super init]))
	{
		_data = malloc(numBytes);
		if(nil == _data)
		{
			NSLog(@"Error: Could not allocate %d bytes of memory", numBytes);
			[self release];
			return nil;
		}
		_numBytes = numBytes;
		_freeOnDealloc = freeOnDealloc;
	}
	return self;
}

- (id) initWithData:(void*) data
		   numBytes:(unsigned int) numBytes
	  freeOnDealloc:(bool) freeOnDealloc
{
	if(nil != (self = [super init]))
	{
		_data = data;
		_numBytes = numBytes;
		_freeOnDealloc = freeOnDealloc;
	}
	return self;
}

- (void) dealloc
{
	if(nil != _data && _freeOnDealloc)
	{
		free(_data);
	}
	[super dealloc];
}

@synthesize data = _data;
@synthesize numBytes = _numBytes;
@synthesize freeOnDealloc = _freeOnDealloc;

@end
