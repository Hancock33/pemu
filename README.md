[![linux-release](https://github.com/Cpasjuste/pemu/actions/workflows/linux-release.yml/badge.svg)](https://github.com/Cpasjuste/pemu/actions/workflows/linux-release.yml)
[![linux-dev](https://github.com/Cpasjuste/pemu/actions/workflows/linux-dev.yml/badge.svg)](https://github.com/Cpasjuste/pemu/actions/workflows/linux-dev.yml)

[![windows-release](https://github.com/Cpasjuste/pemu/actions/workflows/windows-release.yml/badge.svg)](https://github.com/Cpasjuste/pemu/actions/workflows/windows-release.yml)
[![windows-dev](https://github.com/Cpasjuste/pemu/actions/workflows/windows-dev.yml/badge.svg)](https://github.com/Cpasjuste/pemu/actions/workflows/windows-dev.yml)

[![switch-release](https://github.com/Cpasjuste/pemu/actions/workflows/switch-release.yml/badge.svg)](https://github.com/Cpasjuste/pemu/actions/workflows/switch-release.yml)
[![switch-dev](https://github.com/Cpasjuste/pemu/actions/workflows/switch-dev.yml/badge.svg)](https://github.com/Cpasjuste/pemu/actions/workflows/switch-dev.yml)

[![3ds-release](https://github.com/Cpasjuste/pemu/actions/workflows/3ds-release.yml/badge.svg)](https://github.com/Cpasjuste/pemu/actions/workflows/3ds-release.yml)
[![3ds-dev](https://github.com/Cpasjuste/pemu/actions/workflows/3ds-dev.yml/badge.svg)](https://github.com/Cpasjuste/pemu/actions/workflows/3ds-dev.yml)

[![ps4-release](https://github.com/Cpasjuste/pemu/actions/workflows/ps4-release.yml/badge.svg)](https://github.com/Cpasjuste/pemu/actions/workflows/ps4-release.yml)
[![ps4-dev](https://github.com/Cpasjuste/pemu/actions/workflows/ps4-dev.yml/badge.svg)](https://github.com/Cpasjuste/pemu/actions/workflows/ps4-dev.yml)

[![vita-release](https://github.com/Cpasjuste/pemu/actions/workflows/vita-release.yml/badge.svg)](https://github.com/Cpasjuste/pemu/actions/workflows/vita-release.yml)
[![vita-dev](https://github.com/Cpasjuste/pemu/actions/workflows/vita-dev.yml/badge.svg)](https://github.com/Cpasjuste/pemu/actions/workflows/vita-dev.yml)

## pEMU: Portable Emulator

**<ins>Building on ubuntu for ubuntu</ins>**
- Install build dependencies:
    ```
    sudo apt -yq update
    sudo apt -yq install \
        git zip build-essential cmake xxd \
        liblzma-dev libsdl2-dev libconfig-dev libcurl4-openssl-dev libtinyxml2-dev \
        libconfig-dev libglm-dev libfreetype6-dev libpng-dev libminizip-dev zlib1g-dev \
        libvdpau-dev libva-dev libavcodec-dev libavfilter-dev libavformat-dev libavutil-dev \
        libpostproc-dev libswresample-dev libswscale-dev libmpv-dev libass-dev libbz2-dev \
        libfribidi-dev libzip-dev libarchive-dev libfuse-dev
    ```
- clone pemu repository:
    ```
    git clone --recursive https://github.com/Cpasjuste/pemu.git`
    ```
- build (replace `pfbneo` with emulator target: `pfbneo`, `psnes`, `pnes`, `pgen` or remove this option for all emulators):
	```
	mkdir cmake-build && cd cmake-build
	cmake -G "Unix Makefiles" -DPLATFORM_LINUX=ON -DOPTION_BUILTIN_LIBCONFIG=ON \
        -DOPTION_EMU=pfbneo -DCMAKE_BUILD_TYPE=Release ..
	make pfbneo.deps
	make -j $(getconf _NPROCESSORS_ONLN) pfbneo_linux_release
	```

**<ins>Building on ubuntu for nintendo switch</ins>**
- Install build dependencies:
    ```
    sudo apt -yq update
    sudo apt -yq install zip git autoconf libtool automake build-essential cmake
    ```
- Install switch toolchain:
    ```
    wget https://github.com/devkitPro/pacman/releases/download/v1.0.2/devkitpro-pacman.amd64.deb
    sudo dpkg -i devkitpro-pacman.amd64.deb
    sudo dkp-pacman -Syu
    sudo dkp-pacman --noconfirm -S switch-dev switch-portlibs
    ```
- clone pemu repository:
    ```
    git clone --recursive https://github.com/Cpasjuste/pemu.git`
    ```
- build (replace `pfbneo` with emulator target: `pfbneo`, `psnes`, `pnes`, `pgen` or remove this option for all emulators):
    ```
    mkdir cmake-build && cd cmake-build
    source /etc/profile.d/devkit-env.sh
    cmake -G "Unix Makefiles" -DPLATFORM_SWITCH=ON -DOPTION_EMU=pfbneo \
        -DCMAKE_BUILD_TYPE=Release ..
    make pfbneo.deps
    make -j $(getconf _NPROCESSORS_ONLN) pfbneo_switch_release
    ```

**<ins>Building on ubuntu for sony ps4</ins>**
- Install build dependencies:
    ```
    sudo apt -yq update
    sudo apt -yq install git build-essential cmake zip libgpgme11
    ```
- Install ps4 toolchain:
    ```
    wget https://github.com/PacBrew/pacbrew-pacman/releases/download/pacbrew-release-1.0/pacbrew-pacman-1.0.deb
    sudo dpkg -i pacbrew-pacman-1.0.deb
    sudo pacbrew-pacman -Syu
    sudo pacbrew-pacman --noconfirm -S ps4-openorbis ps4-openorbis-portlibs
    ```
- clone pemu repository:
    ```
    git clone --recursive https://github.com/Cpasjuste/pemu.git`
    ```
- build (replace `pfbneo` with emulator target: `pfbneo`, `psnes`, `pnes`, `pgen` or remove this option for all emulators):
    ```
    mkdir cmake-build && cd cmake-build
    export OPENORBIS=/opt/pacbrew/ps4/openorbis
    cmake -G "Unix Makefiles" -DPLATFORM_PS4=ON -DOPTION_EMU=pfbneo -DCMAKE_BUILD_TYPE=Release ..
    make pfbneo.deps
    make -j $(getconf _NPROCESSORS_ONLN) pfbneo_ps4_release
    ```

**<ins>Building on windows for windows</ins>**
- download, install and update [MYSYS2](http://www.msys2.org/)
  - install build dependencies:
    ```
      pacman -S --noconfirm --needed git zip perl make \
        mingw-w64-x86_64-binutils mingw-w64-x86_64-gcc mingw-w64-x86_64-make mingw-w64-x86_64-cmake mingw-w64-x86_64-pkgconf \
        mingw-w64-x86_64-SDL2 mingw-w64-x86_64-freetype mingw-w64-x86_64-glm \
        mingw-w64-x86_64-glew mingw-w64-x86_64-libconfig mingw-w64-x86_64-tinyxml2 \
        mingw-w64-x86_64-mpv mingw-w64-x86_64-zlib mingw-w64-x86_64-minizip mingw-w64-x86_64-libarchive
    ```
- clone pemu repository:
    ```
    git clone --recursive https://github.com/Cpasjuste/pemu.git`
    ```
- build (replace `pfbneo` with emulator target: `pfbneo`, `psnes`, `pnes`, `pgen` or remove this option for all emulators):
    ```
    mkdir cmake-build && cd cmake-build
    cmake -G "MSYS Makefiles" -DPLATFORM_WINDOWS=ON -DOPTION_EMU=pfbneo -DCMAKE_BUILD_TYPE=Release ..
    make pfbneo.deps
    make pfbneo
    ```
