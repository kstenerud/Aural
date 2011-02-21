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
	AUCAudioManager* manager = AUCAudioManager_shared();
	
	context = manager->createContext(manager);
	context->setSampleRate(context, 44100);
	
	buffer = [KSAudioFile aucBufferWithUrl:[OALTools urlForPath:@"ColdFunk.caf"]
									stereo:YES];
	
	source1 = context->createSource(context);
	source1->setBuffer(source1, buffer);
	
	source2 = context->createSource(context);
	source2->setBuffer(source2, buffer);
	
	gainSlider1.value = 1.0f;
	gainSlider2.value = 1.0f;

	pitchSlider1.value = 0.5f;
	pitchSlider2.value = 0.5f;

}

- (void)dealloc
{
	source1->destroy(source1);
	source2->destroy(source2);
	context->destroy(context);
	buffer->destroy(buffer);
	
	[gainSlider1 release];
	[gainSlider2 release];
	[pitchSlider1 release];
	[pitchSlider2 release];

    [super dealloc];
}

@synthesize gainSlider1;
@synthesize gainSlider2;
@synthesize pitchSlider1;
@synthesize pitchSlider2;
@synthesize panSlider1;
@synthesize panSlider2;


- (IBAction) onPlay1
{
	source1->play(source1);
}

- (IBAction) onStop1
{
	source1->stop(source1);
}

- (IBAction) onMute1
{
	source1->setMuted(source1, !source1->getMuted(source1));
}

- (IBAction) onPause1
{
	source1->setPaused(source1, !source1->getPaused(source1));
}

- (IBAction) onGainSlider1:(id) sender
{
	source1->setGain(source1, gainSlider1.value);
}

- (IBAction) onPitchSlider1:(id) sender
{
	source1->setPitch(source1, pitchSlider1.value * 2.0f);
}

- (IBAction) onPanSlider1:(id) sender
{
	source1->setPan(source1, (panSlider1.value - 0.5f) * 2);
}



- (IBAction) onPlay2
{
	source2->play(source2);
}

- (IBAction) onStop2
{
	source2->stop(source2);
}

- (IBAction) onMute2
{
	source2->setMuted(source2, !source2->getMuted(source2));
}

- (IBAction) onPause2
{
	source2->setPaused(source2, !source2->getPaused(source2));
}

- (IBAction) onGainSlider2:(id) sender
{
	source2->setGain(source2, gainSlider2.value);
}

- (IBAction) onPitchSlider2:(id) sender
{
	source2->setPitch(source2, pitchSlider2.value * 2.0f);
}

- (IBAction) onPanSlider2:(id) sender
{
	source2->setPan(source2, (panSlider2.value - 0.5f) * 2);
}



- (IBAction) onContext
{
	context->setActive(context, !context->getActive(context));
}

@end
