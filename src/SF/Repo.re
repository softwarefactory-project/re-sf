type repo = {
  name: string,
  acl: string,
  description: string,
};

let parseRepo = ((name, json)) =>
  Json.Decode.{
    name,
    acl: json |> field("acl", string),
    description: json |> field("description", string),
  };

let parse = (json: Js.Json.t): list(repo) =>
  switch (Js.Json.decodeObject(json)) {
  | Some(dict) => dict->Js.Dict.entries->Belt.Array.map(parseRepo)->Belt.List.fromArray
  | None => "Repos is not a dictionary" |> Util.decodeFail
  };
