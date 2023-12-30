with import <nixpkgs> { };
let
in
stdenv.mkDerivation {
  name = "gcc";
  buildInputs =
    let in
    [
      gmp
      mpfr
      libmpc

      gnumake
      gcc
    ];
}
