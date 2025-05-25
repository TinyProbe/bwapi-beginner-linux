#!/bin/bash

# save argument
ARGC="$#"
ARGV=("$0" "$1" "$2" "$3")

BOT_NAME="$(cat ./Makefile | grep 'TARGET := ' | grep -oE '([a-zA-Z_]+.exe)')"

function error {
    echo "usage:                                                                "
    echo "    ./run.sh <command> [ bot_race [ other_race ] ]                    "
    echo "                                                                      "
    echo "commands:                                                             "
    echo "    single      Play against the computer in single player using bots "
    echo "                you've written.                                       "
    echo "    versus      Bots in the bin directory compete against each other. "
    echo "    self        You will compete directly against the bot you created."
    echo "                                                                      "
    echo "options:                                                              "
    echo "    random      (default)                                             "
    echo "    protoss                                                           "
    echo "    terran                                                            "
    echo "    zerg                                                              "
    echo "                                                                      "
    echo "    [bot_race]      Select the race of the bot you created.           "
    echo "    [other_race]    Choose computer, opponent bots or your own race.  "
    echo "                                                                      "
    exit 1
}

function initial {
    # no argument
    if [ "$ARGC" -lt 1 ]; then
        error
    fi

    # check compiler version
    MINGW32_VERSION="$(i686-w64-mingw32-g++ --version | grep -o '..-win32')"

    # check installed
    if [ -z "$MINGW32_VERSION" ]; then
        echo "Check the README.md file:"
        echo "    Install Mingw-w64 using: `sudo apt install build-essential mingw-w64`."
        exit 1
    fi

    if [ ! -d "./bin" ]; then
        mkdir bin
    fi

    # check runtime library: libstdc++-6.dll
    if [ ! -f "./bin/libstdc++-6.dll" ]; then
        cp "/usr/lib/gcc/i686-w64-mingw32/$MINGW32_VERSION/libstdc++-6.dll" "./bin/"
    fi

    # check runtime library: libgcc_s_dw2-1.dll
    if [ ! -f "./bin/libgcc_s_dw2-1.dll" ]; then
        cp "/usr/lib/gcc/i686-w64-mingw32/$MINGW32_VERSION/libgcc_s_dw2-1.dll" "./bin/"
    fi

    # compile replacer
    if [ ! -f "./tools/repl" ]; then
        g++ -Wall -Wextra -Werror --std=c++17 -O2 -o ./tools/repl ./tools/replacer/replacer.cpp
    fi

    # copy default bwapi.ini
    cp -rf ./tools/bwapi.ini ./starcraft/bwapi-data/bwapi.ini
}

# correction character cases in word
function correction {
    if [ "$#" -ne 1 ]; then
        echo "invalid argument: correction <word>"
        return 1
    fi
    local word="$1"
    local prefix="$(echo ${word:0:1} | tr 'a-z' 'A-Z')"
    local suffix="$(echo ${word:1} | tr 'A-Z' 'a-z')"
    echo "$prefix$suffix"
}

# edit options in bwapi.ini
function replace {
    if [ "$#" -lt 1 ]; then
        echo "invalid argument: replace '<variable> = <value>' [ ... ]"
        return 1
    fi
    ./tools/repl "$@" < ./starcraft/bwapi-data/bwapi.ini > ./starcraft/bwapi-data/bwapi.ini.new
    rm ./starcraft/bwapi-data/bwapi.ini
    mv ./starcraft/bwapi-data/bwapi.ini.new ./starcraft/bwapi-data/bwapi.ini
}

function single_mode {
    if [ "$ARGC" -ge 2 ]; then
        replace "race = $(correction ${ARGV[2]})"
    fi
    if [ "$ARGC" -ge 3 ]; then
        replace "enemy_race = $(correction ${ARGV[3]})"
    fi
    wine cmd /c ./tools/run_bot.bat "./bin/$BOT_NAME"
    wine cmd /c ./tools/run_scbw.bat
}

function versus_mode {
    local bot_list="$(ls -al bin | grep '.exe' | grep -oE '([a-zA-Z_]+.exe)')"
    local old_ifs="$IFS"
    IFS="
    "
    local i=0
    for bot in $bot_list; do
        BOTS[$i]="$bot";
        i=$(expr "$i" + 1)
    done
    IFS="$old_ifs"

    if [[ "${#BOTS[@]}" -lt 1 || "${#BOTS[@]}" -gt 2 ]]; then
        echo "versus mode:                                                         "
        echo "    ./bin/*.exe    There should be one or two bot executables in the "
        echo "                   ./bin/ directory.                                 "
        echo "                                                                     "
        exit 1
    fi

    local other_bot="${BOTS[0]}"
    if [ "${#BOTS[@]}" -eq 2 ]; then
        if [ "$BOT_NAME" = "${BOTS[0]}" ]; then
            other_bot="${BOTS[1]}"
        fi
    fi

    replace "auto_menu = LAN"

    if [ "$ARGC" -ge 2 ]; then
        replace "race = $(correction ${ARGV[2]})"
    fi
    wine cmd /c ./tools/run_bot.bat "./bin/$BOT_NAME"
    wine cmd /c ./tools/run_scbw.bat

    sleep 3

    TODO: cannot create process of other bot

    local other_race="Random"
    if [ "$ARGC" -ge 3 ]; then
        other_race="$(correction ${ARGV[3]})"
    fi
    replace "race = $other_race" "character_name = other-bot" "game = JOIN_FIRST"
    wine cmd /c ./tools/run_bot.bat "./bin/$other_bot"
    wine cmd /c ./tools/run_scbw.bat
}

function self_mode {
    replace "auto_menu = LAN" "sound = OFF"

    if [ "$ARGC" -ge 2 ]; then
        replace "race = $(correction ${ARGV[2]})"
    fi
    wine cmd /c ./tools/run_bot.bat "./bin/$BOT_NAME"
    wine cmd /c ./tools/run_scbw.bat

    sleep 3

    local other_race="Random"
    if [ "$ARGC" -ge 3 ]; then
        other_race="$(correction ${ARGV[3]})"
    fi
    replace "race = $other_race" "character_name = player" "game = JOIN_FIRST" "sound = ON"
    wine cmd /c ./tools/run_scbw.bat
}

########## begin process ##########

initial

if [ "${ARGV[1]}" = "single" ]; then
    single_mode
elif [ "${ARGV[1]}" = "versus" ]; then
    versus_mode
elif [ "${ARGV[1]}" = "self" ]; then
    self_mode
else                # wrong argument
    error
fi
