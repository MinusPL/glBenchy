#configure project and make it crossplatform (Windows, Linux, Web)
cmake_minimum_required(VERSION 3.21)
project(glBenchy)

#Add sources directory
add_subdirectory(src)

#add_executable(glBenchy glbenchy.cxx)