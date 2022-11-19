with import <nixpkgs> {};
stdenv.mkDerivation {
  name = "build-env";
  buildInputs = [ pkg-config cmake SDL2 SDL2_ttf SDL2_gfx ];
}
