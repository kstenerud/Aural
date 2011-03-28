//
//  Information.cpp
//  Aural
//
//  Created by Karl Stenerud on 3/27/11.
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

#include "Information.h"


namespace aural
{
    const char* AudioUnitInfo::scopeName(AudioUnitScope scope)
    {
        switch(scope)
        {
            case kAudioUnitScope_Global:
                return "Global";
            case kAudioUnitScope_Input:
                return "Input";
            case kAudioUnitScope_Output:
                return "Output";
#if !TARGET_OS_IPHONE
            case kAudioUnitScope_Group:
                return "Group";
            case kAudioUnitScope_Part:
                return "Part";
            case kAudioUnitScope_Note:
                return "Note";
#endif
            default:
                return "Unknown";
        }
    }
    
    const char* AudioUnitInfo::errorName(OSStatus errorCode)
    {
        switch(errorCode)
        {
            case kAUGraphErr_NodeNotFound:
                return "Node not found";
            case kAUGraphErr_InvalidConnection:
                return "The attempted connection between two nodes cannot be made";
            case kAUGraphErr_OutputNodeErr:
                return "Output Node Error";
            case kAUGraphErr_CannotDoInCurrentContext:
                return "Cannot do in current context";
            case kAUGraphErr_InvalidAudioUnit:
                return "Invalid audio unit";
            case kAudioUnitErr_InvalidProperty:
                return "Invalid property";
            case kAudioUnitErr_InvalidParameter:
                return "Invalid parameter";
            case kAudioUnitErr_InvalidElement:
                return "Invalid element";
            case kAudioUnitErr_NoConnection:
                return "No input connection to fetch data";
            case kAudioUnitErr_FailedInitialization:
                return "Initialization failed";
            case kAudioUnitErr_TooManyFramesToProcess:
                return "Too many frames to process";
            case kAudioUnitErr_InvalidFile:
                return "Invalid file";
            case kAudioUnitErr_FormatNotSupported:
                return "Format not supported";
            case kAudioUnitErr_Uninitialized:
                return "Unit has not been initialized";
            case kAudioUnitErr_InvalidScope:
                return "Invalid scope";
            case kAudioUnitErr_PropertyNotWritable:
                return "Property is not writable";
            case kAudioUnitErr_InvalidPropertyValue:
                return "Invalid property value";
            case kAudioUnitErr_PropertyNotInUse:
                return "Property has not been set to a valid value";
            case kAudioUnitErr_Initialized:
                return "Unit has already been initialized";
            case kAudioUnitErr_InvalidOfflineRender:
                return "Invalid offline render operation";
            case kAudioUnitErr_Unauthorized:
                return "Unit has not been authorized";
            case noErr:
                return "No error";
            default:
                return "Unknown Audio Unit error";
        }
    }

