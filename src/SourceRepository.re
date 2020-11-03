[@decco]
type t = {
  name: string,
  connection: option(string),
  // This is not provided by the resource schema yet, we use it as a place
  // holder to lift repo description into the project schema.
  description: option(string),
};

// Unfortunately, a source-repository can be defined either by a name or it's type
[@decco]
type union =
  | Full(t)
  | Name(string);

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
