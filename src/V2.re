module SFV1 = SF;

let mapFilterMap = (x, f, g) =>
  x
  ->Belt.Option.map(y => y->Belt.List.keep(f)->Belt.List.map(g))
  ->Belt.Option.getWithDefault([]);

let mapMap = (x, f) => mapFilterMap(x, _ => true, f);

module Connection = {
  [@decco]
  type t =
    | Gerrit(string)
    | Pagure(string)
    | GitLab(string)
    | GitHub(string)
    | Git(string);

  let cloneUrl = (connection: t, project: string): string =>
    switch (connection) {
    | Gerrit(url) => url ++ "/" ++ project
    | Pagure(url) => url ++ "/" ++ project
    | GitLab(url) => url ++ "/" ++ project
    | GitHub(url) => url ++ "/" ++ project
    | Git(url) => url ++ "/" ++ project ++ ".git"
    };

  let webUrl = (connection: t, project: string): string =>
    switch (connection) {
    | Gerrit(url) => url ++ "/gitweb?p=" ++ project ++ ".git;a=tree"
    | Pagure(url) => url ++ "/" ++ project ++ "/tree"
    | GitLab(url) => url ++ "/" ++ project
    | GitHub(url) => url ++ "/" ++ project
    | Git(_url) => ""
    };

  let commitUrl = (connection: t, project: string): string =>
    switch (connection) {
    | Gerrit(url) => url ++ "/gitweb?p=" ++ project ++ ".git;a=shortlog"
    | Pagure(url) => url ++ "/" ++ project ++ "/commits"
    | GitLab(url) => url ++ "/" ++ project ++ "/commits"
    | GitHub(url) => url ++ "/" ++ project ++ "/commits"
    | Git(_url) => ""
    };

  let changesUrl = (connection: t, project: string): string =>
    switch (connection) {
    | Gerrit(url) => url ++ "/q/status:open+project:" ++ project
    | Pagure(url) => url ++ "/" ++ project ++ "/pull-requests"
    | GitLab(url) => url ++ "/" ++ project ++ "/merge_requests"
    | GitHub(url) => url ++ "/" ++ project ++ "/pulls"
    | Git(_url) => ""
    };

  let fromV1 =
      (connections: list(Connection.t), name: string)
      : Belt.Result.t(t, string) => {
    switch (connections->Belt.List.keep(c => c.name == name)->Belt.List.head) {
    | Some(c) =>
      switch (c.connection_type, c.base_url) {
      | (SFV1.Connection.Gerrit, Some(url)) => Gerrit(url)->Ok
      | (SFV1.Connection.Pagure, Some(url)) => Pagure(url)->Ok
      | (SFV1.Connection.Gitlab, Some(url)) => GitLab(url)->Ok
      | (SFV1.Connection.Github, Some(url)) => GitHub(url)->Ok
      | (SFV1.Connection.Git, Some(url)) => Git(url)->Ok
      | (_, None) => c.name->Error
      }
    | None => name->Error
    };
  };
};

let getLocal = (res: SFV1.Resources.t, name: string): option(SFV1.Repo.t) =>
  res.repos
  ->Belt.Option.flatMap(repos =>
      repos->Belt.List.keep(r => r.name == name)->Belt.List.head
    );

module SourceRepository = {
  [@decco]
  type t = {
    name: string,
    description: option(string),
    location: Connection.t,
  };
  let fromV1 =
      (
        res: Resources.t,
        project: SFV1.Project.t,
        tenant: SFV1.Tenant.t,
        sr: SFV1.SourceRepository.union,
      )
      : Belt.Result.t(t, string) => {
    let name = SFV1.SourceRepository.getName(sr);
    let isLocal = getLocal(res, name);
    let connectionName =
      SFV1.SourceRepository.getConnection(sr)
      ->Belt.Option.getWithDefault(
          project.connection
          ->Belt.Option.getWithDefault(
              tenant.default_connection->Belt.Option.getWithDefault(""),
            ),
        );
    let description = isLocal->Belt.Option.map(r => r.description);
    Connection.fromV1(
      res.connections->Belt.Option.getWithDefault([]),
      connectionName,
    )
    ->Belt.Result.flatMap(location => {name, description, location}->Ok);
  };
};

module Project = {
  [@decco]
  type t = {
    name: string,
    description: string,
    [@decco.key "source-repositories"]
    source_repositories: list(SourceRepository.t),
    website: option(string),
    documentation: option(string),
    [@decco.key "issue-tracker-url"]
    issue_tracker_url: option(string),
    [@decco.key "mailing-lists"]
    mailing_lists: option(list(string)),
    contacts: option(list(string)),
  };
  let fromV1 =
      (res: Resources.t, tenant: SFV1.Tenant.t, project: SFV1.Project.t): t => {
    name: project.name,
    description: project.description,
    source_repositories:
      project.sourceRepositories
      ->Belt.List.map(sr =>
          switch (SourceRepository.fromV1(res, project, tenant, sr)) {
          | Ok(sr) => [sr]
          // TODO: handle the case where a source-repository does not have a valid connection
          // though this should already be enforced by the managesf resource engine
          | Error(_) => []
          }
        )
      ->Belt.List.flatten,
    website: project.website,
    documentation: project.documentation,
    issue_tracker_url: project.issue_tracker_url,
    mailing_lists: project.mailing_lists,
    contacts: project.contacts,
  };
};

module Tenant = {
  [@decco]
  type t = {
    name: string,
    projects: list(Project.t),
    description: option(string),
  };
  let fromV1 =
      (defaultTenant: string, res: Resources.t, tenant: SFV1.Tenant.t): t => {
    let isMyProject = (project: SFV1.Project.t): bool =>
      switch (project.tenant) {
      // This project has a tenant name, so check if it is the tenant we are loading
      | Some(projectTenant) => tenant.name == projectTenant
      // This project does not have a tenant, so check the current tenant is the default
      | None => tenant.name == defaultTenant
      };
    {
      name: tenant.name,
      description: tenant.description,
      projects:
        res.projects->mapFilterMap(isMyProject, Project.fromV1(res, tenant)),
    };
  };
};

[@decco]
type t = {tenants: list(Tenant.t)};

let fromV1 = (~defaultTenant: string, res: Resources.t): t => {
  {tenants: res.tenants->mapMap(Tenant.fromV1(defaultTenant, res))};
};

let toV1 = (_v2: t): Resources.t => {
  tenants: None,
  projects: None,
  connections: None,
  repos: None,
  groups: None,
};