    const char* AudioUnitInfo::propertyName(AudioUnitPropertyID property)
    {
        switch(property)
        {
                // Generic
            case kAudioUnitProperty_ClassInfo:
                return "ClassInfo/Azimuth/Volume";
            case kAudioUnitProperty_MakeConnection:
                return "MakeConnection/Elevation/Enable";
            case kAudioUnitProperty_SampleRate:
                return "SampleRate/Distance/Pan";
            case kAudioUnitProperty_ParameterList:
                return "ParameterList/Gain";
            case kAudioUnitProperty_ParameterInfo:
                return "ParameterInfo/PlaybackRate";
            case k3DMixerParam_Enable:
                return "Enable";
            case k3DMixerParam_MinGain:
                return "MinGain";
            case k3DMixerParam_MaxGain:
                return "MaxGain";
            case kAudioUnitProperty_StreamFormat:
                return "StreamFormat";
            case kAudioUnitProperty_ElementCount:
                return "ElementCount";
            case kAudioUnitProperty_Latency:
                return "Latency";
            case kAudioUnitProperty_SupportedNumChannels:
                return "SupportedNumChannels";
            case kAudioUnitProperty_MaximumFramesPerSlice:
                return "MaximumFramesPerSlice";
            case kAudioUnitProperty_AudioChannelLayout:
                return "AudioChannelLayout";
            case kAudioUnitProperty_TailTime:
                return "TailTime";
            case kAudioUnitProperty_BypassEffect:
                return "BypassEffect";
            case kAudioUnitProperty_LastRenderError:
                return "LastRenderError";
            case kAudioUnitProperty_SetRenderCallback:
                return "SetRenderCallback";
            case kAudioUnitProperty_FactoryPresets:
                return "FactoryPresets";
            case kAudioUnitProperty_RenderQuality:
                return "RenderQuality";
            case kAudioUnitProperty_InPlaceProcessing:
                return "InPlaceProcessing";
            case kAudioUnitProperty_ElementName:
                return "ElementName";
            case kAudioUnitProperty_SupportedChannelLayoutTags:
                return "SupportedChannelLayoutTags";
            case kAudioUnitProperty_PresentPreset:
                return "PresentPreset";
            case kAudioUnitProperty_ShouldAllocateBuffer:
                return "ShouldAllocateBuffer";
            case kAudioUnitProperty_ParameterHistoryInfo:
                return "ParameterHistoryInfo";
                
                // 3D Mixer
            case kAudioUnitProperty_3DMixerDistanceParams:
                return "3DMixerDistanceParams";
            case kAudioUnitProperty_3DMixerAttenuationCurve:
                return "3DMixerAttenuationCurve";
            case kAudioUnitProperty_SpatializationAlgorithm:
                return "SpatializationAlgorithm";
            case kAudioUnitProperty_DopplerShift:
                return "DopplerShift";
            case kAudioUnitProperty_3DMixerRenderingFlags:
                return "3DMixerRenderingFlags";
            case kAudioUnitProperty_3DMixerDistanceAtten:
                return "3DMixerDistanceAtten";
            case kAudioUnitProperty_ReverbPreset:
                return "ReverbPreset";
            case kAudioUnitProperty_MeteringMode:
                return "MeteringMode";
            case kAudioUnitProperty_MatrixLevels:
                return "MatrixLevels";
            case kAudioUnitProperty_MatrixDimensions:
                return "MatrixDimensions";
            case kAudioUnitProperty_MeterClipping:
                return "MeterClipping";

            default:
                // Not threadsafe, but unlikely to clash
                static char unknownBuffer[100];
                sprintf(unknownBuffer, "Unknown (%lu)", property);
                return unknownBuffer;
        }
    }
    
    const char* AudioUnitInfo::parameterName(AudioUnitParameterID parameter)
    {
        switch(parameter)
        {
            case k3DMixerParam_Azimuth:
                return "Azimuth";
            case k3DMixerParam_Elevation:
                return "Elevation";
            case k3DMixerParam_Distance:
                return "Distance";
            case k3DMixerParam_Gain:
                return "Gain";
            case k3DMixerParam_PlaybackRate:
                return "PlaybackRate";
            case k3DMixerParam_Enable:
                return "Enable";
            case k3DMixerParam_MinGain:
                return "MinGain";
            case k3DMixerParam_MaxGain:
                return "MaxGain";
            default:
                // Not threadsafe, but unlikely to clash
                static char unknownBuffer[100];
                sprintf(unknownBuffer, "Unknown (%lu)", parameter);
                return unknownBuffer;
        }
    }
    
    
    const char* AudioSessionInfo::errorName(OSStatus errorCode)
    {
        switch(errorCode)
        {
            case kAudioSessionNotInitialized:
                return "Audio session not initialized";
            case kAudioSessionAlreadyInitialized:
                return "Audio session already initialized";
            case kAudioSessionInitializationError:
                return "Audio sesion initialization error";
            case kAudioSessionUnsupportedPropertyError:
                return "Unsupported audio session property";
            case kAudioSessionBadPropertySizeError:
                return "Bad audio session property size";
            case kAudioSessionNotActiveError:
                return "Audio session is not active";
#ifdef __IPHONE_3_1
#if __IPHONE_OS_VERSION_MAX_ALLOWED >= __IPHONE_3_1
            case kAudioSessionNoCategorySet:
                return "No audio session category set";
            case kAudioSessionIncompatibleCategory:
                return "Incompatible audio session category";
#endif /* __IPHONE_OS_VERSION_MAX_ALLOWED >= __IPHONE_3_1 */
#endif /* __IPHONE_3_1 */
            case noErr:
                return "No error";
            default:
                return "Unknown audio session error";
        }
    }
}
