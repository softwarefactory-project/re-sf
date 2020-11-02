type resources = {
  projects: option(list(Project.project)),
  connections: option(list(Connection.connection)),
  tenants: option(list(Tenant.tenant)),
  repos: option(list(Repo.repo)),
};
type top = {resources};

let decodeRObjects = (parser, json: Js.Json.t) => {
  switch (Js.Json.decodeObject(json)) {
  | Some(obj_dict) =>
    let olist = obj_dict |> Js.Dict.values |> Belt.List.fromArray;
    Belt.List.map(olist, parser);
  | None => "Unable to decode resources objects" |> Util.decodeFail
  };
};

let parseResources = (json: Js.Json.t): resources => {
  Json.Decode.{
    tenants: json |> optional(field("tenants", decodeRObjects(Tenant.parse))),
    projects: json |> optional(field("projects", decodeRObjects(Project.parse))),
    connections:
    json |> optional(field("connections", decodeRObjects(Connection.parse))),
    repos: json |> optional(field("repos", Repo.parse))
  };
};

let parse = (json: Js.Json.t): top => {
  Json.Decode.{resources: json |> field("resources", parseResources)};
};
