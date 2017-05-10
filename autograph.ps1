#########################################################################
# Input params
[CmdletBinding()]
Param(
   [Parameter(Mandatory=$True,Position=1)]
   [ValidateSet('create-plugin', 'create-application', 'open', 'cmake', 'full-cmake', 'run')]
   [string]$action,
   [Parameter(Mandatory=$False)]
   [string]$name,
   [string]$targets
)

#########################################################################
# Config
$BOOST_INCLUDE_DIR = "C:\Boost\boost_1_64_0";
$LLVM_DIR = "C:\Users\Alexandre\Developpement\llvm3.9\lib\cmake\llvm";
$Clang_DIR = "C:\Users\Alexandre\Developpement\llvm3.9\lib\cmake\clang";
$PROJECT_NAME = "autograph";
$BUILD_DIR = "build";
$coreSourceDir = "src";
$pluginsSourceDir = "src_plugins";
$BOOST_DEBUG = "FALSE";

#########################################################################
# Templates
$cmakePluginTemplate = @"
set(SRC $name.cpp)
autograph_add_module(PLUGIN NAME $name SOURCES `${SRC})
target_link_libraries($name PUBLIC autographEngine)
"@

$cmakeAppTemplate = @"
cmake_minimum_required(VERSION 3.2)
set(NAME "$name")
file(GLOB SRCS *.cpp)
file(GLOB HDRS *.h)
add_executable(`${NAME} `${SRCS} `${HDRS})
set_property(TARGET `${NAME} PROPERTY ENABLE_EXPORTS TRUE)
target_link_libraries(`${NAME} autograph_engine)
"@

$pluginSourceTemplate = @"
#include "$name/Exports.h"
#include <autograph/Engine/All.h>
#include <autograph/Gfx/All.h>

//////////////////////////////////////////////
class ${name} : public ag::Extension
{
public:
};

//////////////////////////////////////////////
${name}_PLUGIN_ENTRY{
    ag::registerClass<${name}, ag::Extension>("${name}");
}
"@


$appSourceTemplate = @"
#include <autograph/engine/All.h>
#include <autograph/gl/All.h>

int main()
{
	return 0;
}
"@

#########################################################################
function runCMakeConfig() 
{
    mkdir -Force -Path $BUILD_DIR/
    cd build/
    cmake ../ -G "Visual Studio 15 2017 Win64" `
        -DBOOST_ROOT:PATH=$BOOST_INCLUDE_DIR `
        -DLLVM_DIR:PATH=$LLVM_DIR `
        -DClang_DIR:PATH=$Clang_DIR 
    cd ../
}

function buildProject([string]$target=$null) 
{
    if ($target) {
        cmake --build $BUILD_DIR --target $target
    } 
    else {
        cmake --build $BUILD_DIR
    }
}

#########################################################################
if ($action -eq "create-plugin") {
    mkdir -Force -Path "$pluginsSourceDir/$name/" | Out-Null
    Out-File -FilePath "$pluginsSourceDir/$name/CMakeLists.txt" -InputObject $cmakePluginTemplate -Encoding utf8
    Out-File -FilePath "$pluginsSourceDir/$name/$name.cpp" -InputObject $pluginSourceTemplate -Encoding utf8
} 
elseif ($action -eq "create-application") {
    mkdir -Force -Path "$coreSourceDir/$name/" | Out-Null
    Out-File -FilePath "$coreSourceDir/$name/CMakeLists.txt" -InputObject $cmakeAppTemplate -Encoding utf8
    Out-File -FilePath "$coreSourceDir/$name/main.cpp" -InputObject $appSourceTemplate -Encoding utf8
} 
elseif ($action -eq "open") {
    runCMakeConfig
    Invoke-Item .\$BUILD_DIR\${PROJECT_NAME}.sln
}
elseif ($action -eq "cmake") {
    runCMakeConfig
}
elseif ($action -eq "full-cmake") {
	rm "$BUILD_DIR\CMakeCache.txt" | Out-Null
    runCMakeConfig
}
elseif ($action -eq "run") {
    buildProject -target $targets
}
