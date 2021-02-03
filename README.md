# re-sf

ReasonML JSON parser for [Software-Factory][Software-Factory] resources.


## Install

Add to your `package.json`:

```
yarn install @softwarefactory-project/re-sf
```

Add to your `bsconfig.json`:

```diff
"bs-dependencies": [
+  "@softwarefactory-project/re-sf"
]
```

## Run re-sf tests

```bash
yarn test
```

[Software-Factory]: https://softwarefactory-project.io/docs/


## Build native cli

First setup the toolchain:

```bash
# Setup opam
sudo dnf install -y opam
opam init  # press enters, keep default answer
# Setup ocaml version required by reason
opam switch create ocaml-reason 4.10.1
opam switch ocaml-reason
# Ensure env is correct
export PATH=~/.opam/ocaml-reason/bin:$PATH
export OPAM_SWITCH_PREFIX=~/.opam/ocaml-reason
# Install requirements
opam install reason clap
```

Then build the project:

```bash
dune build @install
dune build sf.exe && ./_build/default/sf.exe
```
