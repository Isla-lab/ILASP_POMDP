# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.26

# Delete rule output on recipe failure.
.DELETE_ON_ERROR:

#=============================================================================
# Special targets provided by cmake.

# Disable implicit rules so canonical targets will work.
.SUFFIXES:

# Disable VCS-based implicit rules.
% : %,v

# Disable VCS-based implicit rules.
% : RCS/%

# Disable VCS-based implicit rules.
% : RCS/%,v

# Disable VCS-based implicit rules.
% : SCCS/s.%

# Disable VCS-based implicit rules.
% : s.%

.SUFFIXES: .hpux_make_needs_suffix_list

# Command-line flag to silence nested $(MAKE).
$(VERBOSE)MAKESILENT = -s

#Suppress display of executed commands.
$(VERBOSE).SILENT:

# A target that is always out of date.
cmake_force:
.PHONY : cmake_force

#=============================================================================
# Set environment variables for the build.

# The shell in which to execute make rules.
SHELL = /bin/sh

# The CMake executable.
CMAKE_COMMAND = /usr/local/lib/python3.8/dist-packages/cmake/data/bin/cmake

# The command to remove a file.
RM = /usr/local/lib/python3.8/dist-packages/cmake/data/bin/cmake -E rm -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/daniele/Documents/ilasp_pomdp/despot

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/daniele/Documents/ilasp_pomdp/despot/build

# Include any dependencies generated for this target.
include examples/cpp_models/rock_sample/CMakeFiles/despot_fvrs.dir/depend.make
# Include any dependencies generated by the compiler for this target.
include examples/cpp_models/rock_sample/CMakeFiles/despot_fvrs.dir/compiler_depend.make

# Include the progress variables for this target.
include examples/cpp_models/rock_sample/CMakeFiles/despot_fvrs.dir/progress.make

# Include the compile flags for this target's objects.
include examples/cpp_models/rock_sample/CMakeFiles/despot_fvrs.dir/flags.make

examples/cpp_models/rock_sample/CMakeFiles/despot_fvrs.dir/src/base/base_rock_sample.cpp.o: examples/cpp_models/rock_sample/CMakeFiles/despot_fvrs.dir/flags.make
examples/cpp_models/rock_sample/CMakeFiles/despot_fvrs.dir/src/base/base_rock_sample.cpp.o: /home/daniele/Documents/ilasp_pomdp/despot/examples/cpp_models/rock_sample/src/base/base_rock_sample.cpp
examples/cpp_models/rock_sample/CMakeFiles/despot_fvrs.dir/src/base/base_rock_sample.cpp.o: examples/cpp_models/rock_sample/CMakeFiles/despot_fvrs.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/daniele/Documents/ilasp_pomdp/despot/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object examples/cpp_models/rock_sample/CMakeFiles/despot_fvrs.dir/src/base/base_rock_sample.cpp.o"
	cd /home/daniele/Documents/ilasp_pomdp/despot/build/examples/cpp_models/rock_sample && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT examples/cpp_models/rock_sample/CMakeFiles/despot_fvrs.dir/src/base/base_rock_sample.cpp.o -MF CMakeFiles/despot_fvrs.dir/src/base/base_rock_sample.cpp.o.d -o CMakeFiles/despot_fvrs.dir/src/base/base_rock_sample.cpp.o -c /home/daniele/Documents/ilasp_pomdp/despot/examples/cpp_models/rock_sample/src/base/base_rock_sample.cpp

examples/cpp_models/rock_sample/CMakeFiles/despot_fvrs.dir/src/base/base_rock_sample.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/despot_fvrs.dir/src/base/base_rock_sample.cpp.i"
	cd /home/daniele/Documents/ilasp_pomdp/despot/build/examples/cpp_models/rock_sample && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/daniele/Documents/ilasp_pomdp/despot/examples/cpp_models/rock_sample/src/base/base_rock_sample.cpp > CMakeFiles/despot_fvrs.dir/src/base/base_rock_sample.cpp.i

