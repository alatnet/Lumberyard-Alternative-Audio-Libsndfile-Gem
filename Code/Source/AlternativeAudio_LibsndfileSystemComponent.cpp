
#include "StdAfx.h"

#include <AzCore/Serialization/SerializeContext.h>
#include <AzCore/Serialization/EditContext.h>

#include "AlternativeAudio_LibsndfileSystemComponent.h"

namespace AlternativeAudio_Libsndfile {
	bool AlternativeAudio_LibsndfileSystemComponent::libraryRegistered = false;

	void AlternativeAudio_LibsndfileSystemComponent::Reflect(AZ::ReflectContext* context) {
		if (AZ::SerializeContext* serialize = azrtti_cast<AZ::SerializeContext*>(context)) {
			serialize->Class<AlternativeAudio_LibsndfileSystemComponent, AZ::Component>()
				->Version(0)
				->SerializerForEmptyClass();

			if (AZ::EditContext* ec = serialize->GetEditContext()) {
				ec->Class<AlternativeAudio_LibsndfileSystemComponent>("AlternativeAudio_Libsndfile", "Provides Libsndfile audio sources.")
					->ClassElement(AZ::Edit::ClassElements::EditorData, "")
					->Attribute(AZ::Edit::Attributes::Category, "Alternative Audio - Library")
					->Attribute(AZ::Edit::Attributes::AppearsInAddComponentMenu, AZ_CRC("System", 0xc94d118b))
					->Attribute(AZ::Edit::Attributes::AutoExpand, true)
					;
			}
		}
	}

	void AlternativeAudio_LibsndfileSystemComponent::GetProvidedServices(AZ::ComponentDescriptor::DependencyArrayType& provided) {
		provided.push_back(AZ_CRC("AlternativeAudio_LibsndfileService", 0xf6713d77));
	}

	void AlternativeAudio_LibsndfileSystemComponent::GetIncompatibleServices(AZ::ComponentDescriptor::DependencyArrayType& incompatible) {
		incompatible.push_back(AZ_CRC("AlternativeAudio_LibsndfileService", 0xf6713d77));
	}

	void AlternativeAudio_LibsndfileSystemComponent::GetRequiredServices(AZ::ComponentDescriptor::DependencyArrayType& required) {
		required.push_back(AZ_CRC("AlternativeAudioService", 0x2eb4e627));
	}

	void AlternativeAudio_LibsndfileSystemComponent::GetDependentServices(AZ::ComponentDescriptor::DependencyArrayType& dependent) {
		(void)dependent;
	}

	void AlternativeAudio_LibsndfileSystemComponent::Init() {
	}

	void AlternativeAudio_LibsndfileSystemComponent::Activate() {
		if (libraryRegistered) return;
		libraryRegistered = true;

		AZ_Printf("[Libsndfile]", "[Libsndfile] Registering library.\n");

		AZStd::vector<AZStd::string> filetypes;
		filetypes.push_back("*.wav");
		filetypes.push_back("*.sgi");
		filetypes.push_back("*.aiff");
		filetypes.push_back("*.aifc");
		filetypes.push_back("*.sun");
		filetypes.push_back("*.dec");
		filetypes.push_back("*.next");
		filetypes.push_back("*.au");
		filetypes.push_back("*.snd");
		filetypes.push_back("*.raw");
		filetypes.push_back("*.paf");
		filetypes.push_back("*.iff");
		filetypes.push_back("*.svx");
		filetypes.push_back("*.sf");
		filetypes.push_back("*.voc");
		filetypes.push_back("*.w64");
		filetypes.push_back("*.mat4");
		filetypes.push_back("*.mat5");
		filetypes.push_back("*.pvf");
		filetypes.push_back("*.xi");
		filetypes.push_back("*.htk");
		filetypes.push_back("*.caf");
		filetypes.push_back("*.sd2");
		filetypes.push_back("*.flac");
		filetypes.push_back("*.ogg");

		EBUS_EVENT(
			AlternativeAudio::AlternativeAudioSourceBus,
			RegisterAudioLibrary,
			"libsndfile",
			AZ_CRC("libsndfile", 0x644880ac),
			filetypes,
			[](const char * path, void * userdata)-> AlternativeAudio::IAudioSource* {
				return new AudioSource_Libsnd(path);
			}
		);

		EBUS_EVENT(
			AlternativeAudio::AlternativeAudioSourceBus,
			RegisterAudioLibrary,
			"libsndfile_memory",
			AZ_CRC("libsndfile_memory", 0x51220da8),
			filetypes,
			[](const char * path, void * userdata)-> AlternativeAudio::IAudioSource* {
				return new AudioSource_Libsnd_Memory(path);
			}
		);
		
		AZ_Printf("[Libsndfile]", "[Libsndfile] Libsndfile version: %s\n", sf_version_string());
		AZ_Printf("[Libsndfile]", "[Libsndfile] Registration complete.\n");
	}

	void AlternativeAudio_LibsndfileSystemComponent::Deactivate() {
	}
}
