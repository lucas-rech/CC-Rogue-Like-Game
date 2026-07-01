cmake_minimum_required(VERSION 3.24)
project(CC_Rogue_Like_Game VERSION 0.0.5)

set(CMAKE_CXX_STANDARD 20)

# 1. Inclui o módulo do CMake para buscar conteúdo na internet
include(FetchContent)

# 2. Declara que queremos a SFML estável direto do GitHub oficial
FetchContent_Declare(
        SFML
        GIT_REPOSITORY https://github.com/SFML/SFML.git
        GIT_TAG        2.6.1
)

# 3. Desativa módulos que você não vai usar agora (acelera a compilação)
set(SFML_BUILD_AUDIO TRUE CACHE INTERNAL "")
set(SFML_BUILD_NETWORK FALSE CACHE INTERNAL "")

# 4. Baixa e disponibiliza a SFML no seu projeto
FetchContent_MakeAvailable(SFML)

# 4. Adiciona todos os arquivos-fonte do seu jogo
add_executable(CC_Rogue_Like_Game 
    src/main.cpp
    src/world/Level.cpp
    src/graphics/TileMap.cpp
        src/entities/hero/Player.hpp
        src/entities/hero/Player.cpp
        src/entities/enemies/Enemy.hpp
        src/entities/enemies/Enemy.cpp
        src/items/Item.cpp
        src/traps/Trap.cpp
        src/ui/Menu.cpp
        src/ui/HUD.cpp
        src/ui/GameOverScreen.cpp
        src/ui/VictoryScreen.cpp
        src/ui/AttributeMenu.cpp
        src/entities/npc/NPC.cpp
        src/entities/npc/Companion.cpp
        src/system/Ranking.cpp
        src/ui/RankingScreen.cpp
)

# Adiciona o diretório de includes
target_include_directories(CC_Rogue_Like_Game PRIVATE
    ${SFML_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/include
)

# 6. Linka as bibliotecas da SFML ao seu jogo
target_link_libraries(CC_Rogue_Like_Game sfml-graphics sfml-window sfml-system sfml-audio)

# 7. COPIAR TODAS AS DLLs NECESSÁRIAS AUTOMATICAMENTE (Para rodar o .exe por fora)
if(WIN32)
    # Descobre onde está a pasta 'bin' do compilador do CLion para pegar as DLLs do sistema
    get_filename_component(COMPILER_DIR "${CMAKE_CXX_COMPILER}" DIRECTORY)

    add_custom_command(TARGET CC_Rogue_Like_Game POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
            $<TARGET_FILE:sfml-graphics>
            $<TARGET_FILE:sfml-window>
            $<TARGET_FILE:sfml-system>
            $<TARGET_FILE:sfml-audio>
            "${SFML_SOURCE_DIR}/extlibs/bin/x64/openal32.dll"
            # Destino: A pasta onde o seu executável (.exe) está sendo gerado
            $<TARGET_FILE_DIR:CC_Rogue_Like_Game>
    )

    # Se for compilado usando MinGW (GNU), copie as DLLs exigidas
    if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
        add_custom_command(TARGET CC_Rogue_Like_Game POST_BUILD
                COMMAND ${CMAKE_COMMAND} -E copy_if_different
                "${COMPILER_DIR}/libgcc_s_seh-1.dll"
                "${COMPILER_DIR}/libstdc++-6.dll"
                "${COMPILER_DIR}/libwinpthread-1.dll"
                $<TARGET_FILE_DIR:CC_Rogue_Like_Game>
        )
    endif()
endif()

# 8. Copiar pastas de recursos (Assets e World) para o diretorio de build
add_custom_command(TARGET CC_Rogue_Like_Game POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    "${CMAKE_SOURCE_DIR}/src/assets"
    "$<TARGET_FILE_DIR:CC_Rogue_Like_Game>/assets"
)

add_custom_command(TARGET CC_Rogue_Like_Game POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    "${CMAKE_SOURCE_DIR}/src/world"
    "$<TARGET_FILE_DIR:CC_Rogue_Like_Game>/world"
)