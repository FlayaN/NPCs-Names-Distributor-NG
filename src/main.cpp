#include "Distributor.h"
#include "Hooks.h"
#include "Hotkeys.h"
#include "LookupNameDefinitions.h"
#include "ModAPI.h"
#include "NNDKeywords.h"
#include "Options.h"
#include "Persistency.h"

// ReSharper disable once CppParameterMayBeConstPtrOrRef
void MessageHandler(SKSE::MessagingInterface::Message* a_message) {
	switch (a_message->type) {
	case SKSE::MessagingInterface::kPostLoad:
		// Disregard result of the LoadNameDefinitions. If nothing is loaded we still can use Obscurity.
		NND::LoadNameDefinitions();
		break;
	case SKSE::MessagingInterface::kPostPostLoad:
		NND::Options::Load();
		NND::Install();
		NND::Distribution::Manager::Register();
		break;
	case SKSE::MessagingInterface::kDataLoaded:
		NND::Hotkeys::Manager::Register();
		NND::Persistency::Manager::Register();
		NND::CacheKeywords();
		break;
	case SKSE::MessagingInterface::kPreLoadGame:
		NND::Persistency::Manager::GetSingleton()->StartLoadingGame();
		break;
	case SKSE::MessagingInterface::kPostLoadGame:
		NND::Options::Load();
		NND::Persistency::Manager::GetSingleton()->FinishLoadingGame();
		break;
	default:
		break;
	}
}

void InitializeLog(std::string_view pluginName) {
	auto path = logger::log_directory();
	if (!path) {
		stl::report_and_fail("Failed to find standard logging directory"sv);
	}

	*path /= fmt::format(FMT_STRING("{}.log"), pluginName);
	auto sink = std::make_shared<spdlog::sinks::basic_file_sink_mt>(path->string(), true);

	auto log = std::make_shared<spdlog::logger>("global log"s, std::move(sink));

	log->set_level(spdlog::level::info);
	log->flush_on(spdlog::level::info);

	spdlog::set_default_logger(std::move(log));
	spdlog::set_pattern("[%H:%M:%S] %v"s);
}

SKSEPluginLoad(const SKSE::LoadInterface* a_skse)
{
	const auto plugin{ SKSE::PluginDeclaration::GetSingleton() };
	const auto name{ plugin->GetName() };
	const auto version{ plugin->GetVersion() };

	InitializeLog(name);
	logger::info(FMT_STRING("{} v{}"), name, version);
	logger::info("Game version : {}", a_skse->RuntimeVersion().string());

	SKSE::Init(a_skse);

	if (REL::Module::IsVR())
	{
		REL::IDDatabase::get().IsVRAddressLibraryAtLeastVersion("0.136.0", true);
	}

	SKSE::GetMessagingInterface()->RegisterListener(MessageHandler);

	return true;
}

extern "C" DLLEXPORT void* SKSEAPI RequestPluginAPI(const NND_API::InterfaceVersion a_interfaceVersion) {
	const auto api = Messaging::NNDInterface::GetSingleton();

	logger::info("NND::RequestPluginAPI called, InterfaceVersion {}", static_cast<std::underlying_type<NND_API::InterfaceVersion>::type>(a_interfaceVersion));

	switch (a_interfaceVersion) {
	case NND_API::InterfaceVersion::kV1:
	case NND_API::InterfaceVersion::kV2:
		logger::info("NND::RequestPluginAPI returned the API singleton");
		return api;
	}

	logger::info("NND::RequestPluginAPI requested the wrong interface version");
	return nullptr;
}
