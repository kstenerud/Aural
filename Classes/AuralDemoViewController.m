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
}

- (void)dealloc
{
	source1->destroy(source1);
	source2->destroy(source2);
	context->destroy(context);
	buffer->destroy(buffer);
	
    [super dealloc];
}

@synthesize gainSlider1;
@synthesize gainSlider2;


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



- (IBAction) onContext
{
	context->setActive(context, !context->getActive(context));
}

@end
