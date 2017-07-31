
#include "StdAfx.h"
#include <platform_impl.h>

#include "AlternativeAudio_LibsndfileSystemComponent.h"

#include <IGem.h>

namespace AlternativeAudio_Libsndfile
{
    class AlternativeAudio_LibsndfileModule
        : public CryHooksModule
    {
    public:
        AZ_RTTI(AlternativeAudio_LibsndfileModule, "{C927DD1D-96B3-4F57-ABFF-BA9F78851BDB}", CryHooksModule);

        AlternativeAudio_LibsndfileModule()
            : CryHooksModule()
        {
            // Push results of [MyComponent]::CreateDescriptor() into m_descriptors here.
            m_descriptors.insert(m_descriptors.end(), {
                AlternativeAudio_LibsndfileSystemComponent::CreateDescriptor(),
            });
        }

        /**
         * Add required SystemComponents to the SystemEntity.
         */
        AZ::ComponentTypeList GetRequiredSystemComponents() const override
        {
            return AZ::ComponentTypeList{
                azrtti_typeid<AlternativeAudio_LibsndfileSystemComponent>(),
            };
        }
    };
}

// DO NOT MODIFY THIS LINE UNLESS YOU RENAME THE GEM
// The first parameter should be GemName_GemIdLower
// The second should be the fully qualified name of the class above
AZ_DECLARE_MODULE_CLASS(AlternativeAudio_Libsndfile_a987cb9a5fec4ed2b6266a481d5236f3, AlternativeAudio_Libsndfile::AlternativeAudio_LibsndfileModule)
