type t = {
  projects: option(list(Project.t)),
  connections: option(list(Connection.t)),
  tenants: option(list(Tenant.t)),
  repos: option(list(Repo.t)),
  groups: option(list(Group.t)),
};

open Belt;

// A special list decoder for { name: object_t } dictionary
let decodeList =
    (
      jsonDict: Js.Dict.t(Js.Json.t),
      key: string,
      decoder: Decco.decoder('t),
    )
    : Decco.result(option(list('t))) => {
  switch (jsonDict->Js.Dict.get(key)) {
  | Some(v) =>
    v
    ->Utils.decodeDictName
    ->Utils.note(Utils.makeError("Invalid value", v))
    ->Result.flatMap(dicts => dicts->List.map(decoder)->Utils.sequenceR)
    ->{
        (
          rl =>
            switch (rl) {
            | Ok(l) => Ok(Some(l))
            | Error(_) => Ok(None)
            }
        );
      }
  | None => Ok(None)
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
      ->Js.Dict.get("resources")
      ->Utils.note(error)
      ->Result.flatMap(v =>
          v
          ->Js.Json.decodeObject
          ->Utils.note(error)
          ->Result.flatMap(resDict => decodeResources(resDict))
        )
    );
};

let listToJsonDict = (xs, encoder: Decco.encoder('a)): Js.Json.t =>
  xs
  ->List.map(encoder)
  ->List.map(Utils.encodeDictName)
  ->Utils.sequence
  ->Belt.Option.getExn
  ->List.map(x =>
      x
      ->Js.Json.decodeObject
      ->Option.flatMap(dict =>
          Js.Dict.entries(dict)->Belt.List.fromArray->Some
        )
    )
  ->Utils.sequence
  ->Belt.Option.getExn
  ->Belt.List.flatten
  ->Js.Dict.fromList
  ->Js.Json.object_;

let optionalListToJsonDict =
    (oxs: option(list('a)), encoder: Decco.encoder('a)): Js.Json.t => {
  switch (oxs) {
  | Some(xs) => listToJsonDict(xs, encoder)
  | None => Js.Json.null
  };
};

let encode = (resources: t): Js.Json.t => {
  Js.Json.object_(
    Js.Dict.fromList([
      (
        "resources",
        Js.Json.object_(
          Js.Dict.fromList(
            [
              (
                "connections",
                optionalListToJsonDict(
                  resources.connections,
                  Connection.encode,
                ),
              ),
              (
                "groups",
                optionalListToJsonDict(resources.groups, Group.encode),
              ),
              (
                "projects",
                optionalListToJsonDict(resources.projects, Project.encode),
              ),
              (
                "repos",
                optionalListToJsonDict(resources.repos, Repo.encode),
              ),
              (
                "tenants",
                optionalListToJsonDict(resources.tenants, Tenant.encode),
              ),
            ]
            ->Belt.List.keep(((_, json)) => !json->Utils.isNull),
          ),
        ),
      ),
    ]),
  );
};