examples/cpp_models/rock_sample/CMakeFiles/despot_fvrs.dir/src/base/base_rock_sample.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/despot_fvrs.dir/src/base/base_rock_sample.cpp.s"
	cd /home/daniele/Documents/ilasp_pomdp/despot/build/examples/cpp_models/rock_sample && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/daniele/Documents/ilasp_pomdp/despot/examples/cpp_models/rock_sample/src/base/base_rock_sample.cpp -o CMakeFiles/despot_fvrs.dir/src/base/base_rock_sample.cpp.s

examples/cpp_models/rock_sample/CMakeFiles/despot_fvrs.dir/src/fvrs/fvrs.cpp.o: examples/cpp_models/rock_sample/CMakeFiles/despot_fvrs.dir/flags.make
examples/cpp_models/rock_sample/CMakeFiles/despot_fvrs.dir/src/fvrs/fvrs.cpp.o: /home/daniele/Documents/ilasp_pomdp/despot/examples/cpp_models/rock_sample/src/fvrs/fvrs.cpp
examples/cpp_models/rock_sample/CMakeFiles/despot_fvrs.dir/src/fvrs/fvrs.cpp.o: examples/cpp_models/rock_sample/CMakeFiles/despot_fvrs.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/daniele/Documents/ilasp_pomdp/despot/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Building CXX object examples/cpp_models/rock_sample/CMakeFiles/despot_fvrs.dir/src/fvrs/fvrs.cpp.o"
	cd /home/daniele/Documents/ilasp_pomdp/despot/build/examples/cpp_models/rock_sample && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT examples/cpp_models/rock_sample/CMakeFiles/despot_fvrs.dir/src/fvrs/fvrs.cpp.o -MF CMakeFiles/despot_fvrs.dir/src/fvrs/fvrs.cpp.o.d -o CMakeFiles/despot_fvrs.dir/src/fvrs/fvrs.cpp.o -c /home/daniele/Documents/ilasp_pomdp/despot/examples/cpp_models/rock_sample/src/fvrs/fvrs.cpp

examples/cpp_models/rock_sample/CMakeFiles/despot_fvrs.dir/src/fvrs/fvrs.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/despot_fvrs.dir/src/fvrs/fvrs.cpp.i"
	cd /home/daniele/Documents/ilasp_pomdp/despot/build/examples/cpp_models/rock_sample && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/daniele/Documents/ilasp_pomdp/despot/examples/cpp_models/rock_sample/src/fvrs/fvrs.cpp > CMakeFiles/despot_fvrs.dir/src/fvrs/fvrs.cpp.i

examples/cpp_models/rock_sample/CMakeFiles/despot_fvrs.dir/src/fvrs/fvrs.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/despot_fvrs.dir/src/fvrs/fvrs.cpp.s"
	cd /home/daniele/Documents/ilasp_pomdp/despot/build/examples/cpp_models/rock_sample && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/daniele/Documents/ilasp_pomdp/despot/examples/cpp_models/rock_sample/src/fvrs/fvrs.cpp -o CMakeFiles/despot_fvrs.dir/src/fvrs/fvrs.cpp.s

examples/cpp_models/rock_sample/CMakeFiles/despot_fvrs.dir/src/fvrs/main.cpp.o: examples/cpp_models/rock_sample/CMakeFiles/despot_fvrs.dir/flags.make
examples/cpp_models/rock_sample/CMakeFiles/despot_fvrs.dir/src/fvrs/main.cpp.o: /home/daniele/Documents/ilasp_pomdp/despot/examples/cpp_models/rock_sample/src/fvrs/main.cpp
examples/cpp_models/rock_sample/CMakeFiles/despot_fvrs.dir/src/fvrs/main.cpp.o: examples/cpp_models/rock_sample/CMakeFiles/despot_fvrs.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/daniele/Documents/ilasp_pomdp/despot/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_3) "Building CXX object examples/cpp_models/rock_sample/CMakeFiles/despot_fvrs.dir/src/fvrs/main.cpp.o"
	cd /home/daniele/Documents/ilasp_pomdp/despot/build/examples/cpp_models/rock_sample && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT examples/cpp_models/rock_sample/CMakeFiles/despot_fvrs.dir/src/fvrs/main.cpp.o -MF CMakeFiles/despot_fvrs.dir/src/fvrs/main.cpp.o.d -o CMakeFiles/despot_fvrs.dir/src/fvrs/main.cpp.o -c /home/daniele/Documents/ilasp_pomdp/despot/examples/cpp_models/rock_sample/src/fvrs/main.cpp

