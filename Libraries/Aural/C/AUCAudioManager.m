//
//  AUCAudioManager.m
//  Aural
//
//  Created by Karl Stenerud on 2/19/11.
//
// Copyright 2011 Karl Stenerud
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
// http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//
// Note: You are NOT required to make the license available from within your
// iOS application. Including it in your project is sufficient.
//
// Attribution is not required, but appreciated :)
//

#import "AUCAudioManager.h"
#import "AUCMacros.h"
#import "AUCAudioContext.h"


#pragma mark -
#pragma mark Forward Declarations

static AUCAudioManager* AUCAudioManager_create();
static void self_destroy(AUCAudioManager* self);
static AUCAudioContext* self_createContext(AUCAudioManager* self);



#pragma mark -
#pragma mark Globals

static AUCAudioManager* g_sharedManager;



#pragma mark -
#pragma mark Methods

AUCAudioManager* AUCAudioManager_shared()
{
	if(NULL == g_sharedManager)
	{
		g_sharedManager = AUCAudioManager_create();
	}
	return g_sharedManager;
}

static AUCAudioManager* AUCAudioManager_create()
{
	// Creation
	AUCAudioManager* self = ALLOC_OBJECT(AUCAudioManager);
	if(NULL == self)
	{
		return NULL;
	}
	
	// Method Assignment
	self->destroy = self_destroy;
	self->createContext = self_createContext;

	return self;
}

static void self_destroy(AUCAudioManager* self)
{
	FREE_OBJECT(self);
}

static AUCAudioContext* self_createContext(AUCAudioManager* self)
{
	return AUCAudioContext_create(self);
}
