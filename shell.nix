with import <nixpkgs> { };
let
in
stdenv.mkDerivation {
  name = "gcc";
  hardeningDisable = [ "format" ];
  buildInputs =
    let in
    [
      gmp
      mpfr
      libmpc
      
      flex
      bison

      gnumake
      gcc
    ];
}
