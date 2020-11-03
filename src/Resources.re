type t = {
  projects: list(Project.t),
  connections: list(Connection.t),
  tenants: list(Tenant.t),
  repos: list(Repo.t),
  groups: list(Group.t),
};

open Belt;

// A special list decoder for { name: object_t } dictionary
let decodeList =
    (
      jsonDict: Js.Dict.t(Js.Json.t),
      key: string,
      decoder: Decco.decoder('t),
    )
    : Decco.result(list('t)) => {
  switch (jsonDict->Js.Dict.get(key)) {
  | Some(v) =>
    v
    ->Utils.decodeDictName
    ->Utils.note(Utils.makeError("Invalid value", v))
    ->Result.flatMap(dicts => dicts->List.map(decoder)->Utils.sequenceR)
  | None => Decco.error("Invalid list: " ++ key, Js.Json.object_(jsonDict))
  };
};

let decodeResources = (jsonObject: Js.Dict.t(Js.Json.t)): Decco.result(t) =>
  jsonObject
  ->decodeList("projects", Project.decode)
  ->Result.flatMap(projects =>
      jsonObject
      ->decodeList("connections", Connection.decode)
      ->Result.flatMap(connections =>
          jsonObject
          ->decodeList("tenants", Tenant.decode)
          ->Result.flatMap(tenants =>
              jsonObject
              ->decodeList("repos", Repo.decode)
              ->Result.flatMap(repos =>
                  jsonObject
                  ->decodeList("groups", Group.decode)
                  ->Result.flatMap(groups =>
                      {projects, connections, tenants, repos, groups}->Ok
                    )
                )
            )
        )
    );

let decode = (jsonObject: Js.Json.t): Decco.result(t) => {
  let error = Utils.makeError("Invalid object", jsonObject);
  jsonObject
  ->Js.Json.decodeObject
  ->Utils.note(error)
  ->Result.flatMap(topDict =>
      topDict
      ->Utils.decodeSingleKeyDict
      ->Utils.note(error)
      ->Result.flatMap(((keyName, v)) =>
          switch (keyName) {
          | "resources" =>
            v
            ->Js.Json.decodeObject
            ->Utils.note(error)
            ->Result.flatMap(resDict => decodeResources(resDict))
          | _ => error->Error
          }
        )
    );
};

let encode = (resources: t): Js.Json.t => {
  Js.Json.object_(
    Js.Dict.fromList([
      (
        "resources",
        Js.Json.object_(
          Js.Dict.fromList([
            (
              "projects",
              Decco.listToJson(Project.encode, resources.projects),
            ),
            (
              "connections",
              Decco.listToJson(Connection.encode, resources.connections),
            ),
            ("tenants", Decco.listToJson(Tenant.encode, resources.tenants)),
            ("repos", Decco.listToJson(Repo.encode, resources.repos)),
            ("groups", Decco.listToJson(Group.encode, resources.groups)),
          ]),
        ),
      ),
    ]),
  );
};
