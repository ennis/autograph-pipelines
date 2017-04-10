#########################################################################
# Input params
[CmdletBinding()]
Param(
   [Parameter(Mandatory=$True,Position=1)]
   [ValidateSet('create-plugin','open', 'cmake', 'run')]
   [string]$action,
   [Parameter(Mandatory=$False)]
   [string]$pluginName,
   [string]$targets
)

#########################################################################
# Config
$BOOST_INCLUDE_DIR = "C:\Boost\include\boost-1_62";
$LLVM_DIR = "C:\Users\Alexandre\Developpement\llvm3.9\lib\cmake\llvm";
$Clang_DIR = "C:\Users\Alexandre\Developpement\llvm3.9\lib\cmake\clang";
$PROJECT_NAME = "pipelines";
$BUILD_DIR = "build";
$pluginsSourceDir = "src/plugins";

#########################################################################
# Templates
$cmakePluginTemplate = @"
set(SRC $pluginName.cpp)
add_autograph_module($pluginName `${SRC})
"@

$pluginSourceTemplate = @"
#include "$pluginName.Config.h"
#include <autograph/engine/All.h>
#include <autograph/gl/All.h>

//////////////////////////////////////////////
class ${pluginName} : public ag::Extension
{
public:
};

//////////////////////////////////////////////
PLUGIN_ENTRY{
    ag::registerClass<${pluginName}, ag::Extension>("${pluginName}");
}
"@

#########################################################################
function runCMakeConfig() 
{
    mkdir -Force -Path $BUILD_DIR/
    cd build/
    cmake ../ -G "Visual Studio 15 2017 Win64" `
        -DBoost_INCLUDE_DIR:PATH=$BOOST_INCLUDE_DIR `
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
    mkdir -Force -Path "$pluginsSourceDir/$pluginName/" 
    Out-File -FilePath "$pluginsSourceDir/$pluginName/CMakeLists.txt" -InputObject $cmakePluginTemplate -Encoding utf8
    Out-File -FilePath "$pluginsSourceDir/$pluginName/$pluginName.cpp" -InputObject $pluginSourceTemplate -Encoding utf8
} 
elseif ($action -eq "open") {
    runCMakeConfig
    Invoke-Item .\$BUILD_DIR\${PROJECT_NAME}.sln
}
elseif ($action -eq "cmake") {
    runCMakeConfig
}
elseif ($action -eq "run") {
    buildProject -target $targets
}