examples/cpp_models/rock_sample/CMakeFiles/despot_fvrs.dir/src/fvrs/main.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/despot_fvrs.dir/src/fvrs/main.cpp.i"
	cd /home/daniele/Documents/ilasp_pomdp/despot/build/examples/cpp_models/rock_sample && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/daniele/Documents/ilasp_pomdp/despot/examples/cpp_models/rock_sample/src/fvrs/main.cpp > CMakeFiles/despot_fvrs.dir/src/fvrs/main.cpp.i

examples/cpp_models/rock_sample/CMakeFiles/despot_fvrs.dir/src/fvrs/main.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/despot_fvrs.dir/src/fvrs/main.cpp.s"
	cd /home/daniele/Documents/ilasp_pomdp/despot/build/examples/cpp_models/rock_sample && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/daniele/Documents/ilasp_pomdp/despot/examples/cpp_models/rock_sample/src/fvrs/main.cpp -o CMakeFiles/despot_fvrs.dir/src/fvrs/main.cpp.s

# Object files for target despot_fvrs
despot_fvrs_OBJECTS = \
"CMakeFiles/despot_fvrs.dir/src/base/base_rock_sample.cpp.o" \
"CMakeFiles/despot_fvrs.dir/src/fvrs/fvrs.cpp.o" \
"CMakeFiles/despot_fvrs.dir/src/fvrs/main.cpp.o"

# External object files for target despot_fvrs
despot_fvrs_EXTERNAL_OBJECTS =

examples/cpp_models/rock_sample/despot_fvrs: examples/cpp_models/rock_sample/CMakeFiles/despot_fvrs.dir/src/base/base_rock_sample.cpp.o
examples/cpp_models/rock_sample/despot_fvrs: examples/cpp_models/rock_sample/CMakeFiles/despot_fvrs.dir/src/fvrs/fvrs.cpp.o
examples/cpp_models/rock_sample/despot_fvrs: examples/cpp_models/rock_sample/CMakeFiles/despot_fvrs.dir/src/fvrs/main.cpp.o
examples/cpp_models/rock_sample/despot_fvrs: examples/cpp_models/rock_sample/CMakeFiles/despot_fvrs.dir/build.make
examples/cpp_models/rock_sample/despot_fvrs: libdespot.so
examples/cpp_models/rock_sample/despot_fvrs: examples/cpp_models/rock_sample/CMakeFiles/despot_fvrs.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/daniele/Documents/ilasp_pomdp/despot/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_4) "Linking CXX executable despot_fvrs"
	cd /home/daniele/Documents/ilasp_pomdp/despot/build/examples/cpp_models/rock_sample && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/despot_fvrs.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
examples/cpp_models/rock_sample/CMakeFiles/despot_fvrs.dir/build: examples/cpp_models/rock_sample/despot_fvrs
.PHONY : examples/cpp_models/rock_sample/CMakeFiles/despot_fvrs.dir/build

examples/cpp_models/rock_sample/CMakeFiles/despot_fvrs.dir/clean:
	cd /home/daniele/Documents/ilasp_pomdp/despot/build/examples/cpp_models/rock_sample && $(CMAKE_COMMAND) -P CMakeFiles/despot_fvrs.dir/cmake_clean.cmake
.PHONY : examples/cpp_models/rock_sample/CMakeFiles/despot_fvrs.dir/clean

examples/cpp_models/rock_sample/CMakeFiles/despot_fvrs.dir/depend:
	cd /home/daniele/Documents/ilasp_pomdp/despot/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/daniele/Documents/ilasp_pomdp/despot /home/daniele/Documents/ilasp_pomdp/despot/examples/cpp_models/rock_sample /home/daniele/Documents/ilasp_pomdp/despot/build /home/daniele/Documents/ilasp_pomdp/despot/build/examples/cpp_models/rock_sample /home/daniele/Documents/ilasp_pomdp/despot/build/examples/cpp_models/rock_sample/CMakeFiles/despot_fvrs.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : examples/cpp_models/rock_sample/CMakeFiles/despot_fvrs.dir/depend

