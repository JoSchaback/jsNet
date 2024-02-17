cmake_minimum_required(VERSION 3.20.0)

#project name
project (jsNet VERSION 0.0.1 LANGUAGES C)

set(CMAKE_CXX_STANDARD 23)

# we bake OpenSSL into the project
find_package(OpenSSL REQUIRED)

#### mongoose ####
# add_library(mongoose lib/mongoose/mongoose.c lib/mongoose/mongoose.h)
# target_include_directories(mongoose PUBLIC ${OPENSSL_INCLUDE_DIR})
# target_compile_definitions(mongoose PUBLIC MG_TLS=MG_TLS_OPENSSL)

#### log ####
#add_library(log lib/log/log.c)
#target_include_directories(log PUBLIC lib/log)

#### cliarg ####
#add_library(cliarg lib/cliarg/cliarg.c)
#target_include_directories(cliarg PUBLIC lib/cliarg)

#### JSON ####
#add_library(JSON STATIC lib/JSON/JSON.c)
#target_include_directories(JSON PUBLIC lib/JSON)

#### jsArray ####
#add_library(jsArray lib/jsArray/jsArray.h lib/jsArray/jsArray.c)
#target_include_directories(jsArray PUBLIC lib/jsArray)

#### array ####
#add_library(array lib/array/Array.h lib/array/Array.c)
#target_include_directories(array PUBLIC lib/array)


#add_library(uuid lib/uuid/uuid.h lib/uuid/uuid.c)

#### Multiplexer ####
#add_library(multiplexer lib/multiplexer/Multiplexer.h lib/multiplexer/Multiplexer.c)
#target_include_directories(multiplexer PUBLIC lib/multiplexer)
#target_link_libraries(multiplexer log)

#### ctr_shared ####
#add_library(ctr_shared ctr/Message.c ctr/Config.c ctr/MessageHeader.c ctr/CreateDataNodeMessage.c ctr/ListDataNodeChildrenMessage.c ctr/SetDataNodeValueMessage.c ctr/GetDataNodeMessage.c ctr/SubscribeToDataNodeMessage.c)
#target_link_libraries(ctr_shared JSON)
#target_link_libraries(ctr_shared log)

#add_executable(ctr_hub ctr/hub/main.c ctr/hub/Hub.c ctr/hub/UsernameRecord.c ctr/hub/DataNode.c)
#target_link_libraries(ctr_hub mongoose jsArray OpenSSL::SSL uuid ctr_shared multiplexer log cliarg array)

#add_executable(ctr_test_flows ctr/test/Flows.c ctr/client/Client.c ctr/hub/Hub.c ctr/hub/UsernameRecord.c ctr/hub/DataNode.c)
#target_link_libraries(ctr_test_flows mongoose OpenSSL::SSL jsArray uuid ctr_shared multiplexer log array)

#add_executable(ctr_cli ctr/cli/main.c ctr/cli/cli.c ctr/client/Client.c)
#target_link_libraries(ctr_cli cliarg mongoose OpenSSL::SSL jsArray uuid ctr_shared multiplexer log)

# add_executable(webserver non-blocking-macos.c)
# target_link_libraries(webserver OpenSSL::SSL)

if(CMAKE_SYSTEM MATCHES Windows)
   message(STATUS "Target system is Windows")
endif()

if(CMAKE_HOST_SYSTEM MATCHES Linux)
   message(STATUS "Build host runs Linux")
   add_library(jsNet jsNet-LINUX.c)
endif()

if(CMAKE_HOST_SYSTEM MATCHES Darwin)
   message(STATUS "Build host runs MacOs")
   add_library(jsNet jsNet-MACOS.c)
endif()

target_link_libraries(jsNet OpenSSL::SSL)
target_include_directories(jsNet PUBLIC .)   

add_executable(main tests/main.c)
target_link_libraries(main jsNet)


#include(CTest)
# Include directories
# include_directories(/opt/homebrew/Cellar/openssl@3/3.2.0_1/include)

# Link directories
# link_directories(/opt/homebrew/Cellar/openssl@3/3.2.0_1/lib)


# add_test(NAME ctr_hub_smoke_test COMMAND ctr_hub --max-poll-count 1)
# add_test(NAME generate_five_user_names COMMAND usergen -c 5)
