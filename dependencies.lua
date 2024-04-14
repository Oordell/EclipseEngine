-- Eclipse dependencies

VULKAN_SDK = os.getenv("VULKAN_SDK")

IncludeDir = {}
IncludeDir["stb_image"] = "%{wks.location}/Eclipse/vendor/stb_image"
IncludeDir["yaml_cpp"] = "%{wks.location}/Eclipse/vendor/yaml-cpp/include"
IncludeDir["GLFW"] = "%{wks.location}/Eclipse/vendor/GLFW/include"
IncludeDir["Glad"] = "%{wks.location}/Eclipse/vendor/Glad/include"
IncludeDir["ImGui"] = "%{wks.location}/Eclipse/vendor/ImGui"
IncludeDir["ImGuizmo"] = "%{wks.location}/Eclipse/vendor/ImGuizmo"
IncludeDir["glm"] = "%{wks.location}/Eclipse/vendor/glm"
IncludeDir["entt"] = "%{wks.location}/Eclipse/vendor/entt/include"
IncludeDir["au"] = "%{wks.location}/Eclipse/vendor/au"
IncludeDir["shaderc"] = "%{wks.location}/Eclipse/vendor/shaderc/include"
IncludeDir["SPIRV_Cross"] = "%{wks.location}/Eclipse/vendor/SPIRV-Cross"
IncludeDir["VulkanSDK"] = "%{VULKAN_SDK}/Include"

LibraryDir = {}

LibraryDir["VulkanSDK"] = "%{VULKAN_SDK}/Lib"
LibraryDir["VulkanSDK_Debug"] = "%{wks.location}/Eclipse/vendor/VulkanSDK/Lib"

Library = {}
Library["Vulkan"] = "%{LibraryDir.VulkanSDK}/vulkan-1.lib"
Library["VulkanUtils"] = "%{LibraryDir.VulkanSDK}/VkLayer_utils.lib"

Library["ShaderC_Debug"] = "%{LibraryDir.VulkanSDK_Debug}/shaderc_sharedd.lib"
Library["SPIRV_Cross_Debug"] = "%{LibraryDir.VulkanSDK_Debug}/spirv-cross-cored.lib"
Library["SPIRV_Cross_GLSL_Debug"] = "%{LibraryDir.VulkanSDK_Debug}/spirv-cross-glsld.lib"
Library["SPIRV_Tools_Debug"] = "%{LibraryDir.VulkanSDK_Debug}/SPIRV-Toolsd.lib"

Library["ShaderC_Release"] = "%{LibraryDir.VulkanSDK}/shaderc_shared.lib"
Library["SPIRV_Cross_Release"] = "%{LibraryDir.VulkanSDK}/spirv-cross-core.lib"
Library["SPIRV_Cross_GLSL_Release"] = "%{LibraryDir.VulkanSDK}/spirv-cross-glsl.lib"