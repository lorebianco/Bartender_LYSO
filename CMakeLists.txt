cmake_minimum_required(VERSION 3.0 FATAL_ERROR)

project(my_Bartender)

# Impostazione dei percorsi per ROOT
list(APPEND CMAKE_PREFIX_PATH $ENV{ROOTSYS})
find_package(ROOT REQUIRED)
include(${ROOT_USE_FILE})

# Includi le directory dei file di intestazione
include_directories(${ROOT_INCLUDE_DIRS})
include_directories(${PROJECT_SOURCE_DIR}/include)

# Includi le directory delle librerie
link_directories(${ROOT_LIBRARY_DIR})

# Trova tutti i file sorgente
file(GLOB sources ${PROJECT_SOURCE_DIR}/src/*.cc)
file(GLOB headers ${PROJECT_SOURCE_DIR}/include/*.hh)

# Trova i file di macro e li copia nella directory binaria
file(GLOB MACRO_FILES "*.mac")
file(COPY ${MACRO_FILES} DESTINATION ${PROJECT_BINARY_DIR})

# Aggiungi l'eseguibile bartender
add_executable(bartender_lyso bartender.cc ${sources} ${headers})
target_link_libraries(bartender_lyso ${ROOT_LIBRARIES})

#Attach dictionaries to the executable. First, tell it where to look for headers required by the dictionaries:
ROOT_GENERATE_DICTIONARY(bartender_dict ${headers} MODULE bartender_lyso LINKDEF LinkDef.h)

add_library(bartenderlib SHARED ${sources} bartender_dict.cxx)
target_link_libraries(bartenderlib ${ROOT_LIBRARIES})


# Definisci il target personalizzato per la generazione di entrambi gli eseguibili
add_custom_target(my_Bartender DEPENDS bartender_lyso)


# Aggiungi l'eseguibile bartenderMT includendo solo summary.hh
#add_executable(bartenderMT bartenderMT.cc ${PROJECT_SOURCE_DIR}/include/summary.hh  ${PROJECT_SOURCE_DIR}/src/summary.cc)
#add_custom_target(my_Bartender ALL DEPENDS bartender bartenderMT)