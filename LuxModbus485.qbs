import qbs
import qbs.FileInfo

Project {
    name: "LuxModbus485"
    Product {
        Depends { name: "cpp" }

        name: "micro"
        type: ["application", "hex", "bin", "size","sors"]


        property string includePaths: ".."
        property string corePath: "C:/Program Files (x86)/Arduino/hardware/arduino/avr/cores/arduino/"
        property string coreIncludePaths: "C:/Program Files (x86)/Arduino/hardware/arduino/avr/variants/standard/"
        property string coreLibrariesPath: "C:/Program Files (x86)/Arduino/hardware/arduino/avr/libraries/"
        property string externalLibrariesPath: "C:/YandexDisk/Projects/Arduino/libraries/"

        property stringList libraries: [
            "EEPROM", "BH1750" ,
            "Wire", "GyverTM1637",
            "GyverWDT", "GyverTimer",
            "GyverEncoder", "GyverButton",
            "RTClib", "SoftwareSerial"
        ]

        Group {
            name: "Core cpp"
            files: ["C:/Program Files (x86)/Arduino/hardware/arduino/avr/cores/arduino/*.cpp"]
            fileTags: ['cpp']
        }
        Group {
            name: "Core c"
            files: ["C:/Program Files (x86)/Arduino/hardware/arduino/avr/cores/arduino/*.c"]
            fileTags: ['c']
        }
        Group {
            name: "Core s"
            files: ["C:/Program Files (x86)/Arduino/hardware/arduino/avr/cores/arduino/*.s"]
            fileTags: ['asm']
        }
        Group {
            name: "Core h"
            files: ["C:/Program Files (x86)/Arduino/hardware/arduino/avr/cores/arduino/*.h"]
            fileTags: ['hpp']
        }

        files: ["*.cpp","*.h"]

        Rule {
            id: hex
            inputs: ["application"]
            prepare: {
                var objcopyPath = input.cpp.objcopyPath
                var args = ["-O", "ihex", input.filePath, output.filePath];                
                var cmd = new Command(objcopyPath, args);
                cmd.description = "converting to hex: " + FileInfo.fileName(input.filePath);
                cmd.highlight = "linker";
                return cmd;
            }
            Artifact {
                fileTags: ["hex"]
                filePath: FileInfo.baseName(input.filePath) + ".hex"
            }
        }

        Rule {
            id: bin
            inputs: ["application"]
            prepare: {
                var objcopyPath = input.cpp.objcopyPath
                var args = ["-O", "binary", input.filePath, output.filePath];
                var cmd = new Command(objcopyPath, args);
                cmd.description = "converting to bin: "+ FileInfo.fileName(input.filePath);
                cmd.highlight = "linker";
                return cmd;
            }
            Artifact {
                fileTags: ["bin"]
                filePath: FileInfo.baseName(input.filePath) + ".bin"
            }
        }

        Rule {
            id: size
            inputs: ["application"]
            alwaysRun: true
            prepare: {
                var sizePath = input.cpp.toolchainPrefix + "size"
                var args = ["-C","--mcu=atmega328p",input.filePath];
                var cmd = new Command(sizePath, args);
                cmd.description = "File size: " + FileInfo.fileName(input.filePath);
                cmd.highlight = "linker";
                return cmd;
            }
            Artifact {
                fileTags: ["size"]
                filePath: undefined
            }
        }

        cpp.toolchainInstallPath: "C:\\Program Files (x86)\\Arduino\\hardware\\tools\\avr\\bin"
        cpp.cxxCompilerName: "g++.exe"
        cpp.cCompilerName: "gcc.exe"

        cpp.linkerMode : "manual"
        cpp.linkerPath: "C:\\Program Files (x86)\\Arduino\\hardware\\tools\\avr\\bin\\avr-gcc.exe"
        //cpp.linkerName: "avr-gcc.exe"

        cpp.cLanguageVersion : "c11"
        cpp.cxxLanguageVersion: "c++11"
        cpp.debugInformation: false // passes -g
        //cpp.optimization: "small" // passes -Os
        cpp.warningLevel: "none" // passes -w
        cpp.enableExceptions: false // passes -fno-exceptions
        cpp.commonCompilerFlags: [
            "-ffunction-sections",
            "-O2",
            "-fdata-sections",
            "-MMD",
            "-mmcu=atmega328p",            
            "-fpermissive",
            "-flto",
            "-fno-fat-lto-objects"
        ]
        cpp.linkerFlags:[
            "-w",
            "-O2",
            "-g",
            "-flto",
            "-fuse-linker-plugin",
            "-Wl,--gc-sections",
            "-mmcu=atmega328p",
            "-lm"
        ]
        cpp.defines: [
            "F_CPU=16000000L",
            "ARDUINO=10813",
            "ARDUINO_AVR_NANO",
            "ARDUINO_ARCH_AVR"
        ]

        property pathList librariesIncludePaths: {
            var l = []
            for (var i = 0; i < libraries.length; i++) {                

                l = l.concat(externalLibrariesPath + "/"+libraries[i])
                l = l.concat(externalLibrariesPath + "/"+libraries[i]+"/src")

                l = l.concat(coreLibrariesPath+"/"+libraries[i])
                l = l.concat(coreLibrariesPath+"/"+libraries[i]+"/src")

                l = l.concat(coreLibrariesPath+"/"+libraries[i])
                l = l.concat(coreLibrariesPath+"/"+libraries[i]+"/src/utility")

            }
            return l
        }

        Group {
            condition: true
            name: "Core Libraries files"
            files: {
                var l = []
                for (var i = 0; i < libraries.length; i++) {                    

                    l = l.concat(libraries[i]+"/*.cpp")
                    l = l.concat(libraries[i]+"/*.c")
                    l = l.concat(libraries[i]+"/*.h")
                    l = l.concat(libraries[i]+"/*.hpp")
                    l = l.concat(libraries[i]+"/src/*.cpp")
                    l = l.concat(libraries[i]+"/src/*.c")
                    l = l.concat(libraries[i]+"/src/*.h")
                    l = l.concat(libraries[i]+"/src/*.hpp")
                    l = l.concat(libraries[i]+"/src/utility/*.cpp")
                    l = l.concat(libraries[i]+"/src/utility/*.c")
                    l = l.concat(libraries[i]+"/src/utility/*.h")
                    l = l.concat(libraries[i]+"/src/utility/*.hpp")

                    console.warn("Using Core library: " + libraries[i])
                    //console.info("Using Core library: " + libraries[i])

                }
                return l
            }
            prefix: coreLibrariesPath  // From Arduino App
            cpp.warningLevel: "none"
        }

        Group {
            name: "Project Libraries files"
            files: {
                var l = []
                for (var i = 0; i < libraries.length; i++) {

                    l = l.concat(libraries[i]+"/*.cpp")
                    l = l.concat(libraries[i]+"/*.c")
                    l = l.concat(libraries[i]+"/*.h")
                    l = l.concat(libraries[i]+"/*.hpp")
                    l = l.concat(libraries[i]+"/src/*.cpp")
                    l = l.concat(libraries[i]+"/src/*.c")
                    l = l.concat(libraries[i]+"/src/*.h")
                    l = l.concat(libraries[i]+"/src/*.hpp")

                    console.warn("Using Project library: " + libraries[i])
                    //console.info("Using Project library: " + libraries[i])

                }

                return l
            }
            prefix: externalLibrariesPath
        }

        cpp.includePaths: {
            var l = []
            return l.concat(
                        // Core
                        corePath,
                        coreIncludePaths,
                        // Core+local Libraries
                        librariesIncludePaths,
                        // Project include path
                        includePaths
                        )
        }


    }
}
