//
//  AuralDemoAppDelegate.h
//  AuralDemo
//
//  Created by Karl Stenerud on 2/19/11.
//  Copyright 2011 xxx. All rights reserved.
//

#import <UIKit/UIKit.h>

@class AuralDemoViewController;

@interface AuralDemoAppDelegate : NSObject <UIApplicationDelegate> {
    UIWindow *window;
    AuralDemoViewController *viewController;
}

@property (nonatomic, retain) IBOutlet UIWindow *window;
@property (nonatomic, retain) IBOutlet AuralDemoViewController *viewController;

@end

