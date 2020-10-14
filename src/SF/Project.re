type decoder('item) = Js.Json.t => 'item;

// Do not handle other potential fields as not needed for our main usage
type sourceRepository = {
  name: string,
  connection: option(string),
  // This is not provided by the resource schema yet, we use it as a place
  // holder to lift repo description into the project schema.
  description: option(string),
};

type project = {
  name: string,
  description: string,
  tenant: option(string),
  connection: option(string),
  website: option(string),
  documentation: option(string),
  issue_tracker_url: option(string),
  review_dashboard: option(string),
  mailing_lists: option(list(string)),
  contacts: option(list(string)),
  source_repositories: list(sourceRepository),
  options: option(list(string)),
};

let decodeSourceRepositoryDict = (name: Js.Dict.key, json: Js.Json.t) => {
  Json.Decode.{
    name,
    connection: json |> optional(field("connection", string)),
    description: None,
  };
};

// let decodeSourceRepository = (json: Js.Json.t): sourceRepository => {
let decodeSourceRepository: decoder(sourceRepository) =
  json => {
    switch (Js.Json.decodeObject(json)) {
    | Some(dict) =>
      switch (Js.Dict.entries(dict) |> Belt.List.fromArray) {
      | [(k, v)] => decodeSourceRepositoryDict(k, v)
      | _ =>
        "SourceRepository dictionary can only contain one value"
        |> Util.decodeFail
      }
    | None =>
      Json.Decode.{name: json |> string, connection: None, description: None}
    };
  };

let parse = json => {
  Json.Decode.{
    name: json |> field("name", string),
    description: json |> field("description", string),
    tenant: json |> optional(field("tenant", string)),
    connection: json |> optional(field("connection", string)),
    website: json |> optional(field("website", string)),
    documentation: json |> optional(field("documentation", string)),
    issue_tracker_url: json |> optional(field("issue-tracker-url", string)),
    review_dashboard: json |> optional(field("review-dashboard", string)),
    mailing_lists: json |> optional(field("mailing-lists", list(string))),
    contacts: json |> optional(field("contacts", list(string))),
    source_repositories:
      json |> field("source-repositories", list(decodeSourceRepository)),
    options: json |> optional(field("options", list(string))),
  };
};

/* optionEq compares an optional value `x` with a value `y` using the (==) operator */
let optionEq = (x: option('t), y: 't) => Belt.Option.eq(x, Some(y), (==))

let filterProjectsByTenant = (tenant: string, projects: list(project)) => {
  projects->Belt.List.keep(project => project.tenant->optionEq(tenant));
};
