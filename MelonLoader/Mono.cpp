#include <string>
#include "Mono.h"
#include "MelonLoader.h"
#include "AssertionManager.h"
#include "ModHandler.h"
#include "Logger.h"

bool Mono::IsOldMono = false;
char* Mono::AssemblyPath = NULL;
char* Mono::BasePath = NULL;
char* Mono::ConfigPath = NULL;
HMODULE Mono::Module = NULL;
MonoDomain* Mono::Domain = NULL;
mono_init_t Mono::mono_init = NULL;
mono_jit_init_t Mono::mono_jit_init = NULL;
mono_jit_init_version_t Mono::mono_jit_init_version = NULL;
mono_jit_cleanup_t Mono::mono_jit_cleanup = NULL;
mono_assembly_setrootdir_t Mono::mono_assembly_setrootdir = NULL;
mono_set_assemblies_path_t Mono::mono_set_assemblies_path = NULL;
mono_set_config_dir_t Mono::mono_set_config_dir = NULL;
mono_domain_assembly_open_t Mono::mono_domain_assembly_open = NULL;
mono_assembly_get_image_t Mono::mono_assembly_get_image = NULL;
mono_class_from_name_t Mono::mono_class_from_name = NULL;
mono_class_get_method_from_name_t Mono::mono_class_get_method_from_name = NULL;
mono_runtime_invoke_t Mono::mono_runtime_invoke = NULL;
mono_add_internal_call_t Mono::mono_add_internal_call = NULL;
mono_thread_current_t Mono::mono_thread_current = NULL;
mono_thread_set_main_t Mono::mono_thread_set_main = NULL;
mono_string_to_utf8_t Mono::mono_string_to_utf8 = NULL;
mono_string_new_t Mono::mono_string_new = NULL;
mono_class_get_property_from_name_t Mono::mono_class_get_property_from_name = NULL;
mono_property_get_get_method_t Mono::mono_property_get_get_method = NULL;
mono_object_get_class_t Mono::mono_object_get_class = NULL;
mono_runtime_set_main_args_t Mono::mono_runtime_set_main_args = NULL;
mono_domain_set_config_t Mono::mono_domain_set_config = NULL;
mono_method_get_name_t Mono::mono_method_get_name = NULL;
mono_debug_init_t Mono::mono_debug_init = NULL;
mono_debug_domain_create_t Mono::mono_debug_domain_create = NULL;
mono_jit_parse_options_t Mono::mono_jit_parse_options = NULL;

bool Mono::Load()
{
	AssertionManager::Start("Mono.cpp", "Mono::Load");
	Module = LoadLibrary((std::string(BasePath) + "\\mono.dll").c_str());
	if (Module != NULL)
		IsOldMono = true;
	if (Module == NULL)
		Module = LoadLibrary((std::string(BasePath) + "\\mono-2.0-bdwgc.dll").c_str());
	if (Module == NULL)
		Module = LoadLibrary((std::string(BasePath) + "\\mono-2.0-sgen.dll").c_str());
	if (Module == NULL)
		Module = LoadLibrary((std::string(BasePath) + "\\mono-2.0-boehm.dll").c_str());
	if (Module)
		HMODULE MonoPosixDLL = AssertionManager::LoadLib("MonoPosixDLL", (std::string(BasePath) + "\\MonoPosixHelper.dll").c_str());
	else
		AssertionManager::ThrowError("Failed to load Mono Module!");
	return !AssertionManager::Result;
}

