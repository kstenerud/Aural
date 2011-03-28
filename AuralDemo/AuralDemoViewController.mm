//
//  AuralDemoViewController.m
//  AuralDemo
//
//  Created by Karl Stenerud on 2/19/11.
//  Copyright 2011 xxx. All rights reserved.
//

#import "AuralDemoViewController.h"
#import "OALAudioSession.h"
#import "OALTools.h"

@implementation AuralDemoViewController

- (void)viewDidLoad
{
    [super viewDidLoad];
	
	[OALAudioSession sharedInstance];
	
	context = manager.newContext();
	context->setSampleRate(44100);
	
	buffer = [KSAudioFile aucBufferWithUrl:[OALTools urlForPath:@"ColdFunk.caf"]
									stereo:NO];
	
	source1 = context->newSource();
	source1->setBuffer(buffer);
	
	source2 = context->newSource();
	source2->setBuffer(buffer);
	
	gainSlider1.value = 1.0f;
	gainSlider2.value = 1.0f;

	rateSlider1.value = 0.5f;
	rateSlider2.value = 0.5f;
    
	pitchSlider1.value = 0.5f;
	pitchSlider2.value = 0.5f;
    
	panSlider1.value = 0.5f;
	panSlider2.value = 0.5f;
    
}

- (void)dealloc
{
    context->deleteSource(source1);
	delete source1;
	delete source2;
	delete context;
	delete buffer;
	
	[gainSlider1 release];
	[gainSlider2 release];
	[rateSlider1 release];
	[rateSlider2 release];
	[pitchSlider1 release];
	[pitchSlider2 release];
	[panSlider1 release];
	[panSlider2 release];

    [super dealloc];
}

@synthesize gainSlider1;
@synthesize gainSlider2;
@synthesize rateSlider1;
@synthesize rateSlider2;
@synthesize pitchSlider1;
@synthesize pitchSlider2;
@synthesize panSlider1;
@synthesize panSlider2;


- (IBAction) onPlay1
{
	source1->play();
}

- (IBAction) onStop1
{
	source1->stop();
}

- (IBAction) onMute1
{
	source1->setMuted(!source1->muted());
}

- (IBAction) onPause1
{
	source1->setPaused(!source1->paused());
}

- (IBAction) onGainSlider1:(id) sender
{
	source1->setGain(gainSlider1.value);
}

- (IBAction) onRateSlider1:(id) sender
{
	source1->setPlaybackRate(rateSlider1.value * 2.0f);
}

- (IBAction) onPitchSlider1:(id) sender
{
	source1->setPitch(pitchSlider1.value * 2.0f);
}

- (IBAction) onPanSlider1:(id) sender
{
	source1->setPan((panSlider1.value - 0.5f) * 2);
}



- (IBAction) onPlay2
{
	source2->play();
}

- (IBAction) onStop2
{
	source2->stop();
}

- (IBAction) onMute2
{
	source2->setMuted(!source2->muted());
}

- (IBAction) onPause2
{
	source2->setPaused(!source2->paused());
}

- (IBAction) onGainSlider2:(id) sender
{
	source2->setGain(gainSlider2.value);
}

- (IBAction) onRateSlider2:(id) sender
{
	source2->setPlaybackRate(rateSlider2.value * 2.0f);
}

- (IBAction) onPitchSlider2:(id) sender
{
	source2->setPitch(pitchSlider2.value * 2.0f);
}

- (IBAction) onPanSlider2:(id) sender
{
	source2->setPan((panSlider2.value - 0.5f) * 2);
}



- (IBAction) onContext
{
	context->setActive(!context->active());
}

@end
