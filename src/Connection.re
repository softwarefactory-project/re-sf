type connection_type =
  | GIT
  | PAGURE
  | GERRIT
  | GITLAB
  | GITHUB;

let connection_type_encode = (ct: connection_type): Js.Json.t =>
  Js.Json.string(
    switch (ct) {
    | GIT => "git"
    | PAGURE => "pagure"
    | GERRIT => "gerrit"
    | GITLAB => "gitlab"
    | GITHUB => "github"
    },
  );

let connection_type_decode = (json: Js.Json.t): Decco.result(connection_type) => {
  switch (json |> Js.Json.decodeString) {
  | Some("git") => GIT->Ok
  | Some("gerrit") => GERRIT->Ok
  | Some("pagure") => PAGURE->Ok
  | Some("github") => GITHUB->Ok
  | _ => Decco.error("unknown connection type", json)
  };
};

[@decco]
type t = {
  name: string,
  [@decco.key "base-url"]
  base_url: option(string),
  [@decco.key "type"]
  connection_type,
};

let decode = t_decode;
let encode = t_encode;
