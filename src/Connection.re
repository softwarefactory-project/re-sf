type connection_type =
  | Git
  | Pagure
  | Gerrit
  | Gitlab
  | Github;

let connection_type_encode = (ct: connection_type): Js.Json.t =>
  Js.Json.string(
    switch (ct) {
    | Git => "git"
    | Pagure => "pagure"
    | Gerrit => "gerrit"
    | Gitlab => "gitlab"
    | Github => "github"
    },
  );

let connection_type_decode = (json: Js.Json.t): Decco.result(connection_type) => {
  switch (json |> Js.Json.decodeString) {
  | Some("git") => Git->Ok
  | Some("gerrit") => Gerrit->Ok
  | Some("gitlab") => Gitlab->Ok
  | Some("pagure") => Pagure->Ok
  | Some("github") => Github->Ok
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
