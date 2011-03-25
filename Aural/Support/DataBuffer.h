//
//  DataBuffer.h
//  Aural
//
//  Created by Karl Stenerud on 2/27/11.
//  Copyright 2011 xxx. All rights reserved.
//


@interface DataBuffer : NSObject
{
	void* _data;
	unsigned int _numBytes;
	bool _freeOnDealloc;
}

@property(readonly) void* data;
@property(readonly) unsigned int numBytes;
@property(readwrite) bool freeOnDealloc;


+ (DataBuffer*) bufferWithLength:(unsigned int) numBytes;

+ (DataBuffer*) bufferWithLength:(unsigned int) numBytes
				   freeOnDealloc:(bool) freeOnDealloc;

+ (DataBuffer*) bufferWithData:(void*) data
					  numBytes:(unsigned int) numBytes
				 freeOnDealloc:(bool) freeOnDealloc;

- (id) initWithLength:(unsigned int) numBytes;

- (id) initWithLength:(unsigned int) numBytes
		freeOnDealloc:(bool) freeOnDealloc;

- (id) initWithData:(void*) data
		   numBytes:(unsigned int) numBytes
	  freeOnDealloc:(bool) freeOnDealloc;

@end
