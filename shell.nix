{
  pkgs ? import <nixpkgs> { },
}:
pkgs.mkShell {
  buildInputs = with pkgs; [
    qt6.qtbase
    qt6.qt5compat
    qt6.qttools
    qt6.qtpositioning
    qt6.qtdeclarative

    libglvnd
    qtcreator
    gcc
    gnumake
    cmake
    gdb
  ];
  shellHook = ''
    echo "Entering Qt6 development environment"
    # Set up Qt6 library paths for linking
    export QT_QPA_PLATFORM="wayland;xcb"
  '';
}
