type resources = {
  projects: list(Project.project),
  connections: list(Connection.connection),
  tenants: list(Tenant.tenant),
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
    tenants: json |> field("tenants", decodeRObjects(Tenant.parse)),
    projects: json |> field("projects", decodeRObjects(Project.parse)),
    connections:
      json |> field("connections", decodeRObjects(Connection.parse)),
  };
};

let parse = (json: Js.Json.t): top => {
  Json.Decode.{resources: json |> field("resources", parseResources)};
};
