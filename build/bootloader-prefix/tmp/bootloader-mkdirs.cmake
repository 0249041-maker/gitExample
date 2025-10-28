# Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
# file Copyright.txt or https://cmake.org/licensing for details.

cmake_minimum_required(VERSION 3.5)

file(MAKE_DIRECTORY
  "/home/carmen/esp/esp-idf/components/bootloader/subproject"
  "/home/carmen/Documents/FreeRTOSCourse/Classwork/MQTT/mqtt5/build/bootloader"
  "/home/carmen/Documents/FreeRTOSCourse/Classwork/MQTT/mqtt5/build/bootloader-prefix"
  "/home/carmen/Documents/FreeRTOSCourse/Classwork/MQTT/mqtt5/build/bootloader-prefix/tmp"
  "/home/carmen/Documents/FreeRTOSCourse/Classwork/MQTT/mqtt5/build/bootloader-prefix/src/bootloader-stamp"
  "/home/carmen/Documents/FreeRTOSCourse/Classwork/MQTT/mqtt5/build/bootloader-prefix/src"
  "/home/carmen/Documents/FreeRTOSCourse/Classwork/MQTT/mqtt5/build/bootloader-prefix/src/bootloader-stamp"
)

set(configSubDirs )
foreach(subDir IN LISTS configSubDirs)
    file(MAKE_DIRECTORY "/home/carmen/Documents/FreeRTOSCourse/Classwork/MQTT/mqtt5/build/bootloader-prefix/src/bootloader-stamp/${subDir}")
endforeach()
if(cfgdir)
  file(MAKE_DIRECTORY "/home/carmen/Documents/FreeRTOSCourse/Classwork/MQTT/mqtt5/build/bootloader-prefix/src/bootloader-stamp${cfgdir}") # cfgdir has leading slash
endif()
