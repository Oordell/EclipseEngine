import os
import subprocess
import check_python

# Make sure everything we need is installed
check_python.validate_packages()

import vulkan

# Change from Scripts directory to root
os.chdir("../")

if not vulkan.CheckVulkanSDK():
    print("Vulkan SDK not installed.")

if not vulkan.CheckVulkanSDKDebugLibs():
    print("Vulkan SDK debug libs not found.")

print("\nUpdating submodules...")
subprocess.call(["git", "submodule", "update", "--init", "--recursive"])

print("Running premake...")
subprocess.call(["vendor/bin/premake/premake5.exe", "vs2022"])