bool Mono::Setup()
{
	AssertionManager::Start("Mono.cpp", "Mono::Setup");

	mono_init = (mono_init_t)AssertionManager::GetExport(Module, "mono_init");
	mono_jit_init = (mono_jit_init_t)AssertionManager::GetExport(Module, "mono_jit_init");
	mono_jit_init_version = (mono_jit_init_version_t)AssertionManager::GetExport(Module, "mono_jit_init_version");
	mono_jit_cleanup = (mono_jit_cleanup_t)AssertionManager::GetExport(Module, "mono_jit_cleanup");
	mono_assembly_setrootdir = (mono_assembly_setrootdir_t)AssertionManager::GetExport(Module, "mono_assembly_setrootdir");
	mono_set_assemblies_path = (mono_set_assemblies_path_t)AssertionManager::GetExport(Module, "mono_set_assemblies_path");
	mono_set_config_dir = (mono_set_config_dir_t)AssertionManager::GetExport(Module, "mono_set_config_dir");
	mono_domain_assembly_open = (mono_domain_assembly_open_t)AssertionManager::GetExport(Module, "mono_domain_assembly_open");
	mono_assembly_get_image = (mono_assembly_get_image_t)AssertionManager::GetExport(Module, "mono_assembly_get_image");
	mono_class_from_name = (mono_class_from_name_t)AssertionManager::GetExport(Module, "mono_class_from_name");
	mono_class_get_method_from_name = (mono_class_get_method_from_name_t)AssertionManager::GetExport(Module, "mono_class_get_method_from_name");
	mono_runtime_invoke = (mono_runtime_invoke_t)AssertionManager::GetExport(Module, "mono_runtime_invoke");
	mono_method_get_name = (mono_method_get_name_t)AssertionManager::GetExport(Module, "mono_method_get_name");
	mono_add_internal_call = (mono_add_internal_call_t)AssertionManager::GetExport(Module, "mono_add_internal_call");
	mono_thread_current = (mono_thread_current_t)AssertionManager::GetExport(Module, "mono_thread_current");
	mono_thread_set_main = (mono_thread_set_main_t)AssertionManager::GetExport(Module, "mono_thread_set_main");
	mono_string_to_utf8 = (mono_string_to_utf8_t)AssertionManager::GetExport(Module, "mono_string_to_utf8");
	mono_string_new = (mono_string_new_t)AssertionManager::GetExport(Module, "mono_string_new");
	mono_class_get_property_from_name = (mono_class_get_property_from_name_t)AssertionManager::GetExport(Module, "mono_class_get_property_from_name");
	mono_property_get_get_method = (mono_property_get_get_method_t)AssertionManager::GetExport(Module, "mono_property_get_get_method");
	mono_object_get_class = (mono_object_get_class_t)AssertionManager::GetExport(Module, "mono_object_get_class");

	if (MelonLoader::IsGameIl2Cpp)
	{
		mono_debug_init = (mono_debug_init_t)AssertionManager::GetExport(Module, "mono_debug_init");
		mono_debug_domain_create = (mono_debug_domain_create_t)AssertionManager::GetExport(Module, "mono_debug_domain_create");
		mono_jit_parse_options = (mono_jit_parse_options_t)AssertionManager::GetExport(Module, "mono_jit_parse_options");
	}

	if (!IsOldMono)
	{
		mono_runtime_set_main_args = (mono_runtime_set_main_args_t)AssertionManager::GetExport(Module, "mono_runtime_set_main_args");
		mono_domain_set_config = (mono_domain_set_config_t)AssertionManager::GetExport(Module, "mono_domain_set_config");
	}

	return !AssertionManager::Result;
}

void Mono::CreateDomain()
{
	if (Domain == NULL)
	{
		mono_set_assemblies_path(AssemblyPath);
		mono_assembly_setrootdir(AssemblyPath);
		mono_set_config_dir(ConfigPath);
		mono_runtime_set_main_args(MelonLoader::CommandLineC, MelonLoader::CommandLineV);
		Domain = mono_jit_init("MelonLoader");
		Mono::FixDomainBaseDir();
		mono_debug_init(MONO_DEBUG_FORMAT_MONO);
		mono_debug_domain_create(Domain);
		mono_jit_parse_options(MelonLoader::CommandLineC, MelonLoader::CommandLineV);
	}
}

void Mono::FixDomainBaseDir()
{
	mono_thread_set_main(mono_thread_current());
	if (!IsOldMono)
		mono_domain_set_config(Domain, MelonLoader::GamePath, "MelonLoader");
}

const char* Mono::GetStringProperty(const char* propertyName, MonoClass* classType, MonoObject* classObject) { return mono_string_to_utf8((MonoString*)mono_runtime_invoke(mono_property_get_get_method(mono_class_get_property_from_name(classType, propertyName)), classObject, NULL, NULL)); }
void Mono::LogExceptionMessage(MonoObject* exceptionObject, bool shouldThrow) { if (shouldThrow) AssertionManager::ThrowError(GetStringProperty("Message", mono_object_get_class(exceptionObject), exceptionObject)); else Logger::LogError(GetStringProperty("Message", mono_object_get_class(exceptionObject), exceptionObject)); }