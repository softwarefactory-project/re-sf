type eclass =
  | Job
  | Semaphore
  | Project
  | ProjectTemplate
  | Nodeset
  | Secret;

let eclass_encode = (c: eclass): Js.Json.t => {
  switch (c) {
  | Job => "job"->Js.Json.string
  | Semaphore => "semaphore"->Js.Json.string
  | Project => "project"->Js.Json.string
  | ProjectTemplate => "project-template"->Js.Json.string
  | Nodeset => "nodeset"->Js.Json.string
  | Secret => "secret"->Js.Json.string
  };
};

let eclass_decode = (json: Js.Json.t): Decco.result(eclass) => {
  json
  ->Decco.stringFromJson
  ->Belt.Result.flatMap(str => {
      switch (str) {
      | "job" => Job->Ok
      | "semaphore" => Semaphore->Ok
      | "project" => Project->Ok
      | "projectTemplate" => ProjectTemplate->Ok
      | "nodeset" => Nodeset->Ok
      | "secret" => Secret->Ok
      | _ => Decco.error("Invalid Zuul configuration class", json)
      }
    });
};
type eclasses = option(list(eclass));
let eclasses_encode = (eclasses: eclasses) =>
  Decco.optionToJson(Decco.listToJson(eclass_encode), eclasses);
let eclasses_decode = (jsonObject: Js.Json.t): Decco.result(eclasses) =>
  Decco.optionFromJson(Decco.listFromJson(eclass_decode), jsonObject);
let eclasses_codec = (eclasses_encode, eclasses_decode);

[@decco]
type t = {
  name: string,
  connection: option(string),
  [@decco.key "zuul/include"]
  zuul_include: [@decco.codec eclasses_codec] eclasses,
};

// Unfortunately, a source-repository can be defined either by a name or it's type
[@decco]
type union =
  | Full(t)
  | Name(string);

let getName = (union: union): string =>
  switch (union) {
  | Full(t) => t.name
  | Name(s) => s
  };

let getConnection = (union: union): option(string) =>
  switch (union) {
  | Full(t) => t.connection
  | Name(_) => None
  };

let encode = (union: union): Js.Json.t =>
  switch (union) {
  | Full(full) => Utils.encodeDictName(t_encode(full)) |> Belt.Option.getExn
  | Name(str) => Js.Json.string(str)
  };

let decode = (jsonObject: Js.Json.t): Decco.result(union) =>
  switch (Decco.stringFromJson(jsonObject)) {
  | Ok(s) => s->Name->Ok
  | Error(_) =>
    // Try to decode the '{name: info_dict}' form
    switch (Utils.decodeOneDictName(jsonObject)) {
    | Some(obj) => t_decode(obj)->Belt.Result.map(v => Full(v))
    | None => Decco.error("Unknown source repository", jsonObject)
    }
  };

type unions = list(union);
let encodes = (unions: unions) => Decco.listToJson(encode, unions);
let decodes = (jsonObject: Js.Json.t) =>
  Decco.listFromJson(decode, jsonObject);
let codec = (encodes, decodes);
