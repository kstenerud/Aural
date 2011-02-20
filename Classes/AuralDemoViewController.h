//
//  AuralDemoViewController.h
//  AuralDemo
//
//  Created by Karl Stenerud on 2/19/11.
//  Copyright 2011 xxx. All rights reserved.
//

#import <UIKit/UIKit.h>
#import "Aural.h"

@interface AuralDemoViewController : UIViewController
{
	AUCAudioContext* context;
	AUCAudioBuffer* buffer;
	AUCAudioSource* source1;
	AUCAudioSource* source2;
	
	IBOutlet UISlider *gainSlider1;
	IBOutlet UISlider *gainSlider2;
}

@property (nonatomic, retain) IBOutlet UISlider *gainSlider1;
@property (nonatomic, retain) IBOutlet UISlider *gainSlider2;

- (IBAction) onPlay1;
- (IBAction) onStop1;
- (IBAction) onMute1;
- (IBAction) onPause1;
- (IBAction) onGainSlider1:(id) sender;

- (IBAction) onPlay2;
- (IBAction) onStop2;
- (IBAction) onMute2;
- (IBAction) onPause2;
- (IBAction) onGainSlider2:(id) sender;

- (IBAction) onContext;

@end

