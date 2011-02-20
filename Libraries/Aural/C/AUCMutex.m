//
//  AUCMutex.m
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

#import "AUCMutex.h"
#import "AUCLogging.h"

BOOL auc_mutex_initialize(pthread_mutex_t* mutex)
{
	pthread_mutexattr_t attrib;
	
	if(0 != pthread_mutexattr_init(&attrib))
	{
		AUC_LOG_ERROR("Could not initialize pthread attribute");
		return FALSE;
	}
	
	if(0 != pthread_mutexattr_settype(&attrib, PTHREAD_MUTEX_RECURSIVE))
	{
		AUC_LOG_ERROR("Could not set pthread attribute");
		pthread_mutexattr_destroy(&attrib);
		return FALSE;
	}
	
	if(0 != pthread_mutex_init(mutex, &attrib))
	{
		AUC_LOG_ERROR("Could not initialize mutex");
		pthread_mutexattr_destroy(&attrib);
		return FALSE;
	}
	
	pthread_mutexattr_destroy(&attrib);
	return TRUE;
}

BOOL auc_mutex_destroy(pthread_mutex_t* mutex)
{
	if(0 != pthread_mutex_destroy(mutex))
	{
		AUC_LOG_ERROR("Could not destroy mutex: mutex is busy");
		return FALSE;
	}
	return TRUE;
}
