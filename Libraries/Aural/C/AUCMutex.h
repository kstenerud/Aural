//
//  AUCMutex.h
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

#import <Foundation/Foundation.h>
#include <pthread.h>


#define AUC_MUTEX_INIT(MUTEX) auc_mutex_initialize(MUTEX)
#define AUC_MUTEX_DESTROY(MUTEX) auc_mutex_destroy(MUTEX)

#define AUC_MUTEX_LOCK(MUTEX) pthread_mutex_lock(MUTEX)
#define AUC_MUTEX_TRY_LOCK(MUTEX) (0 != pthread_mutex_trylock(MUTEX))
#define AUC_MUTEX_UNLOCK(MUTEX) pthread_mutex_unlock(MUTEX)


BOOL auc_mutex_initialize(pthread_mutex_t* mutex);
BOOL auc_mutex_destroy(pthread_mutex_t* mutex);
