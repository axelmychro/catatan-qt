{
  pkgs ? import <nixpkgs> { },
}:
pkgs.mkShell {
  buildInputs = with pkgs; [
    qt6.qtbase
    qt6.qtbase
    qt6.qtwebengine
    qt6.qttools
    qt6.qtdeclarative
    qt6.qt5compat
    qt6.qtwebchannel
    qt6.qtpositioning

    cmake
    gnumake
    gcc
    gdb

    qtcreator
  ];
  shellHook = ''
    echo "Entering Qt6 development environment"
    # Set up Qt6 library paths for linking
    export QT_QPA_PLATFORM="wayland;xcb"
  '';
}
