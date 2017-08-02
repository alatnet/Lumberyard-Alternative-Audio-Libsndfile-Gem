
#pragma once

#include <AzCore/Component/Component.h>

#include "AudioSource_libsnd.h"
#include "AudioSource_libsnd_memory.h"
#include <AlternativeAudio\AlternativeAudioBus.h>

namespace AlternativeAudio_Libsndfile
{
    class AlternativeAudio_LibsndfileSystemComponent
        : public AZ::Component
    {
    public:
        AZ_COMPONENT(AlternativeAudio_LibsndfileSystemComponent, "{6FF34875-5DE8-4F32-A7BC-E4D17ECA05AF}");

        static void Reflect(AZ::ReflectContext* context);

        static void GetProvidedServices(AZ::ComponentDescriptor::DependencyArrayType& provided);
        static void GetIncompatibleServices(AZ::ComponentDescriptor::DependencyArrayType& incompatible);
        static void GetRequiredServices(AZ::ComponentDescriptor::DependencyArrayType& required);
        static void GetDependentServices(AZ::ComponentDescriptor::DependencyArrayType& dependent);
    protected:
        ////////////////////////////////////////////////////////////////////////
        // AZ::Component interface implementation
        void Init() override;
        void Activate() override;
        void Deactivate() override;
        ////////////////////////////////////////////////////////////////////////
	private:
		static bool libraryRegistered;
    };
}
