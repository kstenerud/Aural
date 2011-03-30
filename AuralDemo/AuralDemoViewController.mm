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
	
	environment = manager.newEnvironment();
	environment->setSampleRate(44100);
	
	buffer = [KSAudioFile audioDataWithUrl:[OALTools urlForPath:@"ColdFunk.caf"]
                                    stereo:NO];
	
	emitter1 = environment->newEmitter();
	emitter1->setAudioData(buffer);
	
	emitter2 = environment->newEmitter();
	emitter2->setAudioData(buffer);
	
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
    environment->deleteEmitter(emitter1);
    environment->deleteEmitter(emitter2);
    manager.deleteEnvironment(environment);
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
	emitter1->play();
}

- (IBAction) onStop1
{
	emitter1->stop();
}

- (IBAction) onMute1
{
	emitter1->setMuted(!emitter1->muted());
}

- (IBAction) onPause1
{
	emitter1->setPaused(!emitter1->paused());
}

- (IBAction) onGainSlider1:(id) sender
{
	emitter1->setGain(gainSlider1.value);
}

- (IBAction) onRateSlider1:(id) sender
{
	emitter1->setPlaybackRate(rateSlider1.value * 2.0f);
}

- (IBAction) onPitchSlider1:(id) sender
{
	emitter1->setPitch(pitchSlider1.value * 2.0f);
}

- (IBAction) onPanSlider1:(id) sender
{
	emitter1->setPan((panSlider1.value - 0.5f) * 2);
}



- (IBAction) onPlay2
{
	emitter2->play();
}

- (IBAction) onStop2
{
	emitter2->stop();
}

- (IBAction) onMute2
{
	emitter2->setMuted(!emitter2->muted());
}

- (IBAction) onPause2
{
	emitter2->setPaused(!emitter2->paused());
}

- (IBAction) onGainSlider2:(id) sender
{
	emitter2->setGain(gainSlider2.value);
}

- (IBAction) onRateSlider2:(id) sender
{
	emitter2->setPlaybackRate(rateSlider2.value * 2.0f);
}

- (IBAction) onPitchSlider2:(id) sender
{
	emitter2->setPitch(pitchSlider2.value * 2.0f);
}

- (IBAction) onPanSlider2:(id) sender
{
	emitter2->setPan((panSlider2.value - 0.5f) * 2);
}



- (IBAction) onEnvironment
{
	environment->setActive(!environment->active());
}

@end
