{
  inputs.nixpkgs.url = "github:nixos/nixpkgs";

  outputs =
    { self, nixpkgs }:
    let
      system = "x86_64-linux";
      pkgs = nixpkgs.legacyPackages.${system};
    in
    {
      devShells.${system}.default = pkgs.mkShell {
        buildInputs = with pkgs; [
          # List individual packages here
          qt6.qtbase
          qt6.qttools
          qt6.qtdeclarative
          qt6.qt5compat
          qt6.qtwebengine # This contains the WebEngineWidgets config

          cmake
          gnumake
          gcc
          gdb
          qtcreator
        ];

        # Fixes the linking issue for CMake
        QT_PLUGIN_PATH = "${pkgs.qt6.qtbase}/lib/qt-6/plugins";
      };
    };
}
