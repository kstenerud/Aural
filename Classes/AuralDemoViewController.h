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
}

- (IBAction) onPlay1;
- (IBAction) onStop1;
- (IBAction) onMute1;
- (IBAction) onPause1;

- (IBAction) onPlay2;
- (IBAction) onStop2;
- (IBAction) onMute2;
- (IBAction) onPause2;

- (IBAction) onContext;

@end

